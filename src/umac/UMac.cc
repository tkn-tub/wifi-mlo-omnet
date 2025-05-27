//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 


#include "UMac.h"

#include <string>
#include <vector>

#include <omnetpp.h>
#include "inet/common/Simsignals.h"
#include "inet/common/Protocol.h"
#include "inet/linklayer/common/MacAddress.h"
#include "inet/linklayer/common/InterfaceTag_m.h"

#include "inet/linklayer/common/UserPriority.h"
#include "inet/linklayer/common/UserPriorityTag_m.h"

#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/ipv6/Ipv6InterfaceData.h"

#include "inet/networklayer/common/NetworkInterface.h"

#include "inet/physicallayer/wireless/common/contract/packetlevel/SignalTag_m.h"

#include "../lmac/mac/Ieee80211beProtocol.h"

namespace inet {

using namespace std;

Register_Class(UMac);

void UMac::initialize(int stage)
{
    LayeredProtocolBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {

        isStation = par("isStation");

        // TODO: Add if classifier
        ift.reference(this, "interfaceTableModule", true);

        lowerLayerInGateId = findGate("lowerLayerIn");
        lowerLayerOutGateId = findGate("lowerLayerOut");

        lowerLayerMgmtInGateId = findGate("lowerLayerMgmtIn");
        lowerLayerMgmtOutGateId = findGate("lowerLayerMgmtOut");

        lowerLayerLlcInGateId = findGate("lowerLayerLlcIn");
        lowerLayerLlcOutGateId = findGate("lowerLayerLlcOut");

        upperLayerInGateId = findGate("upperLayerIn");
        upperLayerOutGateId = findGate("upperLayerOut");

        upperLayerMgmtInGateId = findGate("upperLayerMgmtIn");
        upperLayerMgmtOutGateId = findGate("upperLayerMgmtOut");

        upperLayerLlcInGateId = findGate("upperLayerLlcIn");
        upperLayerLlcOutGateId = findGate("upperLayerLlcOut");

        classifierInGateId = findGate("classifierIn");
        classifierOutGateId = findGate("classifierOut");

        rcvdPacketsUnique = registerSignal("rcvdPacketsUnique");

        cMessage* statisticsTimer = new cMessage("StatisticsTimer", 0);
        scheduleAt(2, statisticsTimer);

        defaultInterfaceName = par("defaultInterface");

    }
    else if (stage == INITSTAGE_LINK_LAYER) {
        // register service and protocol
        registerService(*ieee80211be, gate("upperLayerIn"), gate("upperLayerOut"));
        registerProtocol(*ieee80211be, gate("lowerLayerOut"), gate("lowerLayerIn"));

        cGate *classifierGate = gate("classifierOut")->getNextGate();
        hasClassifier = classifierGate != nullptr;

        reassignMacAddress();
    }
    else if (stage == INITSTAGE_NETWORK_LAYER) {
        if(isStation) {
            reassignNetworkAddress();
        }
    }
}

void UMac::reassignMacAddress() {

    // IMPORTANT: Need to get the address of the __first__ interface with the __smallest__ address to comply with Global ARP and OMNeT++ addressing
    NetworkInterface *firstIface = ift->getInterface(0);
    const auto unifiedMacAddr = firstIface->getMacAddress();

    for(int i = 1; i < ift->getNumInterfaces(); i++) {
        NetworkInterface *iface = ift->getInterface(i);
        iface->setMacAddress(unifiedMacAddr);
    }
}

void UMac::reassignNetworkAddress() {

    // IMPORTANT: Need to get the address of the __first__ interface with the __smallest__ address to comply with Global ARP and OMNeT++ addressing
    NetworkInterface *firstIface = ift->getInterface(0);
    const auto unifiedNetAddr = firstIface->getNetworkAddress();

    for(int i = 1; i < ift->getNumInterfaces(); i++) {
        NetworkInterface *iface = ift->getInterface(i);
        if(iface->findProtocolData<Ipv4InterfaceData>() != nullptr && unifiedNetAddr.getType() == L3Address::AddressType::IPv4)
            iface->getProtocolDataForUpdate<Ipv4InterfaceData>()->setIPAddress(unifiedNetAddr.toIpv4());
    }
}

void UMac::handleMessageWhenUp(cMessage *message)
{
    if (message->isSelfMessage())
        handleSelfMessage(message);
    else if (isUpperMessage(message))
        handleUpperMessage(message);
    else if (isLowerMessage(message))
        handleLowerMessage(message);
    else if (isClassifierMessage(message))
        handleClassifierMessage(message);
    else
        throw cRuntimeError("Message '%s' received on unexpected gate '%s'", message->getName(), message->getArrivalGate()->getFullName());
}

void UMac::handleUpperMessage(cMessage *message)
{
    if (!message->isPacket())
        handleUpperCommand(message);
    else {
        if(hasClassifier)
            send(message, classifierOutGateId);
        else
            handleUpperPacket(check_and_cast<Packet *>(message));
    }
}

void UMac::handleLowerMessage(cMessage *message)
{
    if (!message->isPacket()){
        handleLowerCommand(message);
    }
    else {
        handleLowerPacket(check_and_cast<Packet *>(message));
    }
}

void UMac::handleUpperPacket(Packet *packet) {
    //std::cout << this->getParentModule()->getName() << "." << this->getName() << "." <<  __func__ << "() logs - " << "Sent  " << packet->getName() << endl;

    // Remove dispatch requirement otherwise the MessageDispatchers
    // bounces the packets back.
    packet->removeTagIfPresent<DispatchProtocolReq>();

    if(isStation) {
        updateChannelAccessParameters();
        // IMPORTANT: This is only for test purposes to check link quality periodically.
        // This assume that there is a (test) UDP application sending packets through all available links
        // so that the destination STAs can check changing link quality, in the absence of duplex traffic.
        string packetName = packet->getName();
        if(packetName.find("UDPLinkQuality") != string::npos) {
            sendTestPacket(packet);
        } else {
            sendPacket(packet);
        }
    } else {
        // IMPORTANT: AP forward the frames on the same interface it received.
        int targetIftId = packet->getTag<InterfaceInd>()->getInterfaceId();
        NetworkInterface *targetIft = ift->getInterfaceById(targetIftId);
        if(targetIft->getState() == NetworkInterface::State::UP) {
            packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(targetIftId);
            send(packet, lowerLayerOutGateId);
        } else {
            // TODO: AP should not just remove the packet if the ingress interface is down.
            delete packet;
        }
    }
}

void UMac::sendPacket(Packet *packet) {
    // Sending to packets to the main interface, e.g., the first one registered.
    NetworkInterface *targetIft = ift->findInterfaceByName(defaultInterfaceName);
    if(targetIft->getState() == NetworkInterface::State::UP) {
        packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(targetIft->getInterfaceId());
        send(packet, lowerLayerOutGateId);
    } else {
        delete packet;
    }
}

void UMac::sendTestPacket(Packet *packet) {
    // Sending packets through all interfaces
    for(int i = 0; i < ift->getNumInterfaces(); i++) {
        NetworkInterface *targetIft = ift->getInterface(i);
        if(targetIft->getState() == NetworkInterface::State::UP) {
            Packet *duplicate = packet->dup();
            duplicate->removeTagIfPresent<InterfaceReq>();
            duplicate->addTagIfAbsent<InterfaceReq>()->setInterfaceId(targetIft->getInterfaceId());
            send(duplicate, lowerLayerOutGateId);
        }
    }
    delete packet;
}

void UMac::handleClassifierMessage(cMessage *message) {
    handleUpperPacket(check_and_cast<Packet *>(message));
}

void UMac::handleLowerPacket(Packet *packet) {
    //std::cout << this->getParentModule()->getName() << "." << this->getName() << "." <<  __func__ << "() logs - Received " << packet->getName() << " from " << packet->getSenderModule()->getParentModule()->getFullName() << endl;

    updateLinkQosParameters(packet);

    // TODO: Here first check if management or control frames are forwarded here.

    // replace 802.11be indicators with upper layer indicator
    // identified by LLC layer
    auto upperProtocol = packet->getTag<MacProtocolInd>()->getProtocol();
    packet->addTagIfAbsent<DispatchProtocolReq>()->setProtocol(upperProtocol);
    packet->addTagIfAbsent<PacketProtocolTag>()->setProtocol(upperProtocol);

    if(!isStation) {
        // IMPORTANT: AP keeps tracks of the received interface of each frame.
        packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(packet->getTag<InterfaceInd>()->getInterfaceId());
    } else {
        updateStatistics(packet);
        string packetName = packet->getName();
        if(packetName.find("UDPLinkQuality") != string::npos) {
            delete packet;
            return;
        }
    }
    send(packet, upperLayerOutGateId);
}

void UMac::updateStatistics(Packet *packet) {

    int receivedIftId = packet->getTag<InterfaceInd>()->getInterfaceId();

    string packetName = packet->getName();

    if(packetName.find("UDPData") != string::npos) {
        int packetNumber = stoi(packetName.substr(packetName.find("-") + 1, packetName.length()));
        double latency = (simTime() - packet->getCreationTime()).dbl();
        auto it = lot.find(receivedIftId);

        recordScalar((std::string("#received " + packetName + " on ift[") + std::to_string(receivedIftId) + std::string("] with latency: ")).c_str(), latency);

        if(it != lot.end()) {
            (it->second).receivedPackets.push_back(packetNumber);
            (it->second).latencyPackets.push_back(latency);
        } else {
            LinkOverview lo = LinkOverview();
            lo.receivedPackets.push_back(packetNumber);
            lo.latencyPackets.push_back(latency);
            lot[receivedIftId] = lo;
        }
    }
}

void UMac::updateLinkQosParameters(Packet *packet) {

    if (packet->findTag<InterfaceInd>() && packet->findTag<SnirInd>()) {
        auto ifaceIndTag = packet->getTag<InterfaceInd>();
        auto snirIndTag = packet->getTag<SnirInd>();

        int ifaceId = ifaceIndTag->getInterfaceId();
        double snir = snirIndTag->getAverageSnir();

        auto it = lot.find(ifaceId);

        if (it != lot.end()) {
            (it->second).qosParams = QosParameters{ifaceId, snir};
        } else {
            LinkOverview lo = LinkOverview();
            lo.qosParams = QosParameters{ifaceId, snir};
            lot[ifaceId] = lo;
        }
    }

}

void UMac::updateChannelAccessParameters() {
    for(int i = 0; i < ift->getNumInterfaces(); i++) {
        auto iface = ift->getInterface(i);
        int ifaceId = iface->getInterfaceId();
        Ieee80211LMac *lmac = check_and_cast<Ieee80211LMac*>(iface->getSubmodule("mac"));
        std::vector<ChannelAccessParameters> parameters = lmac->getChannelAccessParams();

        auto it = lot.find(ifaceId);

        if (it != lot.end()) {
            (it->second).channelParamsPerAc = parameters;
        } else {
            LinkOverview lo = LinkOverview();
            lo.channelParamsPerAc = parameters;
            lot[ifaceId] = lo;
        }
    }
}

void UMac::printLinkOverview() {
    for (auto it = lot.begin(); it != lot.end(); it++) {

        std::cout << "For interface " << it->first << endl
                << "QoS parameters: " << endl
                << (it->second).qosParams << endl
                << "Channel access parameters: " << endl;

        std::vector<ChannelAccessParameters> params = (it->second).channelParamsPerAc;

        for(int j = 0; j < params.size(); j++) {
            std::cout << params[j] << endl;
        }
    }
}

void UMac::handleSelfMessage(cMessage *message) {

    if(message->getKind() == 0) { // record statistics message
        recordStatistics();
        cMessage* statisticsTimer = new cMessage("StatisticsTimer", 0);
        scheduleAt(simTime() + 0.1, statisticsTimer);
    }

    delete message;
}

void UMac::handleUpperCommand(cMessage *message) {
    delete message;
}
void UMac::handleLowerCommand(cMessage *message) {
    delete message;
}

bool UMac::isUpperMessage(cMessage *message) const {
    return message->getArrivalGateId() == upperLayerInGateId;
}

bool UMac::isLowerMessage(cMessage *message) const {
    return message->getArrivalGateId() == lowerLayerInGateId;
}

bool UMac::isClassifierMessage(cMessage *message) const {
    return message->getArrivalGateId() == classifierInGateId;
}

void UMac::recordStatistics() {
    std::vector<int> totalReceivedPackets;

    for (auto it = lot.begin(); it != lot.end(); it++) {
        std::vector<int> receivedPackets = (it->second).receivedPackets;
        totalReceivedPackets.insert(totalReceivedPackets.end(), receivedPackets.begin(), receivedPackets.end());
        recordScalar((std::string("#received ift[") + std::to_string(it->first) + std::string("]")).c_str(), receivedPackets.size());
    }

    std::sort(totalReceivedPackets.begin(), totalReceivedPackets.end());
    auto uniqueReceivedPackets = std::unique(totalReceivedPackets.begin(), totalReceivedPackets.end()) - totalReceivedPackets.begin();

    emit(rcvdPacketsUnique, uniqueReceivedPackets);

    recordScalar("#total received", totalReceivedPackets.size());
    recordScalar("#total unique received", uniqueReceivedPackets);
}

void UMac::finish() {
    std::vector<int> totalReceivedPackets;

    std::cout << this->getParentModule()->getName() << "." << this->getName() << ":" << endl;

    for (auto it = lot.begin(); it != lot.end(); it++) {

        std::vector<int> receivedPackets = (it->second).receivedPackets;
        std::vector<double> latencyPackets = (it->second).latencyPackets;

        float averageLatency = 0.0;
        if(!latencyPackets.empty()) {
            auto const count = static_cast<float>(latencyPackets.size());

            for(auto l: latencyPackets) averageLatency += l;
            averageLatency = averageLatency / count;
        }

        totalReceivedPackets.insert(totalReceivedPackets.end(), receivedPackets.begin(), receivedPackets.end());

        std::cout << "For interface " << it->first << endl
                << "#received: " << receivedPackets.size() << endl
                << "average latency: " << averageLatency << endl;
    }

    std::sort(totalReceivedPackets.begin(), totalReceivedPackets.end());
    auto uniqueReceivedPackets = std::unique(totalReceivedPackets.begin(), totalReceivedPackets.end()) - totalReceivedPackets.begin();

    // This function is called by OMNeT++ at the end of the simulation.
    std::cout << "#total received: " << totalReceivedPackets.size() << endl;
    std::cout << "#total unique received: " << uniqueReceivedPackets << endl;
}


}

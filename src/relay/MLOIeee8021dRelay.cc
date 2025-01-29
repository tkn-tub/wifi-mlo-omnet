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

#include "MLOIeee8021dRelay.h"

#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/linklayer/common/EtherType_m.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/common/MacAddressTag_m.h"
#include "inet/linklayer/common/VlanTag_m.h"
#include "inet/linklayer/common/UserPriorityTag_m.h"
#include "inet/linklayer/configurator/Ieee8021dInterfaceData.h"

namespace inet {

Define_Module(MLOIeee8021dRelay);

void MLOIeee8021dRelay::handleLowerPacket(Packet *incomingPacket)
{
    numReceivedNetworkFrames++;
    auto protocol = incomingPacket->getTag<PacketProtocolTag>()->getProtocol();
    auto macAddressInd = incomingPacket->getTag<MacAddressInd>();
    auto sourceAddress = macAddressInd->getSrcAddress();
    auto destinationAddress = macAddressInd->getDestAddress();
    auto interfaceInd = incomingPacket->getTag<InterfaceInd>();
    int incomingInterfaceId = interfaceInd->getInterfaceId();
    auto incomingInterface = interfaceTable->getInterfaceById(incomingInterfaceId);
    unsigned int vlanId = 0;
    if (auto vlanInd = incomingPacket->findTag<VlanInd>())
        vlanId = vlanInd->getVlanId();
    EV_INFO << "Processing packet from network" << EV_FIELD(incomingInterface) << EV_FIELD(incomingPacket) << EV_ENDL;
    updatePeerAddress(incomingInterface, sourceAddress, vlanId);

    const auto& incomingInterfaceData = incomingInterface->findProtocolData<Ieee8021dInterfaceData>();
    // BPDU Handling
    if ((!incomingInterfaceData || incomingInterfaceData->getRole() != Ieee8021dInterfaceData::DISABLED)
            && (destinationAddress == bridgeAddress
                || in_range(registeredMacAddresses, destinationAddress)
                || incomingInterface->matchesMacAddress(destinationAddress))
            && !destinationAddress.isBroadcast())
    {
        EV_DETAIL << "Deliver to upper layer" << endl;
        sendUp(incomingPacket); // deliver to the STP/RSTP module
    }
    else if (incomingInterfaceData && !incomingInterfaceData->isForwarding()) {
        EV_INFO << "Dropping packet because the incoming interface is currently not forwarding" << EV_FIELD(incomingInterface) << EV_FIELD(incomingPacket) << endl;
        numDroppedFrames++;
        PacketDropDetails details;
        details.setReason(NO_INTERFACE_FOUND);
        emit(packetDroppedSignal, incomingPacket, &details);
        delete incomingPacket;
    }
    else {
        auto outgoingPacket = incomingPacket->dup();
        outgoingPacket->trim();
        outgoingPacket->clearTags();
        outgoingPacket->addTag<PacketProtocolTag>()->setProtocol(protocol);
        if (auto vlanInd = incomingPacket->findTag<VlanInd>())
            outgoingPacket->addTag<VlanReq>()->setVlanId(vlanInd->getVlanId());
        if (auto userPriorityInd = incomingPacket->findTag<UserPriorityInd>())
            outgoingPacket->addTag<UserPriorityReq>()->setUserPriority(userPriorityInd->getUserPriority());
        auto& macAddressReq = outgoingPacket->addTag<MacAddressReq>();
        macAddressReq->setSrcAddress(sourceAddress);
        macAddressReq->setDestAddress(destinationAddress);

        // IMPORTANT: Inbound interface information should be passed back to U-MAC.
        outgoingPacket->addTagIfAbsent<InterfaceInd>()->setInterfaceId(incomingInterfaceId);

        // TODO revise next "if"s: 2nd drops all packets for me if not forwarding port; 3rd sends up when dest==STP_MULTICAST_ADDRESS; etc.
        // reordering, merge 1st and 3rd, ...

        if (destinationAddress.isBroadcast())
            broadcastPacket(outgoingPacket, destinationAddress, incomingInterface);
        else if (destinationAddress.isMulticast()) {
            auto outgoingInterfaceIds = macForwardingTable->getMulticastAddressForwardingInterfaces(destinationAddress);
            if (outgoingInterfaceIds.size() == 0)
                broadcastPacket(outgoingPacket, destinationAddress, incomingInterface);
            else {
                for (auto outgoingInterfaceId : outgoingInterfaceIds) {
                    auto outgoingInterface = interfaceTable->getInterfaceById(outgoingInterfaceId);
                    if (outgoingInterfaceId != incomingInterfaceId) {
                        if (isForwardingInterface(outgoingInterface))
                            sendPacket(outgoingPacket->dup(), destinationAddress, outgoingInterface);
                        else {
                            EV_WARN << "Discarding packet because output interface is currently not forwarding" << EV_FIELD(outgoingInterface) << EV_FIELD(outgoingPacket) << endl;
                            numDroppedFrames++;
                            PacketDropDetails details;
                            details.setReason(NO_INTERFACE_FOUND);
                            emit(packetDroppedSignal, outgoingPacket, &details);
                        }
                    }
                    else {
                        EV_WARN << "Discarding packet because outgoing interface is the same as incoming interface" << EV_FIELD(destinationAddress) << EV_FIELD(incomingInterface) << EV_FIELD(incomingPacket) << EV_ENDL;
                        numDroppedFrames++;
                        PacketDropDetails details;
                        details.setReason(NO_INTERFACE_FOUND);
                        emit(packetDroppedSignal, outgoingPacket, &details);
                    }
                }
                delete outgoingPacket;
            }
        }
        else {
            // IMPORTANT: It is normal to send through the same interface in MLO - this relaying function is overwritten by UMAC.
            // In fact, the relay is only kept as a part of the AccessPoint for backward compatability to the existing AP implementation of OMNeT++.
            // The default behavior of MLO is changed to sending packets over the interface they they are received from.
            if (incomingInterfaceId) {
                sendPacket(outgoingPacket, destinationAddress, incomingInterface);
            } else {
                // IMPORTANT: This is the default behavior of the legacy Ieee8021dRelay
                auto outgoingInterfaceId = macForwardingTable->getUnicastAddressForwardingInterface(destinationAddress);

                if (outgoingInterfaceId == -1) {
                    broadcastPacket(outgoingPacket, destinationAddress, incomingInterface);
                }
                else {
                    auto outgoingInterface = interfaceTable->getInterfaceById(outgoingInterfaceId);
                    if (outgoingInterfaceId != incomingInterfaceId) {
                        if (isForwardingInterface(outgoingInterface)) {
                            sendPacket(outgoingPacket->dup(), destinationAddress, outgoingInterface);
                        }
                        else {
                            EV_WARN << "Discarding packet because output interface is currently not forwarding" << EV_FIELD(outgoingInterface) << EV_FIELD(outgoingPacket) << endl;
                            numDroppedFrames++;
                            PacketDropDetails details;
                            details.setReason(NO_INTERFACE_FOUND);
                            emit(packetDroppedSignal, outgoingPacket, &details);
                        }
                    }

                    else {
                        EV_WARN << "Discarding packet because outgoing interface is the same as incoming interface" << EV_FIELD(destinationAddress) << EV_FIELD(incomingInterface) << EV_FIELD(incomingPacket) << EV_ENDL;
                        numDroppedFrames++;
                        PacketDropDetails details;
                        details.setReason(NO_INTERFACE_FOUND);
                        emit(packetDroppedSignal, outgoingPacket, &details);
                    }
                    delete outgoingPacket;
                }
            }
        }
        delete incomingPacket;
    }
    updateDisplayString();
};

}

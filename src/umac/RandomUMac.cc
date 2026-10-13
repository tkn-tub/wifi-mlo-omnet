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

#include "RandomUMac.h"

#include <omnetpp.h>
#include <string>

#include "inet/linklayer/common/InterfaceTag_m.h"

namespace inet {

Register_Class(RandomUMac);

void RandomUMac::sendPacket(Packet *packet) {
    // send to a random interface
    int getRandomIfaceEnum = std::rand() % ift->getNumInterfaces();
    NetworkInterface *targetIft = ift->getInterface(getRandomIfaceEnum);

    int randomTrial = ift->getNumInterfaces() * 10;

    while(targetIft->getState() != NetworkInterface::State::UP && randomTrial >= 0) {
        targetIft = ift->getInterface(std::rand() % ift->getNumInterfaces());
        randomTrial--;
    }

    if(targetIft->getState() == NetworkInterface::State::UP) {
        packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(targetIft->getInterfaceId());
        std::string packetName = packet->getName();
        recordScalar((std::string("#sent " + packetName + " on ift[") + std::to_string(targetIft->getInterfaceId()) + std::string("] at: ")).c_str(), simTime());
        send(packet, lowerLayerOutGateId);
    } else {
        delete packet;
    }
}

void RandomUMac::handleUpperPacket(Packet *packet) {
    //std::cout << this->getParentModule()->getName() << "." << this->getName() << "." <<  __func__ << "() logs - " << "Sent  " << packet->getName() << endl;

    // Remove dispatch requirement otherwise the MessageDispatchers
    // bounces the packets back.
    packet->removeTagIfPresent<DispatchProtocolReq>();

    if(isStation) {
        updateChannelAccessParameters();
    }
    sendPacket(packet);
}

}

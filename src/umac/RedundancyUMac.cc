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

#include "RedundancyUMac.h"

#include <omnetpp.h>

#include "inet/linklayer/common/InterfaceTag_m.h"

namespace inet {

using namespace std;

Register_Class(RedundancyUMac);

void RedundancyUMac::sendPacket(Packet *packet) {

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

}

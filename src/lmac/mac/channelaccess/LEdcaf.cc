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

#include <string>
#include "LEdcaf.h"

#include "inet/linklayer/ieee80211/mac/channelaccess/Edcaf.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/Simsignals.h"
#include "inet/networklayer/common/NetworkInterface.h"



namespace inet {
namespace ieee80211 {

Define_Module(LEdcaf);

void LEdcaf::finish() {

    // TODO: Need to make sure no packets are received in the queues but when we try to use the whole link capacity
    // contention becomes the main factor for packet queueing. At least for the reliability of the final results,
    // we need to know how many packets are not transmitted but buffered in the queues.

    double queuedPackets = getQueueSize();
    if(queuedPackets > 0) {
        /*
         * Recording all queued packets can help to evaluate per-app performance.
        queueing::IPacketQueue *pq = getPendingQueue();
        while(pq->getNumPackets()) {
            Packet* pending_packet = pq->dequeuePacket();
            std::string packetName = pending_packet->getName();
            recordScalar((std::string("queued " + packetName + " at: ")).c_str(), simTime());
            delete pending_packet;
        }*/

        std::cout << "#queuedPackets: " << queuedPackets << std::endl;
        recordScalar("#queuedPackets: ", queuedPackets);
    }
}

void LEdcaf::initialize(int stage)
{
    Edcaf::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {
        contention = check_and_cast<LContention *>(getSubmodule("contention"));
    }
}

int LEdcaf::getBackoffCounter() {
    return contention->getBackoffCounter();
}

int LEdcaf::getQueueSize() {
    return pendingQueue->getNumPackets();
}

void LEdcaf::channelAccessGranted()
{
    Enter_Method("channelAccessGranted");
    ASSERT(callback != nullptr);
    if (!collisionController->isInternalCollision(this)) {
        owning = true;
        emit(channelOwnershipChangedSignal, owning);
        callback->channelGranted(this);
    }
    else
        EV_WARN << "Ignoring channel access granted due to internal collision.\n";
}


} // namespace ieee80211
} // namespace inet


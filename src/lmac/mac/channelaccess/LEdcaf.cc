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

#include "LEdcaf.h"

#include "inet/linklayer/ieee80211/mac/channelaccess/Edcaf.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/Simsignals.h"
#include "inet/networklayer/common/NetworkInterface.h"



namespace inet {
namespace ieee80211 {

Define_Module(LEdcaf);

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


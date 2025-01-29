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

#include "LContention.h"

#include "inet/linklayer/ieee80211/mac/contention/Contention.h"

#include "inet/common/FSMA.h"
#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"

namespace inet {
namespace ieee80211 {

Define_Module(LContention);

int LContention::getBackoffCounter() {
    //computeRemainingBackoffSlots();
    return backoffSlots;
}

void LContention::handleMessage(cMessage *msg)
{
    // IMPORTANT: This function is overwritten to circumvent a SEGFAULT in the internal collision handler
    if (msg == startTxEvent) {
        emit(backoffStoppedSignal, SimTime::ZERO);
        handleWithFSM(CHANNEL_ACCESS_GRANTED);
    }
    else if (msg == channelGrantedEvent) {
        EV_INFO << "Channel granted: startTime = " << startTime << std::endl;
        emit(channelAccessGrantedSignal, this);
        callback->channelAccessGranted();
        callback = nullptr;
    }
    else
        throw cRuntimeError("Unknown msg");
}

}
}

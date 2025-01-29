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

#include "Ieee80211LMgmtSta.h"

#include "inet/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h"

namespace inet {

namespace ieee80211 {

Define_Module(Ieee80211LMgmtSta);

void Ieee80211LMgmtSta::handleMessageWhenDown(cMessage *message)
{
    // IMPORTANT: To handle failure scenarios, the management module should ignore the messages when it is down.
    // do not delete the message because it might be sent to multiple modules.
    EV_WARN << "Self message " << message->getName() << " received when " << getComponentType()->getName() << " is down" << endl;
}

void Ieee80211LMgmtSta::stop()
{
    if(assocAP.beaconTimeoutMsg != nullptr)
        cancelEvent(assocAP.beaconTimeoutMsg);
    if(assocTimeoutMsg != nullptr)
        cancelEvent(assocTimeoutMsg);

    for (auto& ap : apList) {
        if(ap.authTimeoutMsg != nullptr)
            cancelEvent(ap.authTimeoutMsg);
    }
    Ieee80211MgmtBase::stop();
}

}
}

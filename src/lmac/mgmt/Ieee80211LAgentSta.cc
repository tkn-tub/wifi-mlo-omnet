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

#include "Ieee80211LAgentSta.h"

#include "inet/common/INETUtils.h"
#include "inet/common/ModuleAccess.h"

namespace inet {

namespace ieee80211 {

Define_Module(Ieee80211LAgentSta);

void Ieee80211LAgentSta::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj, cObject *details)
{
    Enter_Method("%s", cComponent::getSignalName(signalID));

    if (signalID == l2BeaconLostSignal) {

        NetworkInterface *targetInf = check_and_cast<NetworkInterface*>(obj);

        // FIXME Management module emits signals for beacon timeout and it triggers dissociation at ALL network interfaces that captures the signal.
        // in legacy WiFi, this is harmless. for MLO, we should check first which interface the signal is sent for.

        if(strcmp(targetInf->getInterfaceName(), myIface->getInterfaceName()) == 0) {
            EV << "beacon lost, starting scanning again\n";
            getContainingNode(this)->bubble("Beacon lost!");
            // sendDisassociateRequest();
            sendScanRequest();
            emit(l2DisassociatedSignal, myIface);
        }
    }
}

}}

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

#include "LDs.h"

#include "inet/common/ModuleAccess.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"
#include "inet/linklayer/ieee80211/mgmt/Ieee80211MgmtApSimplified.h"

namespace inet {
namespace ieee80211 {

Define_Module(LDs);

void LDs::processDataFrame(Packet *frame, const Ptr<const Ieee80211DataHeader>& header)
{
    Enter_Method("processDataFrame");
    take(frame);
    if (mib->mode == Ieee80211Mib::INDEPENDENT)
        mac->sendUp(frame);
    else if (mib->mode == Ieee80211Mib::INFRASTRUCTURE) {
        if (mib->bssStationData.stationType == Ieee80211Mib::ACCESS_POINT) {
            // check toDS bit
            if (!header->getToDS()) {
                // looks like this is not for us - discard
                EV_WARN << "Frame is not for us (toDS=false) -- discarding\n" << endl;
                PacketDropDetails details;
                details.setReason(NOT_ADDRESSED_TO_US);
                emit(packetDroppedSignal, frame, &details);
                delete frame;
                return;
            }
            // handle broadcast/multicast frames
            if (header->getAddress3().isMulticast()) {
                EV_INFO << "Handling multicast frame\n" << endl;
                //distributeDataFrame(frame, header);
                mac->sendUp(frame);
                return;
            }
            // look up destination address in our STA list
            auto it = mib->bssAccessPointData.stations.find(header->getAddress3());

            if (it == mib->bssAccessPointData.stations.end()) {
                EV_WARN << "Frame's destination address is not in our STA list -- passing up\n"<< endl;
                mac->sendUp(frame);
            }
            else {
                // dest address is our STA, but is it already associated?
                if (it->second == Ieee80211Mib::ASSOCIATED) {
                    // NOTE: Never distribute frames without sending to UMAC.
                    if (dynamicPtrCast<const Ieee80211DataHeader>(header)) {
                        mac->sendUp(frame);
                    }
                    else {
                        distributeDataFrame(frame, header); // send it out to the destination STA
                        delete frame;
                    }
                }
                else {
                    EV_WARN << "Frame's destination STA is not in associated state -- dropping frame\n"<< endl;
                    PacketDropDetails details;
                    details.setReason(OTHER_PACKET_DROP);
                    emit(packetDroppedSignal, frame, &details);
                    delete frame;
                }
            }
        }
        else if (mib->bssStationData.stationType == Ieee80211Mib::STATION) {
            if (!mib->bssStationData.isAssociated) {
                EV_WARN << "Rejecting data frame as STA is not associated with an AP" << endl;
                PacketDropDetails details;
                details.setReason(OTHER_PACKET_DROP);
                emit(packetDroppedSignal, frame, &details);
                delete frame;
            }
            else if (mib->bssData.bssid != header->getTransmitterAddress()) {
                EV_WARN << "Rejecting data frame received from another AP" << endl;
                PacketDropDetails details;
                details.setReason(OTHER_PACKET_DROP);
                emit(packetDroppedSignal, frame, &details);
                delete frame;
            }
            else
                mac->sendUp(frame);
        }
        else
            throw cRuntimeError("Unknown station type");
    }
    else
        throw cRuntimeError("Unknown mode");
}

} }

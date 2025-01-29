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

#include "inet/linklayer/ieee80211/mac/originator/QosAckHandler.h"
#include "QosNoAckHandler.h"

namespace inet {
namespace ieee80211 {

Define_Module(QosNoAckHandler);

void QosNoAckHandler::processFailedFrame(const Ptr<const Ieee80211DataOrMgmtHeader>& dataOrMgmtHeader)
{
    if (dataOrMgmtHeader->getType() == ST_DATA_WITH_QOS) {
        auto dataHeader = dynamicPtrCast<const Ieee80211DataHeader>(dataOrMgmtHeader);
        auto id = std::make_pair(dataHeader->getReceiverAddress(), std::make_pair(dataHeader->getTid(), SequenceControlField(dataHeader->getSequenceNumber().get(), dataHeader->getFragmentNumber())));
        auto status = getQoSDataAckStatus(id);
        if (status == Status::WAITING_FOR_NORMAL_ACK)
            ackStatuses[id] = Status::NORMAL_ACK_NOT_ARRIVED;
        else if (status == Status::WAITING_FOR_BLOCK_ACK)
            ackStatuses[id] = Status::BLOCK_ACK_NOT_ARRIVED;
        else if (status == Status::NO_ACK_REQUIRED) // IMPORTANT: Added for disabled ACK
            ackStatuses[id] = Status::NO_ACK_REQUIRED;
        else
            throw cRuntimeError("Invalid ACK status");
    }
    else {
        ASSERT(getMgmtOrNonQoSAckStatus(dataOrMgmtHeader) == Status::WAITING_FOR_NORMAL_ACK);
        auto id = std::make_pair(dataOrMgmtHeader->getReceiverAddress(), SequenceControlField(dataOrMgmtHeader->getSequenceNumber().get(), dataOrMgmtHeader->getFragmentNumber()));
        mgmtAckStatuses[id] = Status::NORMAL_ACK_NOT_ARRIVED;
    }
}

}}

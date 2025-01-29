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

#include "Ieee80211LLlcLpd.h"

#include "inet/common/ProtocolGroup.h"
#include "inet/linklayer/ieee8022/Ieee8022LlcHeader_m.h"
#include "inet/linklayer/ieee8022/Ieee8022SnapHeader_m.h"

#include "inet/linklayer/common/Ieee802SapTag_m.h"
#include "inet/linklayer/ieee8022/Ieee8022LlcSocketCommand_m.h"

#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/Simsignals.h"
#include "inet/common/lifecycle/NodeStatus.h"
#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/stlutils.h"
#include "inet/linklayer/common/Ieee802SapTag_m.h"
#include "inet/linklayer/ieee8022/Ieee8022LlcSocketCommand_m.h"

//#include "src/common/MLOInetExtensions.h"

namespace inet {
namespace ieee80211 {

Define_Module(Ieee80211LLlcLpd);

void Ieee80211LLlcLpd::decapsulate(Packet *frame)
{
    const auto& llcHeader = frame->popAtFront<Ieee8022LlcHeader>();

    auto sapInd = frame->addTagIfAbsent<Ieee802SapInd>();
    sapInd->setSsap(llcHeader->getSsap());
    sapInd->setDsap(llcHeader->getDsap());

    if (llcHeader->getSsap() == 0xAA && llcHeader->getDsap() == 0xAA && llcHeader->getControl() == 0x03) {
        const auto& snapHeader = dynamicPtrCast<const Ieee8022LlcSnapHeader>(llcHeader);

        if (snapHeader == nullptr)
            throw cRuntimeError("LLC header indicates SNAP header, but SNAP header is missing");
    }
    auto payloadProtocol = Ieee8022Llc::getProtocol(llcHeader);
    if (payloadProtocol) {
        // This line injects a protocol identifier to be used by UMAC.
        // The type of tag, "MacProtocolInd", is selected randomly and
        // indicates the identified upper (e.g., network) layer protocol.
        frame->addTagIfAbsent<MacProtocolInd>()->setProtocol(payloadProtocol);
    }
    else {
        frame->removeTagIfPresent<DispatchProtocolReq>();
        frame->removeTagIfPresent<PacketProtocolTag>();
    }
}

const Protocol *Ieee80211LLlcLpd::getProtocol() const
{
    return &Protocol::ieee8022llc;
}

} // namespace ieee80211
} // namespace inet



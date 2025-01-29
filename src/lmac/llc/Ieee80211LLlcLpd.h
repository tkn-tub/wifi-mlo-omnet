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

#ifndef LMAC_LLC_IEEE80211LLLCLPD_H_
#define LMAC_LLC_IEEE80211LLLCLPD_H_

#include "inet/linklayer/ieee8022/Ieee8022Llc.h"
#include "inet/linklayer/ieee80211/llc/Ieee80211LlcLpd.h"

#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/Protocol.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/lifecycle/OperationalBase.h"
#include "inet/common/packet/Message.h"
#include "inet/common/packet/Packet.h"
#include "inet/linklayer/ieee8022/Ieee8022LlcHeader_m.h"
#include "inet/linklayer/ieee8022/Ieee8022SnapHeader_m.h"

namespace inet {
namespace ieee80211 {

class Ieee80211LLlcLpd : public Ieee8022Llc, public IIeee80211Llc
{
  public:
    const Protocol *getProtocol() const override;

  protected:
    virtual void decapsulate(Packet *frame) override;
};

} // namespace ieee80211
} // namespace inet

#endif

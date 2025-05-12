//
// Copyright (C) 2016 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __IEEE80211LMAC_H
#define __IEEE80211LMAC_H

#include "inet/linklayer/ieee80211/mac/Ieee80211Mac.h"
#include "inet/common/ModuleRefByPar.h"
#include "inet/linklayer/ieee80211/mac/contract/IDs.h"
#include "inet/linklayer/ieee80211/mac/contract/IRateControl.h"
#include "inet/linklayer/ieee80211/mac/contract/IRateSelection.h"
#include "inet/linklayer/ieee80211/mac/contract/IRx.h"
#include "inet/linklayer/ieee80211/mac/contract/ITx.h"
#include "inet/linklayer/ieee80211/mac/coordinationfunction/Dcf.h"
#include "inet/linklayer/ieee80211/mac/coordinationfunction/Hcf.h"
#include "inet/linklayer/ieee80211/mac/coordinationfunction/Mcf.h"
#include "inet/linklayer/ieee80211/mac/coordinationfunction/Pcf.h"
#include "inet/linklayer/ieee80211/mib/Ieee80211Mib.h"
#include "inet/physicallayer/wireless/common/contract/packetlevel/IRadio.h"

#include "src/lmac/mac/coordinationfunction/LHcf.h"
#include "Ieee80211beProtocol.h"

using namespace inet;
using namespace ieee80211;

namespace inet {
namespace ieee80211 {

class IContention;
class IRx;
class IIeee80211Llc;
class Ieee80211MacHeader;

/**
 * Implements the IEEE 802.11 MAC. The features, standards compliance and
 * exact operation of the MAC depend on the plugged-in components (see IUpperMac,
 * IRx, ITx, IContention and other interface classes).
 */
class Ieee80211LMac : public Ieee80211Mac
{

protected:

   LHcf *hcf = nullptr;

   virtual void initialize(int) override;

   /** @brief Handle messages from lower (physical) layer */
   virtual void handleLowerPacket(Packet *packet) override;

   virtual void encapsulate(Packet *packet) override;
   virtual void decapsulate(Packet *packet) override;

 public:
   Ieee80211LMac();
   virtual ~Ieee80211LMac();


   virtual void sendUp(cMessage *message) override;
   virtual void sendUpFrame(Packet *frame) override;

   virtual void processUpperFrame(Packet *packet, const Ptr<const Ieee80211DataOrMgmtHeader>& header) override;
   virtual void processLowerFrame(Packet *packet, const Ptr<const Ieee80211MacHeader>& header) override;

   std::vector<ChannelAccessParameters> getChannelAccessParams();
};

} // namespace ieee80211
} // namespace inet

#endif

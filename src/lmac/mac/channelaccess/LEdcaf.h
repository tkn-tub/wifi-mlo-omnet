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

#ifndef LMAC_MAC_CHANNELACCESS_LEDCAF_H_
#define LMAC_MAC_CHANNELACCESS_LEDCAF_H_

#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"
#include "inet/linklayer/ieee80211/mac/common/AccessCategory.h"
#include "inet/linklayer/ieee80211/mac/common/ModeSetListener.h"
#include "inet/linklayer/ieee80211/mac/channelaccess/Edcaf.h"

#include "../contention/LContention.h"

namespace inet {
namespace ieee80211 {

class LEdcaf : public Edcaf
{
  public:
    int getBackoffCounter();
    int getQueueSize();
    // IMPORTANT: This function is overwritten to circumvent a SEGFAULT in the internal collision handler
    virtual InProgressFrames *getInProgressFrames() const override { return inProgressFrames; }

  protected:
    LContention *contention = nullptr;

    void initialize(int stage) override;
    virtual void channelAccessGranted() override;

};

} /* namespace ieee80211 */
} /* namespace inet */

#endif

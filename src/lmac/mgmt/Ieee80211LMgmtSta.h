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

#ifndef LMAC_MGMT_IEEE80211LMGMTSTA_H_
#define LMAC_MGMT_IEEE80211LMGMTSTA_H_

#include "inet/linklayer/ieee80211/mgmt/Ieee80211MgmtSta.h"

namespace inet {

namespace ieee80211 {

class Ieee80211LMgmtSta: public Ieee80211MgmtSta {
    public:
        virtual void stop() override;
        virtual void handleMessageWhenDown(cMessage *message) override;
};

}

}

#endif /* LMAC_MGMT_IEEE80211LMGMTSTA_H_ */

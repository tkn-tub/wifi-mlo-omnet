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

#ifndef LMAC_PORTAL_IEEE80211LPORTAL_H_
#define LMAC_PORTAL_IEEE80211LPORTAL_H_

#include "inet/common/packet/Packet.h"
#include "inet/linklayer/ieee80211/portal/Ieee80211Portal.h"

namespace inet {

namespace ieee80211 {

class Ieee80211LPortal: public Ieee80211Portal {

    protected:
        virtual void decapsulate(Packet *packet);
};

#endif /* LMAC_PORTAL_IEEE80211LPORTAL_H_ */


}
}

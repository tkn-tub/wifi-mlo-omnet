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

#ifndef LMAC_MAC_CHANNELACCESS_LEDCA_H_
#define LMAC_MAC_CHANNELACCESS_LEDCA_H_

#include "inet/linklayer/ieee80211/mac/channelaccess/Edca.h"
#include "LEdcaf.h"

#include "inet/linklayer/ieee80211/mac/common/Ieee80211Defs.h"
#include "inet/linklayer/ieee80211/mac/lifetime/EdcaTransmitLifetimeHandler.h"
#include "inet/linklayer/ieee80211/mac/originator/NonQosRecoveryProcedure.h"

namespace inet {
namespace ieee80211 {

struct ChannelAccessParameters {
    ChannelAccessParameters(AccessCategory a, int b) : ac{a}, backoffCounter{b} {};
    AccessCategory ac;
    int backoffCounter;
    int ifaceId = 0;
};

inline std::ostream& operator<<(std::ostream& os, const ChannelAccessParameters& params) {
    return os << "Interface Id: " << params.ifaceId << endl
            << "Access Category: " << params.ac << endl
            << "Backoff Counter: " << params.backoffCounter;
}

class LEdca : public Edca
{
    public:
        std::vector<ChannelAccessParameters> getChannelAccessParams();
        LEdcaf *getEdcaf(AccessCategory ac) const override { return edcafs[ac]; }
        LEdcaf *getChannelOwner() override;
        virtual void requestChannelAccess(AccessCategory ac, IChannelAccess::ICallback *callback) override;
        virtual void releaseChannelAccess(AccessCategory ac, IChannelAccess::ICallback *callback) override;
        std::vector<LEdcaf *> getInternallyCollidedEdcafsModified();

    protected:
        LEdcaf **edcafs = nullptr;
        void initialize(int stage) override;

};

} /* namespace ieee80211 */
} /* namespace inet */

#endif


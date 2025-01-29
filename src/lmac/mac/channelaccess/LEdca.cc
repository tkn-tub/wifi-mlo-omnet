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

#include "LEdca.h"

namespace inet {
namespace ieee80211 {

Define_Module(LEdca);

void LEdca::initialize(int stage)
{
    if (stage == INITSTAGE_LINK_LAYER) {
        numEdcafs = par("numEdcafs");
        edcafs = new LEdcaf *[numEdcafs];
        for (int ac = 0; ac < numEdcafs; ac++) {
            edcafs[ac] = check_and_cast<LEdcaf *>(getSubmodule("edcaf", ac));
        }
        mgmtAndNonQoSRecoveryProcedure = check_and_cast<NonQosRecoveryProcedure *>(getSubmodule("mgmtAndNonQoSRecoveryProcedure"));
    }
}

std::vector<ChannelAccessParameters> LEdca::getChannelAccessParams() {
    // Returns immediate backoff counter for each AC on the respective interface
    std::vector<ChannelAccessParameters> parameters;

    for (int ac = 0; ac < numEdcafs; ac++) {
        ChannelAccessParameters p = ChannelAccessParameters{edcafs[ac]->getAccessCategory(), edcafs[ac]->getBackoffCounter()};
        parameters.push_back(p);
    }
    return parameters;
}

void LEdca::requestChannelAccess(AccessCategory ac, IChannelAccess::ICallback *callback)
{
    // IMPORTANT: Direct inheritance of this function causes SEGFAULT due to callback functions - thus it is copied.
    edcafs[ac]->requestChannel(callback);
}

void LEdca::releaseChannelAccess(AccessCategory ac, IChannelAccess::ICallback *callback)
{
    // IMPORTANT: Direct inheritance of this function causes SEGFAULT due to callback functions - thus it is copied.
    edcafs[ac]->releaseChannel(callback);
}

std::vector<LEdcaf *> LEdca::getInternallyCollidedEdcafsModified()
{
    // IMPORTANT: vectors of inherited objects are problematic to cast in virtual functions. Thus this function is modified here and used accordingly within HCF.
    std::vector<LEdcaf *> collidedEdcafs;
    for (int ac = 0; ac < numEdcafs; ac++)
        if (edcafs[ac]->isInternalCollision())
            collidedEdcafs.push_back(edcafs[ac]);
    return collidedEdcafs;
}


LEdcaf *LEdca::getChannelOwner()
{
    for (int ac = 0; ac < numEdcafs; ac++)
        if (edcafs[ac]->isOwning())
            return edcafs[ac];
    return nullptr;
}

} // namespace ieee80211
} // namespace inet


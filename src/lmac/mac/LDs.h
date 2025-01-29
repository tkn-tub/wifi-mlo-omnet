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

#ifndef LMAC_MAC_LDS_H_
#define LMAC_MAC_LDS_H_

#include "inet/common/packet/Packet.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Mac.h"
#include "inet/linklayer/ieee80211/mac/Ds.h"

namespace inet {
namespace ieee80211 {

class LDs : public Ds
{

  public:
    virtual void processDataFrame(Packet *frame, const Ptr<const Ieee80211DataHeader>& header) override;
};

} // namespace ieee80211
} // namespace inet

#endif /* LMAC_MAC_LDS_H_ */

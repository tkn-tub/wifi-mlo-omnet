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

#ifndef LMAC_RELAY_MLOIEEE8021DRELAY_H_
#define LMAC_RELAY_MLOIEEE8021DRELAY_H_

#include "inet/common/stlutils.h"
#include "inet/linklayer/ieee8021d/relay/Ieee8021dRelay.h"

namespace inet {

class MLOIeee8021dRelay : public Ieee8021dRelay
{

  protected:
    virtual void handleLowerPacket(Packet *packet) override;
};

} // namespace inet

#endif /* LMAC_RELAY_MLOIEEE8021DRELAY_H_ */

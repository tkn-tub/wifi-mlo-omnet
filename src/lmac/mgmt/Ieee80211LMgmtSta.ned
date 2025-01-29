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

package mlo.lmac.mgmt;

import inet.linklayer.ieee80211.mgmt.Ieee80211MgmtSta;
import inet.linklayer.ieee80211.mgmt.IIeee80211Mgmt;

module Ieee80211LMgmtSta extends Ieee80211MgmtSta like IIeee80211Mgmt
{
    parameters:
        @class(inet::ieee80211::Ieee80211LMgmtSta);

}

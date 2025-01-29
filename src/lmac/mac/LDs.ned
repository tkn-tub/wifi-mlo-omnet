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

package mlo.lmac.mac;


import inet.linklayer.ieee80211.mac.contract.IDs;
import inet.linklayer.ieee80211.mac.Ds;

//
// Implements the DS (Distribution Service) for IEEE 802.11, which is
// responsible for distributing correctly received frames to the higher
// layer, to the wireless LAN, etc.
//
// This is the default implementation of ~IDs.
//
simple LDs extends Ds like IDs
{
    parameters:
        @class(inet::ieee80211::LDs);
}


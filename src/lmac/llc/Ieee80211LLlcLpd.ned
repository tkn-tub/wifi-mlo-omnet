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

package mlo.lmac.llc;

import inet.linklayer.ieee80211.llc.IIeee80211Llc;
import inet.linklayer.ieee8022.Ieee8022Llc;

//
// An LLC implementation that encapsulates packets in an LLC header,
// using IEEE 802 LPD-style encoding.
//
// See ~Ieee8022LlcHeader, Ieee8022LlcSnapHeader.
//
simple Ieee80211LLlcLpd extends Ieee8022Llc like IIeee80211Llc
{
    @class(inet::ieee80211::Ieee80211LLlcLpd);
}
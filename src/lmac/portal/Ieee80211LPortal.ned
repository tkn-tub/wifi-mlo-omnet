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

package mlo.lmac.portal;

import inet.linklayer.ieee80211.portal.Ieee80211Portal;
import inet.linklayer.ieee80211.llc.IIeee80211Llc;
import inet.linklayer.ieee8022.IIeee8022Llc;

simple Ieee80211LPortal extends Ieee80211Portal like IIeee8022Llc, IIeee80211Llc
{
    parameters:
		@class(inet::ieee80211::Ieee80211LPortal);
}
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

package mlo.umac;

import inet.linklayer.contract.ILinkLayer;

//
// This module interface provides an abstraction for the upper interface of
// different link layers.
//
moduleinterface IUMac extends ILinkLayer
{
    parameters:
        @display("i=block/rxtx");

    gates:
        input classifierIn @loose;
        output classifierOut @loose;
        
        // these mgmt gates are redundant, should be revised after LMAC is stripped out from mgmt connections.
        input lowerLayerMgmtIn @loose;
        output lowerLayerMgmtOut @loose;
        input upperLayerMgmtIn @loose;
        output upperLayerMgmtOut @loose;
        
        // these llc gates are redundant, should be revised after LMAC is stripped out from mgmt connections.
        input lowerLayerLlcIn @loose;
        output lowerLayerLlcOut @loose;
        input upperLayerLlcIn @loose;
        output upperLayerLlcOut @loose;
}

//
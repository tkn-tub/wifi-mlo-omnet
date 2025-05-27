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

module UMac like IUMac
{
    parameters:
        @class(inet::UMac);
        string interfaceTableModule;   // The path to the InterfaceTable module
        bool isStation = default(true);
        string defaultInterface = default("lmac0");
        
        // IMPORTANT: Mode is not an active parameter for now. Just left here for future reference. 
        string mode @enum("UMac", "RandomUMac", "RedundancyUMac") = default("UMac"); 
        
        @signal[rcvdPacketsUnique](type=int);
        
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

        input upperLayerIn @labels(INetworkHeader/down);
        output upperLayerOut @labels(INetworkHeader/up);
        
        input lowerLayerIn @labels(ILinkFrame/up);
        output lowerLayerOut @labels(ILinkFrame/down);
}

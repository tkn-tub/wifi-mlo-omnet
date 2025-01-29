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

package mlo.lmac;

import inet.networklayer.common.NetworkInterface;
import inet.linklayer.contract.IWirelessInterface;
import inet.linklayer.ieee80211.mib.Ieee80211Mib;
import inet.linklayer.ieee80211.mgmt.IIeee80211Mgmt;
import inet.linklayer.ieee80211.IIeee80211Mac;
import inet.physicallayer.wireless.common.contract.packetlevel.IRadio;

import inet.linklayer.ieee80211.mgmt.IIeee80211Agent;
import inet.linklayer.common.IIeee8021dQosClassifier;
import inet.linklayer.ieee80211.mgmt.Ieee80211MgmtSta;
import inet.linklayer.ieee80211.mgmt.Ieee80211AgentSta;
import inet.networklayer.common.InterfaceTable;
import inet.linklayer.contract.IWirelessInterface;
import inet.linklayer.ieee80211.llc.IIeee80211Llc;

module Ieee80211LInterface extends NetworkInterface like IWirelessInterface
{
    parameters:
		string interfaceTableModule;
		
        string opMode @enum("a","b","g(erp)","g(mixed)","n(mixed-2.4Ghz)","p","ac") = default("ac");
        string address @mutable = default("auto"); // MAC address as hex string (12 hex digits), or
                                                   // "auto". "auto" values will be replaced by
                                                   // a generated MAC address in init stage 0.
        string protocol = default("ieee80211be"); // ieee80211be
        double bitrate @unit(bps) = default(-1bps);
        
        **.opMode = this.opMode;
        **.bitrate = this.bitrate;
        mac.modeSet = default(this.opMode);
        mac.*.rateSelection.dataFrameBitrate = default(this.bitrate);
        *.macModule = default(absPath(".mac"));
        *.mibModule = default(absPath(".mib"));
		*.interfaceTableModule = default(absPath(this.interfaceTableModule));
		
    gates:
        input upperLayerIn;
        output upperLayerOut;
        
        input radioIn @labels(IWirelessSignal);
    submodules:
        mib: Ieee80211Mib {
            @display("p=125,742");
        }
        llc: <default("Ieee80211LLlcLpd")> like IIeee80211Llc {
            parameters:
                @display("p=300,200");
        }
        agent: <default("Ieee80211AgentSta")> like IIeee80211Agent if typename != "" {
            @display("p=481,300");
        }
        mgmt: <default("Ieee80211LMgmtSta")> like IIeee80211Mgmt  {
            @display("p=389,300");
        }
        mac: <default("Ieee80211LMac")> like IIeee80211Mac {
            parameters:
                @display("p=300,300");
        }
        radio: <default("Ieee80211ScalarRadio")> like IRadio {
            parameters:
                @display("p=300,400");
        }
    connections:
        radioIn --> { @display("m=s"); } --> radio.radioIn;
        radio.upperLayerIn <-- mac.lowerLayerOut;
        radio.upperLayerOut --> mac.lowerLayerIn;

        mgmt.agentOut --> agent.mgmtIn if exists(agent);
        mgmt.agentIn <-- agent.mgmtOut if exists(agent);

        mac.mgmtIn <-- mgmt.macOut;
        mac.mgmtOut --> mgmt.macIn;
 
        llc.upperLayerOut --> { @display("m=n"); } --> upperLayerOut;
        llc.upperLayerIn <-- { @display("m=n"); } <-- upperLayerIn;
        
        llc.lowerLayerOut --> mac.upperLayerIn;
        llc.lowerLayerIn <-- mac.upperLayerOut;

}



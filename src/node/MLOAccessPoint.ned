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

package mlo.node;

import inet.common.MessageDispatcher;
import inet.common.lifecycle.NodeStatus;
import inet.linklayer.contract.IEthernetInterface;
import inet.linklayer.contract.IMacForwardingTable;
import inet.linklayer.contract.IMacRelayUnit;
import inet.linklayer.contract.IWirelessInterface;
import inet.linklayer.ethernet.contract.IEthernetLayer;
import inet.mobility.contract.IMobility;
import inet.networklayer.common.InterfaceTable;

import mlo.relay.MLOIeee8021dRelay;
import mlo.lmac.Ieee80211LInterface;
import mlo.umac.IUMac;
//
// A generic access point supporting multiple wireless radios, and
// multiple ethernet ports. The type of the ethernet MAC, relay unit
// and wireless card can be specified as parameters.
//
// By default, the access point is stationary (~StationaryMobility),
// but that can also be configured by a parameter.
//
// The wlan[*].mgmt.typename can be configured for different management types
// currently it can be: ~Ieee80211MgmtApSimplified and ~Ieee80211MgmtAp.
// By default it is ~Ieee80211MgmtAp. The Simplified version does not
// support channel scanning, authentication and association .
// In this case, nodes must explicitly specify the hardware address
// of the wlan card they want to be associated with.
//
module MLOAccessPoint
{
    parameters:
        @networkNode();
        @labels(node,ethernet-node,wireless-node);
        @display("i=device/accesspoint");
        int numWlanInterfaces = default(2);               // the number of radios in the access point
        bool hasStatus = default(false);
        string fcsMode @enum("declared","computed") = default("declared");
        lmac[*].protocol = default("ieee8021be");
        lmac[*].mgmt.typename = default("Ieee80211MgmtAp");
        lmac[*].llc.typename = default("Ieee80211LPortal");
        lmac[*].agent.typename = default("");
        lmac[*].radio.antenna.mobilityModule = default("^.^.^.mobility");
        eth[*].encap.typename = "";
        *.macTableModule = default(absPath(".macTable"));
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        *.fcsMode = this.fcsMode;
        ethernet.registerProtocol = default(true);
        
        umac.isStation = false;
        
    gates:
        input radioIn[numWlanInterfaces] @directIn;
        inout ethg[] @labels(EtherFrame-conn);
    submodules:
        status: NodeStatus if hasStatus {
            @display("p=100,100;is=s");
        }
        interfaceTable: InterfaceTable {
            @display("p=100,200;is=s");
        }
        macTable: <default("MacForwardingTable")> like IMacForwardingTable {
            @display("p=100,300;is=s");
        }
        mobility: <default("StationaryMobility")> like IMobility {
            @display("p=100,400;is=s");
        }
        bridging: <default("MLOIeee8021dRelay")> like IMacRelayUnit if sizeof(ethg)+numWlanInterfaces > 1 && typename != "" {
            @display("p=800,100");
        }
        bl: MessageDispatcher if sizeof(ethg)+numWlanInterfaces > 1 && exists(bridging) {
            @display("p=800,200;b=1200,5,,,,1");
        }
        ethernet: <default("EthernetEncapsulation")> like IEthernetLayer if typename != "" && exists(bridging) {
            @display("p=700,282");
        }
        nl: MessageDispatcher {
            @display("p=800,348;b=1200,5,,,,1");
        }
        umac: <default("UMac")> like IUMac {
            @display("p=700,408,row,150;q=queue");
        }
        li: MessageDispatcher {
            @display("p=800,463;b=1000,5,,,,1");
        }
        lmac[numWlanInterfaces]: <default("Ieee80211LInterface")> like IWirelessInterface {
            @display("p=689,542,row,150;q=queue");
        }
        eth[sizeof(ethg)]: <default("EthernetInterface")> like IEthernetInterface {
            mac.promiscuous = true;
            @display("p=889,542,row;q=txQueue");
        }
    connections allowunconnected:
        bl.out++ --> bridging.lowerLayerIn if sizeof(ethg)+numWlanInterfaces > 1 && exists(bridging);
        bl.in++ <-- bridging.lowerLayerOut if sizeof(ethg)+numWlanInterfaces > 1 && exists(bridging);

        bl.out++ --> nl.in++ if exists(bl);
        nl.out++ --> bl.in++ if exists(bl);

        bl.out++ --> ethernet.upperLayerIn if exists(ethernet);
        ethernet.upperLayerOut --> bl.in++ if exists(ethernet);
        ethernet.lowerLayerOut --> nl.in++ if exists(ethernet);
        nl.out++ --> ethernet.lowerLayerIn if exists(ethernet);

        umac.upperLayerOut --> nl.in++;
        umac.upperLayerIn <-- nl.out++;

        umac.lowerLayerOut --> li.in++;
        umac.lowerLayerIn <-- li.out++;

		nl.out++ --> li.in++;
		li.out++ --> nl.in++;

        // connections to network outside
        // wireless interfaces MUST be connected first (i.e. ports 0..numWlanInterfaces-1)
        // because broadcasts must be handled differently for wireless IFs by the bridging
        for i=0..numWlanInterfaces-1 {
            radioIn[i] --> { @display("m=s"); } --> lmac[i].radioIn;
            lmac[i].upperLayerOut --> li.in++ if sizeof(ethg)+numWlanInterfaces > 1 && exists(bridging);
            lmac[i].upperLayerIn <-- li.out++ if sizeof(ethg)+numWlanInterfaces > 1 && exists(bridging);
        }
        // ethernet must be connected only AFTER wireless ports
        for i=0..sizeof(ethg)-1 {
            eth[i].phys <--> { @display("m=s"); } <--> ethg[i];
            eth[i].upperLayerIn <-- li.out++ if sizeof(ethg)+numWlanInterfaces>1;
            eth[i].upperLayerOut --> li.in++ if sizeof(ethg)+numWlanInterfaces>1;
        }
}


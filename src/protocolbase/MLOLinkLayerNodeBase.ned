//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


package mlo.protocolbase;

import mlo.lmac.Ieee80211LInterface;
import mlo.umac.IUMac;

import inet.common.packet.recorder.PcapRecorder;
import inet.linklayer.common.IIeee8021dQosClassifier;
import inet.linklayer.contract.IWirelessInterface;
import inet.linklayer.ieee80211.llc.IIeee80211Llc;
import inet.linklayer.ieee80211.mgmt.IIeee80211Agent;
import inet.linklayer.ieee80211.mgmt.IIeee80211Mgmt;
import inet.networklayer.common.NetworkInterface;
import inet.networklayer.common.InterfaceTable;
import inet.common.MessageDispatcher;
import inet.physicallayer.wireless.common.contract.packetlevel.IRadio;
import inet.linklayer.ethernet.contract.IEthernetLayer;
import inet.linklayer.ieee8021q.IIeee8021qLayer;
import inet.linklayer.ieee8021r.IIeee8021rLayer;
import inet.node.contract.IEthernetNetworkNode;
import inet.protocolelement.contract.IProtocolLayer;
import inet.node.base.NodeBase;
import inet.linklayer.ieee80211.llc.IIeee80211Llc;

module MLOLinkLayerNodeBase extends NodeBase
{
    parameters:
        @figure[linkLayer](type=rectangle; pos=250,458; size=1000,284; fillColor=#0000ff; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[linkLayer.title](type=text; pos=1245,463; anchor=ne; text="link layer");
        @figure[interfaceLayer](type=rectangle; pos=250,758; size=1000,360; fillColor=#00ffff; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[interfaceLayer.title](type=text; pos=1245,763; anchor=ne; text="interface layer");
        @display("bgb=2693.7942,2316.708");
            
        string opMode @enum("a","b","g(erp)","g(mixed)","n(mixed-2.4Ghz)","p","ac") = default("ac");
        int numLoInterfaces = default(1);
        bool recordPcap = default(false);
        int numPcapRecorders = default(recordPcap ? 1 : 0);
		int numWlanInterfaces = default(2);
        string fcsMode @enum("declared","computed") = default("declared");
        mobility.typename = default(numWlanInterfaces > 0 ? "StationaryMobility" : "");
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        *.fcsMode = this.fcsMode;		
        ethernet.registerProtocol = default(true);
        
        lmac[*].radio.antenna.mobilityModule = default("^.^.^.mobility");

    gates:
        input radioIn[numWlanInterfaces] @directIn;
    submodules:
        pcapRecorder[numPcapRecorders]: PcapRecorder {
            @display("p=125,729;is=s");
        }
        interfaceTable: InterfaceTable {
            @display("p=125,240;is=s");
        }
        ethernet: <default("")> like IEthernetLayer if typename != "" {
            @display("p=481,853");
        }
        ieee8021q: <default("")> like IIeee8021qLayer if typename != "" {
            @display("p=569,853");
        }
        ieee8021r: <default("")> like IIeee8021rLayer if typename != "" {
            @display("p=647,853");
        }
        li: MessageDispatcher {
            @display("p=718,961;b=1000,5,,,,1");
        }
        umac: <default("UMac")> like IUMac {
            @display("p=820.4196,1062.484,row,150;q=queue");
        }
        classifier: <default("RandomQosClassifier")> like IIeee8021dQosClassifier {
            @display("p=605.9733,1062.484");
        }
        bl: MessageDispatcher {
            @display("p=716.4457,1187.5778;b=1000,5");
        }
        lmac[numWlanInterfaces]: <default("Ieee80211LInterface")> like IWirelessInterface {
            @display("p=718.07025,1393.9011,row,150;q=queue");
        }

    connections allowunconnected:

        //umac.lowerLayerOut --> classifier.in;
        umac.lowerLayerOut --> bl.in++;
        //umac.lowerLayerProtocolIndicatorOut --> bl.in++;

        umac.classifierIn <-- classifier.out;
        umac.classifierOut --> classifier.in;

        //bl.in++ <-- classifier.out;

        //umac.lowerLayerOut --> bl.in++;
        bl.out++ --> umac.lowerLayerIn;

        li.out++ --> umac.upperLayerIn;
        li.in++ <-- umac.upperLayerOut;

        bl.out++ --> li.in++;
        li.out++ --> bl.in++;

        ieee8021q.lowerLayerOut --> li.in++ if exists(ieee8021q);
        li.out++ --> ieee8021q.lowerLayerIn if exists(ieee8021q);

        ieee8021r.lowerLayerOut --> li.in++ if exists(ieee8021r);
        li.out++ --> ieee8021r.lowerLayerIn if exists(ieee8021r);

        ethernet.lowerLayerOut --> li.in++ if exists(ethernet);
        li.out++ --> ethernet.lowerLayerIn if exists(ethernet);

        for i=0..sizeof(radioIn)-1 {
            lmac[i].upperLayerOut --> bl.in++;
            lmac[i].upperLayerIn <-- bl.out++;

            radioIn[i] --> { @display("m=s"); } --> lmac[i].radioIn;
        }

}


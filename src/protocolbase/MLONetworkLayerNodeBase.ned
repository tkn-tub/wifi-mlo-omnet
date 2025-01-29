//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


package mlo.protocolbase;

import inet.common.MessageDispatcher;
import inet.networklayer.contract.INetworkLayer;

module MLONetworkLayerNodeBase extends MLOLinkLayerNodeBase
{
    parameters:
        bool hasIpv4 = default(true);
        bool hasIpv6 = default(false);
        bool hasGn = default(false);
        bool forwarding = default(false);
        bool multicastForwarding = default(false);
        *.forwarding = this.forwarding;
        *.multicastForwarding = this.multicastForwarding;
        @figure[networkLayer](type=rectangle; pos=250,308; size=1000,134; fillColor=#00ff00; lineColor=#808080; cornerRadius=5; fillOpacity=0.1);
        @figure[networkLayer.title](type=text; pos=1245,313; anchor=ne; text="network layer");
    submodules:
        ipv4: <default("Ipv4NetworkLayer")> like INetworkLayer if hasIpv4 {
            @display("p=442,431;q=queue");
        }
        ipv6: <default("Ipv6NetworkLayer")> like INetworkLayer if hasIpv6 {
            @display("p=536,431;q=queue");
        }
        generic: <default("")> like INetworkLayer if hasGn {
            @display("p=630,431;q=queue");
        }
        nl: MessageDispatcher {
            @display("p=718,538;b=1000,5,,,,1");
        }
    connections allowunconnected:
        ipv4.ifIn <-- nl.out++ if hasIpv4;
        ipv4.ifOut --> nl.in++ if hasIpv4;

        ipv6.ifIn <-- nl.out++ if hasIpv6;
        ipv6.ifOut --> nl.in++ if hasIpv6;

        generic.ifIn <-- nl.out++ if exists(generic);
        generic.ifOut --> nl.in++ if exists(generic);
    
    	nl.out++ --> li.in++;
    	li.out++ --> nl.in++;
    
        nl.out++ --> ieee8021q.upperLayerIn if exists(ieee8021q);
        ieee8021q.upperLayerOut --> nl.in++ if exists(ieee8021q);

        nl.out++ --> ieee8021r.upperLayerIn if exists(ieee8021r);
        ieee8021r.upperLayerOut --> nl.in++ if exists(ieee8021r);

        nl.out++ --> ethernet.upperLayerIn if exists(ethernet);
        ethernet.upperLayerOut --> nl.in++ if exists(ethernet);
        

}


//
// Copyright (C) 2020 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


package mlo.node;

import mlo.protocolbase.MLOApplicationLayerNodeBase;

//
// Models a host with (default) one wireless (802.11) card in infrastructure mode.
// This module is basically a ~StandardHost with an ~Ieee80211Interface with
// mgmt.typename = ~Ieee80211MgmtSta added.
// It should be used in conjunction with ~AccessPoint, or any other AP model
// which contains ~Ieee80211Interface with mgmt.typename = ~Ieee80211MgmtAp.
//
module MLOStation extends MLOApplicationLayerNodeBase
{
    parameters:
        @display("i=device/wifilaptop");
        @figure[submodules];
        forwarding = default(false);  // disable routing by default
        ipv4.arp.proxyArpInterfaces = default(""); // proxy arp is disabled on hosts by default
        *.routingTableModule = default("^.ipv4.routingTable");
        numWlanInterfaces = default(2);
}


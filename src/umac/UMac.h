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

#ifndef __LUMACBASIC_UMAC_H_
#define __LUMACBASIC_UMAC_H_

#include <omnetpp.h>

#include <list>
#include <map>
#include <set>

#include "inet/common/LayeredProtocolBase.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/networklayer/common/NetworkInterface.h"

#include "src/lmac/mac/Ieee80211LMac.h"

struct QosParameters {
    QosParameters(): snr(0) {}
    QosParameters(double s) : snr{s} {};
    QosParameters(int i, double s) : ifaceId(i), snr{s} {};
    double snr;
    int ifaceId = 0;
};

inline std::ostream& operator<<(std::ostream& os, const QosParameters& params) {
    return os << "Interface Id: " << params.ifaceId << endl
            << "Last avg. SNIR: " << params.snr;
}

struct LinkOverview {
    QosParameters qosParams;
    std::vector<ChannelAccessParameters> channelParamsPerAc; // Channel access parameters for every AC on the corresponding link
    std::vector<int> receivedPackets;
    std::vector<double> latencyPackets;
};

namespace inet {

class IInterfaceTable;

class UMac : public LayeredProtocolBase, public DefaultProtocolRegistrationListener
{
    protected:

      simsignal_t rcvdPacketsUnique;

      int upperLayerInGateId = -1;
      int upperLayerOutGateId = 1;

      int upperLayerMgmtInGateId = -1;
      int upperLayerMgmtOutGateId = 1;

      int upperLayerLlcInGateId = -1;
      int upperLayerLlcOutGateId = 1;

      int lowerLayerInGateId;
      int lowerLayerOutGateId;

      int lowerLayerMgmtInGateId;
      int lowerLayerMgmtOutGateId;

      int lowerLayerLlcInGateId;
      int lowerLayerLlcOutGateId;

      int classifierInGateId = -1;
      int classifierOutGateId = -1;

      ModuleRefByPar<IInterfaceTable> ift;
      std::map<int, LinkOverview> lot;

      bool hasClassifier = false;
      bool isStation = true;

      NetworkInterface **primaryLinkPerAc = nullptr;

      const char *defaultInterfaceName = nullptr;

      void reassignNetworkAddress() ;
      void reassignMacAddress() ;

      virtual void initialize(int stage) override;
      virtual void finish() override;

      virtual void handleMessageWhenUp(cMessage *message) override;
      virtual void handleSelfMessage(cMessage *message) override;

      virtual void handleUpperMessage(cMessage *message) override;
      virtual void handleLowerMessage(cMessage *message) override;

      virtual void handleUpperCommand(cMessage *message) override;
      virtual void handleLowerCommand(cMessage *message) override;

      virtual void handleUpperPacket(Packet *packet) override;
      virtual void handleLowerPacket(Packet *packet) override;

      virtual bool isUpperMessage(cMessage *message) const override;
      virtual bool isLowerMessage(cMessage *message) const override;

      // UMAC is an intermediate layer and should always be operational.
      virtual void handleStartOperation(LifecycleOperation *operation) override { }
      virtual void handleStopOperation(LifecycleOperation *operation) override { }
      virtual void handleCrashOperation(LifecycleOperation *operation) override { }

      virtual bool isInitializeStage(int stage) const override { return stage == INITSTAGE_LINK_LAYER; }
      virtual bool isModuleStartStage(int stage) const override { return stage == ModuleStartOperation::STAGE_LINK_LAYER; }
      virtual bool isModuleStopStage(int stage) const override { return stage == ModuleStopOperation::STAGE_LINK_LAYER; }

      bool isClassifierMessage(cMessage *message) const;
      void handleClassifierMessage(cMessage *message);

      void updateChannelAccessParameters();
      virtual void updateLinkQosParameters(Packet *packet);
      void printLinkOverview();

      virtual void sendPacket(Packet *packet);
      virtual void sendTestPacket(Packet *packet);

      virtual void updateStatistics(Packet *packet);
      virtual void recordStatistics();

    };
}

#endif

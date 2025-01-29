
package mlo.simulations;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.physicallayer.wireless.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.visualizer.common.IntegratedVisualizer;
import inet.visualizer.canvas.integrated.IntegratedMultiCanvasVisualizer;
import inet.common.lifecycle.NodeStatus;
import inet.common.scenario.ScenarioManager;

import mlo.node.MLOStation;
import mlo.node.MLOAccessPoint;

network MLONetwork
{
    parameters:
        @display("bgb=591,267");
       
        @statistic[throughput](source=liveThroughput(host2.app[0].packetReceived)/1000000; record=figure; targetFigure=panel.throughput; checkSignals=false);
        @statistic[numRcvdPk](source=count(host2.app[0].packetReceived); record=figure; targetFigure=panel.numRcvdPkCounter; checkSignals=false);
        @figure[panel](type=panel; pos=1.5,0.1);
    //    @figure[panel.throughput](type=gauge; pos=0,0; size=100,100; minValue=0; maxValue=40; tickSize=5; label="App level throughput [Mbps]");
        @figure[panel.throughput](type=linearGauge; pos=250,50; size=250,30; minValue=0; maxValue=54; tickSize=6; label="Application level throughput [Mbps]");
        @figure[panel.numRcvdPkCounter](type=counter; pos=50,50; label="Packets received"; decimalPlaces=6);

    submodules:
        visualizer: IntegratedMultiCanvasVisualizer {
            parameters:
                @display("p=50,36");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            parameters:
                @display("p=50,100");
        }
        configurator: Ipv4NetworkConfigurator {
            parameters:
                @display("p=50,167");
        }
        scenarioManager: ScenarioManager {
             parameters:
                @display("p=50,300");            
        }
        host1: MLOStation {
            parameters:
                @display("p=173.41832,186.16296");
        }
        host2: MLOStation {
            parameters:
                @display("p=437.4147,186.16296");
        }
        accessPoint: MLOAccessPoint {
            parameters:
                @display("p=308.1475,99.6814");
        }
}



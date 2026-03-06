
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/aodv-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MANET_AODV_Mobility");

int
main (int argc, char *argv[])
{
  double speed = 5.0;                 
  double simTime = 150.0;             
  uint32_t numNodes = 20;
  double pauseTime = 2.0;             
  uint32_t packetSize = 512;          
  std::string dataRate = "1Mbps";
  uint16_t port = 5000;

  CommandLine cmd;
  cmd.AddValue ("speed", "Node speed in m/s (constant for RandomWaypoint)", speed);
  cmd.AddValue ("nodes", "Number of nodes", numNodes);
  cmd.AddValue ("simTime", "Simulation time (s)", simTime);
  cmd.Parse (argc, argv);

  NodeContainer nodes;
  nodes.Create (numNodes);

  WifiHelper wifi;
  wifi.SetStandard (WIFI_STANDARD_80211b);

  YansWifiPhyHelper phy;
  YansWifiChannelHelper channel;
  channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  channel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  phy.SetChannel (channel.Create ());

  WifiMacHelper mac;
  mac.SetType ("ns3::AdhocWifiMac");

  NetDeviceContainer devices = wifi.Install (phy, mac, nodes);

  MobilityHelper mobility;

  Ptr<RandomRectanglePositionAllocator> positionAlloc = CreateObject<RandomRectanglePositionAllocator> ();
  Ptr<UniformRandomVariable> xval = CreateObject<UniformRandomVariable> ();
  xval->SetAttribute ("Min", DoubleValue (0.0));
  xval->SetAttribute ("Max", DoubleValue (1000.0));
  Ptr<UniformRandomVariable> yval = CreateObject<UniformRandomVariable> ();
  yval->SetAttribute ("Min", DoubleValue (0.0));
  yval->SetAttribute ("Max", DoubleValue (1000.0));
  positionAlloc->SetX (xval);
  positionAlloc->SetY (yval);

  mobility.SetPositionAllocator (positionAlloc);

  Ptr<ConstantRandomVariable> speedVar = CreateObject<ConstantRandomVariable> ();
  speedVar->SetAttribute ("Constant", DoubleValue (speed));

  Ptr<ConstantRandomVariable> pauseVar = CreateObject<ConstantRandomVariable> ();
  pauseVar->SetAttribute ("Constant", DoubleValue (pauseTime));

  mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
                             "Speed", PointerValue (speedVar),
                             "Pause", PointerValue (pauseVar),
                             "PositionAllocator", PointerValue (positionAlloc));

  mobility.Install (nodes);

  AodvHelper aodv;
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv);
  stack.Install (nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     InetSocketAddress (interfaces.GetAddress (numNodes - 1), port));
  onoff.SetAttribute ("DataRate", StringValue (dataRate));
  onoff.SetAttribute ("PacketSize", UintegerValue (packetSize));
  onoff.SetAttribute ("StartTime", TimeValue (Seconds (1.0)));
  onoff.SetAttribute ("StopTime", TimeValue (Seconds (simTime - 1.0)));

  ApplicationContainer clientApps = onoff.Install (nodes.Get (0));

  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApp = sink.Install (nodes.Get (numNodes - 1));
  sinkApp.Start (Seconds (0.0));
  sinkApp.Stop (Seconds (simTime));

  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();

  Simulator::Stop (Seconds (simTime));
  Simulator::Run ();

  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

  std::cout << "=== Simulation results ===\n";
  std::cout << "Nodes: " << numNodes << ", Area: 1000x1000, Speed: " << speed << " m/s\n";
  std::cout << "-------------------------------------\n";

  for (auto &flow : stats)
    {
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (flow.first);

      if (t.destinationAddress == interfaces.GetAddress (numNodes - 1) && t.destinationPort == port)
        {
          FlowMonitor::FlowStats st = flow.second;

          double timeInterval = simTime; 
          double throughputKbps = (st.rxBytes * 8.0) / (timeInterval * 1000.0); 
          double pdr = (st.txPackets > 0) ? ((double) st.rxPackets / (double) st.txPackets) * 100.0 : 0.0;
          double avgDelayMs = (st.rxPackets > 0) ? (st.delaySum.GetSeconds () / st.rxPackets) * 1000.0 : 0.0;

          std::cout << "Flow ID: " << flow.first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Throughput: " << throughputKbps << " kbps\n";
          std::cout << "  PDR: " << pdr << " %\n";
          std::cout << "  Avg delay: " << avgDelayMs << " ms\n";
          std::cout << "  Tx packets: " << st.txPackets << "  Rx packets: " << st.rxPackets << "  Lost: " << st.lostPackets << "\n";
          std::cout << "-------------------------------------\n";
        }
    }

  Simulator::Destroy ();
  return 0;
}
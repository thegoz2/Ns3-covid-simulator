/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/ssid.h"
#include "ns3/csma-module.h"
#include "ns3/ipv4-global-routing-helper.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{
  CommandLine cmd (__FILE__);
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer p2pnode;
  p2pnode.Create (2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pnode);


  NodeContainer wifiStaNodes;
   wifiStaNodes.Create (2);
   NodeContainer wifiApNode = p2pnode.Get (0);

   YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();
   YansWifiPhyHelper phy;
   phy.SetChannel (channel.Create ());

   WifiHelper wifi;
   wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

   WifiMacHelper mac;
   Ssid ssid = Ssid ("ns-3-ssid");
   mac.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid),
                "ActiveProbing", BooleanValue (false));

   NetDeviceContainer staDevices;
   staDevices = wifi.Install (phy, mac, wifiStaNodes);

   mac.SetType ("ns3::ApWifiMac",
                "Ssid", SsidValue (ssid));

   NetDeviceContainer apDevices;
   apDevices = wifi.Install (phy, mac, wifiApNode);


   //customer
   NodeContainer csmaNodes;
     csmaNodes.Add (p2pnode.Get (1));
     csmaNodes.Create (3);

     CsmaHelper csma;
       csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
       csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

       NetDeviceContainer csmaDevices;
        csmaDevices = csma.Install (csmaNodes);

 //
        InternetStackHelper stack;
        stack.Install (csmaNodes);
          stack.Install (wifiApNode);
          stack.Install (wifiStaNodes);
   Ipv4AddressHelper address;

     address.SetBase ("10.1.1.0", "255.255.255.0");
     Ipv4InterfaceContainer p2pInterfaces;
     p2pInterfaces = address.Assign (p2pDevices);

     address.SetBase ("10.1.2.0", "255.255.255.0");
     Ipv4InterfaceContainer csmaInterfaces;
     csmaInterfaces = address.Assign (csmaDevices);

     address.SetBase ("10.1.3.0", "255.255.255.0");
     address.Assign (staDevices);
     address.Assign (apDevices);

     MobilityHelper mobility;

       mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                      "MinX", DoubleValue (15.0),
                                      "MinY", DoubleValue (17.0),
                                      "DeltaX", DoubleValue (5.0),
                                      "DeltaY", DoubleValue (10.0),
                                      "GridWidth", UintegerValue (5),
                                      "LayoutType", StringValue ("RowFirst"));
    //bug detected here
       mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                                "Bounds", RectangleValue (Rectangle (-20,20, -20, 20)));
       mobility.Install (wifiStaNodes);

       mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
       mobility.Install (wifiApNode);

     UdpEchoServerHelper echoServer (9);

       ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (3));
       serverApps.Start (Seconds (1.0));
       serverApps.Stop (Seconds (10.0));

       UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (3), 9);
       echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
       echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
       echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

       ApplicationContainer clientApps = echoClient.Install (p2pnode.Get (0));
       clientApps.Start (Seconds (2.0));
       clientApps.Stop (Seconds (10.0));

       Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
       Simulator::Stop(Seconds(3));

  AnimationInterface anim("oktestzx.xml");
      anim.SetConstantPosition(p2pnode.Get(0), 30.0, 10.0);
      anim.SetConstantPosition(p2pnode.Get(1), 50.0, 10.0);
      anim.SetConstantPosition(csmaNodes.Get(3), 60.0, 20.0);
      anim.SetConstantPosition(csmaNodes.Get(1), 50.0, 30.0);
      anim.SetConstantPosition(csmaNodes.Get(2), 60.0, 30.0);




  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

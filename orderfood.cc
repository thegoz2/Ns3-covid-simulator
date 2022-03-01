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
#include "ns3/packet-socket-address.h"
#include "ns3/packet-socket-helper.h"
#include "ns3/simulator.h"


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
     Ipv4InterfaceContainer staInterface;
     staInterface = address.Assign (staDevices);
     Ipv4InterfaceContainer ApInterface;
         ApInterface = address.Assign (apDevices);

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
                                "Bounds", RectangleValue (Rectangle (-50,50, -50, 50)));
       mobility.Install (wifiStaNodes);

       mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
       mobility.Install (wifiApNode);
//
//       PacketSocketAddress socket;
//       socket.SetSingleDevice(p2pDevices.Get(0)->GetIfIndex());
//       socket.SetPhysicalAddress(p2pDevices.Get(1) ->GetAddress());
//       socket.SetProtocol(1);
//
//      OnOffHelper onoff1("ns3::UdpPacketSocketFactory", Address());
//      onoff1.SetConstantRate(DataRate(6000000));
//      onoff1.SetAttribute("PacketSize", UintegerValue(2000));
//
//       OnOffHelper onoff1("ns3::UdpPacketSocketFactory", Address());
//       onoff1.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
//       onoff1.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));
//       onoff1.SetAttribute("DataRate",DataRateValue());
//
//
//      TypeId tid = TypeId::LookupByName("ns3::UdpSocketFactory");
//      Ptr<Socket> sink = Socket::CreateSocket(p2pnode.Get(0), tid);
//      InetSocketAddress local = InetSocketAddress (p2pInterfaces.GetAddress(1), 9);
//      sink->Bind(local);
//      AddressValue remoteAddress (InetSocketAddress(p2pInterfaces.GetAddress(1), 9));
//      onoff1.SetAttribute("Remote", remoteAddress);

//
//     Ptr<UniformRandomVariable> var = CreateObject<UniformRandomVariable> ();
//      ApplicationContainer apps = onoff1.Install(p2pnode.Get(0));
//      apps.Start (Seconds(0.5));
//      apps.Stop (Seconds(10.0));




     UdpEchoServerHelper echoServer (9);
     UdpEchoServerHelper echoServer2 (10);
     UdpEchoServerHelper echoServer3 (11);
    UdpEchoServerHelper echoServer4 (12);
     UdpEchoServerHelper echoServer5 (13);

       ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (0));

       serverApps.Start (Seconds (1));
       serverApps.Stop (Seconds (10.0));

       ApplicationContainer serverApps2 = echoServer2.Install (csmaNodes.Get (1));

       serverApps2.Start (Seconds (1));
       serverApps2.Stop (Seconds (10.0));

              ApplicationContainer serverApps3 = echoServer3.Install (csmaNodes.Get (2));
                     serverApps3.Start (Seconds (1));
                     serverApps3.Stop (Seconds (10.0));

                     ApplicationContainer serverApps4 = echoServer4.Install (wifiStaNodes.Get (0));
                                          serverApps4.Start (Seconds (0.5));
                                          serverApps4.Stop (Seconds (10.0));

                      ApplicationContainer serverApps5 = echoServer5.Install (wifiStaNodes.Get (1));
                                       serverApps5.Start (Seconds (0.5));
                                       serverApps5.Stop (Seconds (10.0));




       UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (0), 9);
       echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
       echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
       echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

       UdpEchoClientHelper echoClient2 (csmaInterfaces.GetAddress (1), 10);
              echoClient2.SetAttribute ("MaxPackets", UintegerValue (1));
              echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
              echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

        UdpEchoClientHelper echoClient3 (csmaInterfaces.GetAddress (3), 11);
                     echoClient3.SetAttribute ("MaxPackets", UintegerValue (1));
                     echoClient3.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
                     echoClient3.SetAttribute ("PacketSize", UintegerValue (1024));
  UdpEchoClientHelper echoClient4 (staInterface.GetAddress (0), 12);
                                          echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
                                          echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
                                          echoClient.SetAttribute ("PacketSize", UintegerValue (10000));
        UdpEchoClientHelper echoClient5 (staInterface.GetAddress (1), 12);
                           echoClient5.SetAttribute ("MaxPackets", UintegerValue (1));
                           echoClient5.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
                           echoClient5.SetAttribute ("PacketSize", UintegerValue (10000));


       ApplicationContainer clientApps = echoClient.Install (p2pnode.Get (0));

       clientApps.Start (Seconds (0.8));
       clientApps.Stop (Seconds (10.0));
       ApplicationContainer clientApps2 = echoClient2.Install (p2pnode.Get (0));
          clientApps2.Start (Seconds (0.82));
          clientApps2.Stop (Seconds (10.0));
          ApplicationContainer clientApps3 = echoClient3.Install (p2pnode.Get (0));
             clientApps3.Start (Seconds (0.83));
             clientApps3.Stop (Seconds (10.0));
         ApplicationContainer clientApps4 = echoClient4.Install (p2pnode.Get (0));
                          clientApps4.Start(Seconds (0.2));
                          clientApps4.Stop (Seconds (10.0));
            ApplicationContainer clientApps5 = echoClient5.Install (p2pnode.Get (0));
                            clientApps5.Start(Seconds (0.3));
                            clientApps5.Stop (Seconds (10.0));

       Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
       Simulator::Stop(Seconds(10));

  AnimationInterface anim("orderfood.xml");
      anim.SetConstantPosition(p2pnode.Get(0), 30.0, 10.0);
      anim.SetConstantPosition(p2pnode.Get(1), 50.0, 10.0);
      anim.SetConstantPosition(csmaNodes.Get(3), 60.0, 20.0);
      anim.SetConstantPosition(csmaNodes.Get(1), 50.0, 30.0);
      anim.SetConstantPosition(csmaNodes.Get(2), 60.0, 30.0);
      anim.UpdateNodeColor(p2pnode.Get(0), 255, 255, 0);
      anim.UpdateNodeColor(p2pnode.Get(1), 255, 255, 0);



AsciiTraceHelper ascii;
Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream("foodorderP2p.tr");
//Check p2p//
pointToPoint.EnableAsciiAll(stream);
AsciiTraceHelper ascii2;
Ptr<OutputStreamWrapper> streamc = ascii2.CreateFileStream("foodorderCsma.tr");
csma.EnableAsciiAll(streamc);

//checkCsma//
AsciiTraceHelper ascii3;
Ptr<OutputStreamWrapper> streamw = ascii3.CreateFileStream("foodorderWifi.tr");
//checkWifi//
phy.EnableAsciiAll(streamw);

  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
//void  calInfect( signed char status,signed char node){
//	srand (time(NULL));
//
//	               	  	  	  	  	 int rate_covid = 0;
//	               	  	  	  	  	 int time = 1;
//	               	  	  	  	NS_LOG_INFO ("Rate Up::::" <<rate_covid);
//	                                  int infection1 = rand()%10+time;
//	                                  // 70% oppnity get covid
//	                                  if(infection1 > 7  ){
//	                                  NS_LOG_INFO ("\n Status:still Alive " <<infection1);
//
//
//	                                  }
//	                                  else if( status == 'Infected'){
//
//
//	                                	  NS_LOG_INFO ("You are Infected " <<infection1);
//	                                	  anim.UpdateNodeColor(node, 255, 255, 0);
//	                                  }else {
//	                                	  NS_LOG_INFO ("\n Status:still Alive " <<infection1);
//	                                  }
//
//}



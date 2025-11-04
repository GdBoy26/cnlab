#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("StopAndWaitWithNetAnim");

uint32_t packetCount = 0;
uint32_t maxPackets = 5;
uint32_t seqNum = 0;
bool ackReceived = true;

Ptr<Socket> senderSocket;
Ptr<Socket> receiverSocket;

void SendPacket();
void ReceivePacket(Ptr<Socket> socket);

void SendPacket() {
    if (packetCount >= maxPackets) {
        NS_LOG_UNCOND("Simulation finished after sending " << maxPackets << " packets.");
        Simulator::Stop(Seconds(0.1));
        return;
    }
    if (ackReceived) {
        Ptr<Packet> packet = Create<Packet>(100);
        senderSocket->Send(packet);
        NS_LOG_UNCOND("Sender: Sent Packet Seq=" << seqNum << " at " << Simulator::Now().GetSeconds() << "s");
        ackReceived = false;
        Simulator::Schedule(Seconds(2.0), &SendPacket);
    } else {
        NS_LOG_UNCOND("Sender: Timeout! Retransmitting Seq=" << seqNum << " at " << Simulator::Now().GetSeconds() << "s");
        Ptr<Packet> packet = Create<Packet>(100);
        senderSocket->Send(packet);
        Simulator::Schedule(Seconds(2.0), &SendPacket);
    }
}

void ReceivePacket(Ptr<Socket> socket) {
    Ptr<Packet> packet;
    while ((packet = socket->Recv())) {
        NS_LOG_UNCOND("Receiver: Got Packet Seq=" << seqNum << " at " << Simulator::Now().GetSeconds() << "s");
        ackReceived = true;
        NS_LOG_UNCOND("Receiver: Sent ACK for Seq=" << seqNum << " at " << Simulator::Now().GetSeconds() << "s");
        seqNum++;
        packetCount++;
    }
}

int main(int argc, char *argv[]) {
    NodeContainer nodes;
    nodes.Create(2);

    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer devices = p2p.Install(nodes);

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    senderSocket = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());
    receiverSocket = Socket::CreateSocket(nodes.Get(1), UdpSocketFactory::GetTypeId());

    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 8080);
    receiverSocket->Bind(local);
    receiverSocket->SetRecvCallback(MakeCallback(&ReceivePacket));

    InetSocketAddress remote = InetSocketAddress(interfaces.GetAddress(1), 8080);
    senderSocket->Connect(remote);

    Simulator::Schedule(Seconds(1.0), &SendPacket);

    AnimationInterface anim("stop-and-wait.xml");
    anim.SetConstantPosition(nodes.Get(0), 10, 30);
    anim.SetConstantPosition(nodes.Get(1), 50, 30);

    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
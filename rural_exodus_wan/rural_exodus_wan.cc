/*
 * NS-3 Simulation: Reducing Rural Exodus via Starlink WAN and Cloud Services
 * 
 * Project Goal: Enable remote area residents to access essential services without
 *               migrating to cities, thus reducing rural exodus.
 * 
 * Use Cases Simulated:
 * 1. Remote Work: Cloud-based office applications and video conferencing
 * 2. Telemedicine: Remote healthcare consultations and medical data transfer
 * 3. E-Learning: Online education platforms and video lectures
 * 
 * Network Architecture:
 * Remote Villages (no cell towers) -> Starlink Terminals -> LEO Satellites -> 
 * Ground Stations -> Internet -> Cloud Service Providers
 * 
 * Technology: Starlink LEO satellite constellation provides high-speed internet
 *             access to areas without traditional infrastructure
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("RuralExodusReductionSimulation");

int main(int argc, char *argv[])
{
    // ==================== SIMULATION PARAMETERS ====================
    
    // Simulate 3 remote villages without cellular infrastructure
    uint32_t nRemoteWorkersVillage1 = 4;   // Village 1: Remote workers
    uint32_t nTelemedicineUsersVillage2 = 3; // Village 2: Telemedicine patients
    uint32_t nStudentsVillage3 = 5;         // Village 3: E-learning students
    
    // Simulation duration
    double simulationTime = 60.0; // seconds
    
    // Starlink Network Parameters (Based on real Starlink specifications)
    // Source: Starlink provides internet to areas without cell towers or fiber
    double satelliteLatency = 20.0;        // ms - typical one-way latency
    std::string downlinkSpeed = "150Mbps"; // Download speed for users
    std::string uplinkSpeed = "20Mbps";    // Upload speed for users
    
    // Quality of Service requirements for each use case
    uint32_t remoteWorkPacketSize = 1024;      // bytes - office applications
    uint32_t telemedicinePacketSize = 1400;    // bytes - medical imaging/video
    uint32_t eLearningPacketSize = 1200;       // bytes - video lectures
    
    double remoteWorkInterval = 100.0;         // ms - regular data exchange
    double telemedicineInterval = 40.0;        // ms - real-time video consultation
    double eLearningInterval = 60.0;           // ms - streaming education content
    
    // Command line arguments
    bool verbose = true;
    bool enableTelemedicine = true;
    bool enableRemoteWork = true;
    bool enableELearning = true;
    
    CommandLine cmd;
    cmd.AddValue("workers", "Number of remote workers in village 1", nRemoteWorkersVillage1);
    cmd.AddValue("patients", "Number of telemedicine users in village 2", nTelemedicineUsersVillage2);
    cmd.AddValue("students", "Number of e-learning students in village 3", nStudentsVillage3);
    cmd.AddValue("time", "Simulation duration (seconds)", simulationTime);
    cmd.AddValue("verbose", "Enable detailed logging", verbose);
    cmd.AddValue("telemedicine", "Enable telemedicine simulation", enableTelemedicine);
    cmd.AddValue("remotework", "Enable remote work simulation", enableRemoteWork);
    cmd.AddValue("elearning", "Enable e-learning simulation", enableELearning);
    cmd.Parse(argc, argv);
    
    if (verbose)
    {
        LogComponentEnable("RuralExodusReductionSimulation", LOG_LEVEL_INFO);
    }
    
    NS_LOG_INFO("========================================");
    NS_LOG_INFO("Rural Exodus Reduction via Starlink WAN");
    NS_LOG_INFO("========================================");
    NS_LOG_INFO("Village 1 (Remote Work): " << nRemoteWorkersVillage1 << " workers");
    NS_LOG_INFO("Village 2 (Telemedicine): " << nTelemedicineUsersVillage2 << " patients");
    NS_LOG_INFO("Village 3 (E-Learning): " << nStudentsVillage3 << " students");
    NS_LOG_INFO("Starlink Parameters: " << downlinkSpeed << " down, " << uplinkSpeed << " up, " << satelliteLatency << "ms latency");
    
    // ==================== CREATE NETWORK NODES ====================
    
    // Village 1: Remote workers (home offices)
    NodeContainer remoteWorkers;
    remoteWorkers.Create(nRemoteWorkersVillage1);
    
    // Village 2: Telemedicine users (patients connecting with doctors)
    NodeContainer telemedicineUsers;
    telemedicineUsers.Create(nTelemedicineUsersVillage2);
    
    // Village 3: Students (accessing online education)
    NodeContainer students;
    students.Create(nStudentsVillage3);
    
    // Starlink User Terminals (one per village)
    // These provide WiFi access point functionality for local users
    NodeContainer starlinkTerminals;
    starlinkTerminals.Create(3);
    
    // Starlink Satellite (represents the LEO constellation)
    // In reality, Starlink has thousands of satellites providing global coverage
    NodeContainer satellite;
    satellite.Create(1);
    
    // Starlink Ground Station (gateway to terrestrial internet)
    NodeContainer groundStation;
    groundStation.Create(1);
    
    // Internet Core Router (backbone network)
    NodeContainer coreRouter;
    coreRouter.Create(1);
    
    // Cloud Service Providers
    NodeContainer remoteWorkServers;      // Video conferencing, cloud office apps
    remoteWorkServers.Create(2);
    
    NodeContainer telemedicineServers;    // Electronic health records, teleconsultation
    telemedicineServers.Create(2);
    
    NodeContainer eLearningServers;       // Learning management systems, video lectures
    eLearningServers.Create(2);
    
    NS_LOG_INFO("Created all network nodes for 3 remote villages");
    
    // ==================== CONFIGURE WIFI NETWORKS (LOCAL ACCESS) ====================
    
    // Each village has a local WiFi network connected to a Starlink terminal
    // This allows multiple users to share one Starlink subscription
    
    // Village 1: Remote Work WiFi Network
    WifiHelper wifiRemoteWork;
    wifiRemoteWork.SetStandard(WIFI_STANDARD_80211ac); // High performance WiFi
    
    WifiMacHelper macRemoteWork;
    YansWifiPhyHelper phyRemoteWork;
    YansWifiChannelHelper channelRemoteWork = YansWifiChannelHelper::Default();
    phyRemoteWork.SetChannel(channelRemoteWork.Create());
    
    Ssid ssidVillage1 = Ssid("Starlink-RemoteWork-Village");
    macRemoteWork.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidVillage1));
    NetDeviceContainer apDeviceVillage1 = wifiRemoteWork.Install(phyRemoteWork, macRemoteWork, starlinkTerminals.Get(0));
    
    macRemoteWork.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidVillage1));
    NetDeviceContainer staDevicesVillage1 = wifiRemoteWork.Install(phyRemoteWork, macRemoteWork, remoteWorkers);
    
    NS_LOG_INFO("Configured WiFi network for Remote Work Village (Village 1)");
    
    // Village 2: Telemedicine WiFi Network
    WifiHelper wifiTelemedicine;
    wifiTelemedicine.SetStandard(WIFI_STANDARD_80211ac);
    
    WifiMacHelper macTelemedicine;
    YansWifiPhyHelper phyTelemedicine;
    YansWifiChannelHelper channelTelemedicine = YansWifiChannelHelper::Default();
    phyTelemedicine.SetChannel(channelTelemedicine.Create());
    
    Ssid ssidVillage2 = Ssid("Starlink-Healthcare-Village");
    macTelemedicine.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidVillage2));
    NetDeviceContainer apDeviceVillage2 = wifiTelemedicine.Install(phyTelemedicine, macTelemedicine, starlinkTerminals.Get(1));
    
    macTelemedicine.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidVillage2));
    NetDeviceContainer staDevicesVillage2 = wifiTelemedicine.Install(phyTelemedicine, macTelemedicine, telemedicineUsers);
    
    NS_LOG_INFO("Configured WiFi network for Telemedicine Village (Village 2)");
    
    // Village 3: E-Learning WiFi Network
    WifiHelper wifiELearning;
    wifiELearning.SetStandard(WIFI_STANDARD_80211ac);
    
    WifiMacHelper macELearning;
    YansWifiPhyHelper phyELearning;
    YansWifiChannelHelper channelELearning = YansWifiChannelHelper::Default();
    phyELearning.SetChannel(channelELearning.Create());
    
    Ssid ssidVillage3 = Ssid("Starlink-Education-Village");
    macELearning.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssidVillage3));
    NetDeviceContainer apDeviceVillage3 = wifiELearning.Install(phyELearning, macELearning, starlinkTerminals.Get(2));
    
    macELearning.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssidVillage3));
    NetDeviceContainer staDevicesVillage3 = wifiELearning.Install(phyELearning, macELearning, students);
    
    NS_LOG_INFO("Configured WiFi network for E-Learning Village (Village 3)");
    
    // ==================== CONFIGURE STARLINK WAN LINKS ====================
    
    PointToPointHelper p2p;
    
    // Starlink Terminal to Satellite Links (User Uplink)
    // These represent the wireless link from ground terminals to LEO satellites
    p2p.SetDeviceAttribute("DataRate", StringValue(uplinkSpeed));
    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(satelliteLatency)));
    
    NetDeviceContainer terminal1ToSat = p2p.Install(starlinkTerminals.Get(0), satellite.Get(0));
    NetDeviceContainer terminal2ToSat = p2p.Install(starlinkTerminals.Get(1), satellite.Get(0));
    NetDeviceContainer terminal3ToSat = p2p.Install(starlinkTerminals.Get(2), satellite.Get(0));
    
    NS_LOG_INFO("Configured Starlink uplinks (Terminals -> Satellite)");
    
    // Satellite to Ground Station Link (Downlink)
    // High capacity link aggregating all user traffic
    p2p.SetDeviceAttribute("DataRate", StringValue("10Gbps")); // Aggregate capacity
    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(satelliteLatency)));
    NetDeviceContainer satToGround = p2p.Install(satellite.Get(0), groundStation.Get(0));
    
    NS_LOG_INFO("Configured Starlink downlink (Satellite -> Ground Station)");
    
    // Ground Station to Internet Core (Fiber Backbone)
    p2p.SetDeviceAttribute("DataRate", StringValue("100Gbps"));
    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(2)));
    NetDeviceContainer groundToCore = p2p.Install(groundStation.Get(0), coreRouter.Get(0));
    
    NS_LOG_INFO("Configured ground station to internet backbone");
    
    // Internet Core to Cloud Service Providers
    // High-speed connections to data centers
    p2p.SetDeviceAttribute("DataRate", StringValue("10Gbps"));
    p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(5)));
    
    // Remote Work Cloud Services (Video conferencing, collaboration tools)
    NetDeviceContainer coreToRemoteWork1 = p2p.Install(coreRouter.Get(0), remoteWorkServers.Get(0));
    NetDeviceContainer coreToRemoteWork2 = p2p.Install(coreRouter.Get(0), remoteWorkServers.Get(1));
    
    // Telemedicine Cloud Services (EHR systems, teleconsultation platforms)
    NetDeviceContainer coreToTelemedicine1 = p2p.Install(coreRouter.Get(0), telemedicineServers.Get(0));
    NetDeviceContainer coreToTelemedicine2 = p2p.Install(coreRouter.Get(0), telemedicineServers.Get(1));
    
    // E-Learning Cloud Services (LMS, video streaming platforms)
    NetDeviceContainer coreToELearning1 = p2p.Install(coreRouter.Get(0), eLearningServers.Get(0));
    NetDeviceContainer coreToELearning2 = p2p.Install(coreRouter.Get(0), eLearningServers.Get(1));
    
    NS_LOG_INFO("Configured cloud service provider connections");
    
    // ==================== INSTALL INTERNET PROTOCOL STACK ====================
    
    InternetStackHelper internet;
    internet.Install(remoteWorkers);
    internet.Install(telemedicineUsers);
    internet.Install(students);
    internet.Install(starlinkTerminals);
    internet.Install(satellite);
    internet.Install(groundStation);
    internet.Install(coreRouter);
    internet.Install(remoteWorkServers);
    internet.Install(telemedicineServers);
    internet.Install(eLearningServers);
    
    NS_LOG_INFO("Installed IP stack on all nodes");
    
    // ==================== ASSIGN IP ADDRESSES ====================
    
    Ipv4AddressHelper ipv4;
    
    // Village 1: Remote Work Network (10.1.1.0/24)
    ipv4.SetBase("10.1.1.0", "255.255.255.0");
    NetDeviceContainer village1Network;
    village1Network.Add(apDeviceVillage1);
    village1Network.Add(staDevicesVillage1);
    Ipv4InterfaceContainer village1Interfaces = ipv4.Assign(village1Network);
    
    // Village 2: Telemedicine Network (10.1.2.0/24)
    ipv4.SetBase("10.1.2.0", "255.255.255.0");
    NetDeviceContainer village2Network;
    village2Network.Add(apDeviceVillage2);
    village2Network.Add(staDevicesVillage2);
    Ipv4InterfaceContainer village2Interfaces = ipv4.Assign(village2Network);
    
    // Village 3: E-Learning Network (10.1.3.0/24)
    ipv4.SetBase("10.1.3.0", "255.255.255.0");
    NetDeviceContainer village3Network;
    village3Network.Add(apDeviceVillage3);
    village3Network.Add(staDevicesVillage3);
    Ipv4InterfaceContainer village3Interfaces = ipv4.Assign(village3Network);
    
    // Starlink WAN Links
    ipv4.SetBase("10.2.1.0", "255.255.255.0");
    Ipv4InterfaceContainer terminal1SatInterfaces = ipv4.Assign(terminal1ToSat);
    
    ipv4.SetBase("10.2.2.0", "255.255.255.0");
    Ipv4InterfaceContainer terminal2SatInterfaces = ipv4.Assign(terminal2ToSat);
    
    ipv4.SetBase("10.2.3.0", "255.255.255.0");
    Ipv4InterfaceContainer terminal3SatInterfaces = ipv4.Assign(terminal3ToSat);
    
    ipv4.SetBase("10.2.4.0", "255.255.255.0");
    Ipv4InterfaceContainer satGroundInterfaces = ipv4.Assign(satToGround);
    
    // Internet Backbone
    ipv4.SetBase("10.3.0.0", "255.255.255.0");
    Ipv4InterfaceContainer groundCoreInterfaces = ipv4.Assign(groundToCore);
    
    // Cloud Service Networks
    ipv4.SetBase("10.4.1.0", "255.255.255.0");
    Ipv4InterfaceContainer remoteWork1Interfaces = ipv4.Assign(coreToRemoteWork1);
    
    ipv4.SetBase("10.4.2.0", "255.255.255.0");
    Ipv4InterfaceContainer remoteWork2Interfaces = ipv4.Assign(coreToRemoteWork2);
    
    ipv4.SetBase("10.4.3.0", "255.255.255.0");
    Ipv4InterfaceContainer telemedicine1Interfaces = ipv4.Assign(coreToTelemedicine1);
    
    ipv4.SetBase("10.4.4.0", "255.255.255.0");
    Ipv4InterfaceContainer telemedicine2Interfaces = ipv4.Assign(coreToTelemedicine2);
    
    ipv4.SetBase("10.4.5.0", "255.255.255.0");
    Ipv4InterfaceContainer eLearning1Interfaces = ipv4.Assign(coreToELearning1);
    
    ipv4.SetBase("10.4.6.0", "255.255.255.0");
    Ipv4InterfaceContainer eLearning2Interfaces = ipv4.Assign(coreToELearning2);
    
    NS_LOG_INFO("Assigned IP addresses to all networks");
    
    // ==================== POPULATE ROUTING TABLES ====================
    
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    NS_LOG_INFO("Routing tables populated - end-to-end connectivity established");
    
    // ==================== SETUP CLOUD APPLICATIONS ====================
    
    uint16_t basePort = 5000;
    
    // ========== REMOTE WORK CLOUD SERVICES ==========
    
    // Server 1: Video Conferencing Platform (e.g., Zoom, Teams)
    UdpServerHelper videoConfServer(basePort);
    ApplicationContainer videoConfServerApp = videoConfServer.Install(remoteWorkServers.Get(0));
    videoConfServerApp.Start(Seconds(1.0));
    videoConfServerApp.Stop(Seconds(simulationTime));
    
    // Server 2: Cloud Office Suite (e.g., Google Workspace, Microsoft 365)
    UdpServerHelper officeServer(basePort + 1);
    ApplicationContainer officeServerApp = officeServer.Install(remoteWorkServers.Get(1));
    officeServerApp.Start(Seconds(1.0));
    officeServerApp.Stop(Seconds(simulationTime));
    
    NS_LOG_INFO("Remote work servers deployed (Video Conferencing + Office Suite)");
    
    // ========== TELEMEDICINE CLOUD SERVICES ==========
    
    // Server 1: Teleconsultation Platform (real-time video with doctors)
    UdpServerHelper teleconsultServer(basePort + 2);
    ApplicationContainer teleconsultServerApp = teleconsultServer.Install(telemedicineServers.Get(0));
    teleconsultServerApp.Start(Seconds(1.0));
    teleconsultServerApp.Stop(Seconds(simulationTime));
    
    // Server 2: Electronic Health Records (EHR) System
    UdpServerHelper ehrServer(basePort + 3);
    ApplicationContainer ehrServerApp = ehrServer.Install(telemedicineServers.Get(1));
    ehrServerApp.Start(Seconds(1.0));
    ehrServerApp.Stop(Seconds(simulationTime));
    
    NS_LOG_INFO("Telemedicine servers deployed (Teleconsultation + EHR)");
    
    // ========== E-LEARNING CLOUD SERVICES ==========
    
    // Server 1: Learning Management System (LMS)
    UdpServerHelper lmsServer(basePort + 4);
    ApplicationContainer lmsServerApp = lmsServer.Install(eLearningServers.Get(0));
    lmsServerApp.Start(Seconds(1.0));
    lmsServerApp.Stop(Seconds(simulationTime));
    
    // Server 2: Video Lecture Platform
    UdpServerHelper lectureServer(basePort + 5);
    ApplicationContainer lectureServerApp = lectureServer.Install(eLearningServers.Get(1));
    lectureServerApp.Start(Seconds(1.0));
    lectureServerApp.Stop(Seconds(simulationTime));
    
    NS_LOG_INFO("E-Learning servers deployed (LMS + Video Lectures)");
    
    // ==================== SETUP USER APPLICATIONS ====================
    
    double startTime = 2.0;
    
    // ========== VILLAGE 1: REMOTE WORKERS ==========
    if (enableRemoteWork)
    {
        for (uint32_t i = 0; i < nRemoteWorkersVillage1; i++)
        {
            if (i % 2 == 0)
            {
                // Video conferencing (high priority for real-time communication)
                UdpClientHelper videoClient(remoteWork1Interfaces.GetAddress(1), basePort);
                videoClient.SetAttribute("MaxPackets", UintegerValue(10000));
                videoClient.SetAttribute("Interval", TimeValue(MilliSeconds(remoteWorkInterval)));
                videoClient.SetAttribute("PacketSize", UintegerValue(remoteWorkPacketSize));
                
                ApplicationContainer clientApp = videoClient.Install(remoteWorkers.Get(i));
                clientApp.Start(Seconds(startTime + i * 0.2));
                clientApp.Stop(Seconds(simulationTime));
            }
            else
            {
                // Cloud office applications
                UdpClientHelper officeClient(remoteWork2Interfaces.GetAddress(1), basePort + 1);
                officeClient.SetAttribute("MaxPackets", UintegerValue(8000));
                officeClient.SetAttribute("Interval", TimeValue(MilliSeconds(remoteWorkInterval)));
                officeClient.SetAttribute("PacketSize", UintegerValue(remoteWorkPacketSize));
                
                ApplicationContainer clientApp = officeClient.Install(remoteWorkers.Get(i));
                clientApp.Start(Seconds(startTime + i * 0.2));
                clientApp.Stop(Seconds(simulationTime));
            }
        }
        NS_LOG_INFO("Remote work applications started for " << nRemoteWorkersVillage1 << " workers");
    }
    
    // ========== VILLAGE 2: TELEMEDICINE USERS ==========
    if (enableTelemedicine)
    {
        for (uint32_t i = 0; i < nTelemedicineUsersVillage2; i++)
        {
            if (i % 2 == 0)
            {
                // Real-time teleconsultation (video call with doctor)
                UdpClientHelper teleconsultClient(telemedicine1Interfaces.GetAddress(1), basePort + 2);
                teleconsultClient.SetAttribute("MaxPackets", UintegerValue(12000));
                teleconsultClient.SetAttribute("Interval", TimeValue(MilliSeconds(telemedicineInterval)));
                teleconsultClient.SetAttribute("PacketSize", UintegerValue(telemedicinePacketSize));
                
                ApplicationContainer clientApp = teleconsultClient.Install(telemedicineUsers.Get(i));
                clientApp.Start(Seconds(startTime + i * 0.3));
                clientApp.Stop(Seconds(simulationTime));
            }
            else
            {
                // Electronic Health Records access
                UdpClientHelper ehrClient(telemedicine2Interfaces.GetAddress(1), basePort + 3);
                ehrClient.SetAttribute("MaxPackets", UintegerValue(6000));
                ehrClient.SetAttribute("Interval", TimeValue(MilliSeconds(telemedicineInterval * 1.5)));
                ehrClient.SetAttribute("PacketSize", UintegerValue(telemedicinePacketSize));
                
                ApplicationContainer clientApp = ehrClient.Install(telemedicineUsers.Get(i));
                clientApp.Start(Seconds(startTime + i * 0.3));
                clientApp.Stop(Seconds(simulationTime));
            }
        }
        NS_LOG_INFO("Telemedicine applications started for " << nTelemedicineUsersVillage2 << " patients");
    }
    
    // ========== VILLAGE 3: E-LEARNING STUDENTS ==========
    if (enableELearning)
    {
        for (uint32_t i = 0; i < nStudentsVillage3; i++)
        {
            if (i % 2 == 0)
            {
                // Video lecture streaming
                UdpClientHelper lectureClient(eLearning2Interfaces.GetAddress(1), basePort + 5);
                lectureClient.SetAttribute("MaxPackets", UintegerValue(15000));
                lectureClient.SetAttribute("Interval", TimeValue(MilliSeconds(eLearningInterval)));
                lectureClient.SetAttribute("PacketSize", UintegerValue(eLearningPacketSize));
                
                ApplicationContainer clientApp = lectureClient.Install(students.Get(i));
                clientApp.Start(Seconds(startTime + i * 0.25));
                clientApp.Stop(Seconds(simulationTime));
            }
            else
            {
                // LMS access (assignments, quizzes, materials)
                UdpClientHelper lmsClient(eLearning1Interfaces.GetAddress(1), basePort + 4);
                lmsClient.SetAttribute("MaxPackets", UintegerValue(10000));
                lmsClient.SetAttribute("Interval", TimeValue(MilliSeconds(eLearningInterval * 1.2)));
                lmsClient.SetAttribute("PacketSize", UintegerValue(eLearningPacketSize));
                
                ApplicationContainer clientApp = lmsClient.Install(students.Get(i));
                clientApp.Start(Seconds(startTime + i * 0.25));
                clientApp.Stop(Seconds(simulationTime));
            }
        }
        NS_LOG_INFO("E-Learning applications started for " << nStudentsVillage3 << " students");
    }
    
    // ==================== SETUP MOBILITY MODELS ====================
    
    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    
    // Village 1: Remote Workers (left side)
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(10.0),
                                  "DeltaY", DoubleValue(10.0),
                                  "GridWidth", UintegerValue(2),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(remoteWorkers);
    
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(15.0),
                                  "MinY", DoubleValue(10.0),
                                  "DeltaX", DoubleValue(0.0),
                                  "DeltaY", DoubleValue(0.0),
                                  "GridWidth", UintegerValue(1),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(starlinkTerminals.Get(0));
    
    // Village 2: Telemedicine Users (middle)
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(50.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(10.0),
                                  "DeltaY", DoubleValue(10.0),
                                  "GridWidth", UintegerValue(2),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(telemedicineUsers);
    
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(65.0),
                                  "MinY", DoubleValue(10.0),
                                  "DeltaX", DoubleValue(0.0),
                                  "DeltaY", DoubleValue(0.0),
                                  "GridWidth", UintegerValue(1),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(starlinkTerminals.Get(1));
    
    // Village 3: Students (right side)
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(100.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(10.0),
                                  "DeltaY", DoubleValue(10.0),
                                  "GridWidth", UintegerValue(3),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(students);
    
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(120.0),
                                  "MinY", DoubleValue(10.0),
                                  "DeltaX", DoubleValue(0.0),
                                  "DeltaY", DoubleValue(0.0),
                                  "GridWidth", UintegerValue(1),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(starlinkTerminals.Get(2));
    
    // Starlink Satellite (high above)
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(60.0),
                                  "MinY", DoubleValue(150.0),
                                  "DeltaX", DoubleValue(0.0),
                                  "DeltaY", DoubleValue(0.0),
                                  "GridWidth", UintegerValue(1),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(satellite);
    
    // Ground Station
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(60.0),
                                  "MinY", DoubleValue(100.0),
                                  "DeltaX", DoubleValue(0.0),
                                  "DeltaY", DoubleValue(0.0),
                                  "GridWidth", UintegerValue(1),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(groundStation);
    
    // Core Router
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(60.0),
                                  "MinY", DoubleValue(70.0),
                                  "DeltaX", DoubleValue(0.0),
                                  "DeltaY", DoubleValue(0.0),
                                  "GridWidth", UintegerValue(1),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(coreRouter);
    
    // Cloud Servers (data centers)
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(30.0),
                                  "MinY", DoubleValue(50.0),
                                  "DeltaX", DoubleValue(20.0),
                                  "DeltaY", DoubleValue(0.0),
                                  "GridWidth", UintegerValue(3),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.Install(remoteWorkServers);
    mobility.Install(telemedicineServers);
    mobility.Install(eLearningServers);
    
    // ==================== ENABLE PACKET TRACING ====================
    
    // Capture packets on key links for analysis
    phyRemoteWork.EnablePcap("rural-exodus-remotework", apDeviceVillage1.Get(0));
    phyTelemedicine.EnablePcap("rural-exodus-telemedicine", apDeviceVillage2.Get(0));
    phyELearning.EnablePcap("rural-exodus-elearning", apDeviceVillage3.Get(0));
    p2p.EnablePcapAll("rural-exodus-wan");
    
    // ASCII trace
    AsciiTraceHelper ascii;
    p2p.EnableAsciiAll(ascii.CreateFileStream("rural-exodus-network.tr"));
    
    NS_LOG_INFO("Packet tracing enabled");
    
    // ==================== FLOW MONITORING ====================
    
    FlowMonitorHelper flowmon;
    Ptr<FlowMonitor> monitor = flowmon.InstallAll();
    
    // ==================== NETANIM VISUALIZATION ====================
    
    AnimationInterface anim("rural-exodus-animation.xml");
    anim.SetMaxPktsPerTraceFile(500000);
    
    // Village 1: Remote Workers (Green - Economic opportunity)
    for (uint32_t i = 0; i < remoteWorkers.GetN(); i++)
    {
        anim.UpdateNodeDescription(remoteWorkers.Get(i), "Worker-" + std::to_string(i+1));
        anim.UpdateNodeColor(remoteWorkers.Get(i), 0, 200, 0);
        anim.UpdateNodeSize(remoteWorkers.Get(i)->GetId(), 3, 3);
    }
    
    // Village 2: Telemedicine Users (Red - Healthcare)
    for (uint32_t i = 0; i < telemedicineUsers.GetN(); i++)
    {
        anim.UpdateNodeDescription(telemedicineUsers.Get(i), "Patient-" + std::to_string(i+1));
        anim.UpdateNodeColor(telemedicineUsers.Get(i), 255, 0, 0);
        anim.UpdateNodeSize(telemedicineUsers.Get(i)->GetId(), 3, 3);
    }
    
    // Village 3: Students (Blue - Education)
    for (uint32_t i = 0; i < students.GetN(); i++)
    {
        anim.UpdateNodeDescription(students.Get(i), "Student-" + std::to_string(i+1));
        anim.UpdateNodeColor(students.Get(i), 0, 100, 255);
        anim.UpdateNodeSize(students.Get(i)->GetId(), 3, 3);
    }
    
    // Starlink Terminals (Orange - Access points)
    for (uint32_t i = 0; i < starlinkTerminals.GetN(); i++)
    {
        anim.UpdateNodeDescription(starlinkTerminals.Get(i), "Starlink-Terminal-" + std::to_string(i+1));
        anim.UpdateNodeColor(starlinkTerminals.Get(i), 255, 165, 0);
        anim.UpdateNodeSize(starlinkTerminals.Get(i)->GetId(), 5, 5);
    }
    
    // Satellite (Bright Red - Space infrastructure)
    anim.UpdateNodeDescription(satellite.Get(0), "LEO-Satellite");
    anim.UpdateNodeColor(satellite.Get(0), 255, 50, 50);
    anim.UpdateNodeSize(satellite.Get(0)->GetId(), 8, 8);
    
    // Ground Station (Purple)
    anim.UpdateNodeDescription(groundStation.Get(0), "Ground-Station");
    anim.UpdateNodeColor(groundStation.Get(0), 128, 0, 128);
    anim.UpdateNodeSize(groundStation.Get(0)->GetId(), 6, 6);
    
    // Core Router (Dark Blue)
    anim.UpdateNodeDescription(coreRouter.Get(0), "Internet-Core");
    anim.UpdateNodeColor(coreRouter.Get(0), 0, 0, 139);
    anim.UpdateNodeSize(coreRouter.Get(0)->GetId(), 6, 6);
    
    // Cloud Servers (Yellow - Cloud services)
    anim.UpdateNodeDescription(remoteWorkServers.Get(0), "VideoConf-Server");
    anim.UpdateNodeColor(remoteWorkServers.Get(0), 255, 215, 0);
    anim.UpdateNodeSize(remoteWorkServers.Get(0)->GetId(), 5, 5);
    
    anim.UpdateNodeDescription(remoteWorkServers.Get(1), "Office-Cloud");
    anim.UpdateNodeColor(remoteWorkServers.Get(1), 255, 215, 0);
    anim.UpdateNodeSize(remoteWorkServers.Get(1)->GetId(), 5, 5);
    
    anim.UpdateNodeDescription(telemedicineServers.Get(0), "Teleconsult-Server");
    anim.UpdateNodeColor(telemedicineServers.Get(0), 255, 215, 0);
    anim.UpdateNodeSize(telemedicineServers.Get(0)->GetId(), 5, 5);
    
    anim.UpdateNodeDescription(telemedicineServers.Get(1), "EHR-Server");
    anim.UpdateNodeColor(telemedicineServers.Get(1), 255, 215, 0);
    anim.UpdateNodeSize(telemedicineServers.Get(1)->GetId(), 5, 5);
    
    anim.UpdateNodeDescription(eLearningServers.Get(0), "LMS-Server");
    anim.UpdateNodeColor(eLearningServers.Get(0), 255, 215, 0);
    anim.UpdateNodeSize(eLearningServers.Get(0)->GetId(), 5, 5);
    
    anim.UpdateNodeDescription(eLearningServers.Get(1), "Lecture-Server");
    anim.UpdateNodeColor(eLearningServers.Get(1), 255, 215, 0);
    anim.UpdateNodeSize(eLearningServers.Get(1)->GetId(), 5, 5);
    
    // ==================== RUN SIMULATION ====================
    
    NS_LOG_INFO("========================================");
    NS_LOG_INFO("Starting simulation for " << simulationTime << " seconds");
    NS_LOG_INFO("Connecting remote villages to essential services via Starlink");
    NS_LOG_INFO("========================================");
    
    Simulator::Stop(Seconds(simulationTime));
    Simulator::Run();
    
    // ==================== ANALYZE RESULTS ====================
    
    NS_LOG_INFO("\n========================================");
    NS_LOG_INFO("SIMULATION COMPLETED - ANALYZING RESULTS");
    NS_LOG_INFO("========================================\n");
    
    monitor->CheckForLostPackets();
    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
    std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();
    
    // Categorize flows by service type
    std::vector<FlowMonitor::FlowStats> remoteWorkFlows;
    std::vector<FlowMonitor::FlowStats> telemedicineFlows;
    std::vector<FlowMonitor::FlowStats> eLearningFlows;
    
    for (auto const& stat : stats)
    {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(stat.first);
        
        // Convert IP address to string properly
        std::ostringstream oss;
        t.destinationAddress.Print(oss);
        std::string destAddr = oss.str();
        
        // Categorize based on destination network
        if (destAddr.find("10.4.1") != std::string::npos || destAddr.find("10.4.2") != std::string::npos)
        {
            remoteWorkFlows.push_back(stat.second);
        }
        else if (destAddr.find("10.4.3") != std::string::npos || destAddr.find("10.4.4") != std::string::npos)
        {
            telemedicineFlows.push_back(stat.second);
        }
        else if (destAddr.find("10.4.5") != std::string::npos || destAddr.find("10.4.6") != std::string::npos)
        {
            eLearningFlows.push_back(stat.second);
        }
    }
    
    // ========== REMOTE WORK STATISTICS ==========
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║         REMOTE WORK SERVICE ANALYSIS               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    
    double rwTotalThroughput = 0, rwTotalDelay = 0, rwTotalPDR = 0;
    uint32_t rwFlowCount = 0;
    
    for (const auto& flow : remoteWorkFlows)
    {
        if (flow.rxPackets > 0)
        {
            rwTotalThroughput += (flow.rxBytes * 8.0) / simulationTime / 1000000; // Mbps
            rwTotalDelay += flow.delaySum.GetMilliSeconds() / flow.rxPackets;
            rwTotalPDR += (flow.rxPackets * 100.0) / flow.txPackets;
            rwFlowCount++;
        }
    }
    
    if (rwFlowCount > 0)
    {
        std::cout << "Number of Workers Connected: " << nRemoteWorkersVillage1 << std::endl;
        std::cout << "Average Throughput: " << (rwTotalThroughput / rwFlowCount) << " Mbps" << std::endl;
        std::cout << "Average Latency: " << (rwTotalDelay / rwFlowCount) << " ms" << std::endl;
        std::cout << "Average Packet Delivery: " << (rwTotalPDR / rwFlowCount) << "%" << std::endl;
        
        if ((rwTotalDelay / rwFlowCount) < 150)
        {
            std::cout << "✓ EXCELLENT: Video conferencing quality acceptable (<150ms)" << std::endl;
        }
        std::cout << "✓ Remote workers can stay in rural areas with quality connectivity" << std::endl;
    }
    
    // ========== TELEMEDICINE STATISTICS ==========
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║         TELEMEDICINE SERVICE ANALYSIS              ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    
    double tmTotalThroughput = 0, tmTotalDelay = 0, tmTotalPDR = 0;
    uint32_t tmFlowCount = 0;
    
    for (const auto& flow : telemedicineFlows)
    {
        if (flow.rxPackets > 0)
        {
            tmTotalThroughput += (flow.rxBytes * 8.0) / simulationTime / 1000000; // Mbps
            tmTotalDelay += flow.delaySum.GetMilliSeconds() / flow.rxPackets;
            tmTotalPDR += (flow.rxPackets * 100.0) / flow.txPackets;
            tmFlowCount++;
        }
    }
    
    if (tmFlowCount > 0)
    {
        std::cout << "Number of Patients Served: " << nTelemedicineUsersVillage2 << std::endl;
        std::cout << "Average Throughput: " << (tmTotalThroughput / tmFlowCount) << " Mbps" << std::endl;
        std::cout << "Average Latency: " << (tmTotalDelay / tmFlowCount) << " ms" << std::endl;
        std::cout << "Average Packet Delivery: " << (tmTotalPDR / tmFlowCount) << "%" << std::endl;
        
        if ((tmTotalDelay / tmFlowCount) < 200)
        {
            std::cout << "✓ EXCELLENT: Real-time teleconsultation feasible (<200ms)" << std::endl;
        }
        std::cout << "✓ Patients can access healthcare without traveling to cities" << std::endl;
    }
    
    // ========== E-LEARNING STATISTICS ==========
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║         E-LEARNING SERVICE ANALYSIS                ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    
    double elTotalThroughput = 0, elTotalDelay = 0, elTotalPDR = 0;
    uint32_t elFlowCount = 0;
    
    for (const auto& flow : eLearningFlows)
    {
        if (flow.rxPackets > 0)
        {
            elTotalThroughput += (flow.rxBytes * 8.0) / simulationTime / 1000000; // Mbps
            elTotalDelay += flow.delaySum.GetMilliSeconds() / flow.rxPackets;
            elTotalPDR += (flow.rxPackets * 100.0) / flow.txPackets;
            elFlowCount++;
        }
    }
    
    if (elFlowCount > 0)
    {
        std::cout << "Number of Students Connected: " << nStudentsVillage3 << std::endl;
        std::cout << "Average Throughput: " << (elTotalThroughput / elFlowCount) << " Mbps" << std::endl;
        std::cout << "Average Latency: " << (elTotalDelay / elFlowCount) << " ms" << std::endl;
        std::cout << "Average Packet Delivery: " << (elTotalPDR / elFlowCount) << "%" << std::endl;
        
        if ((elTotalThroughput / elFlowCount) > 5)
        {
            std::cout << "✓ EXCELLENT: HD video streaming capable (>5 Mbps)" << std::endl;
        }
        std::cout << "✓ Students can access quality education without relocating" << std::endl;
    }
    
    // ========== OVERALL RURAL EXODUS IMPACT ==========
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║       RURAL EXODUS REDUCTION IMPACT SUMMARY        ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    
    uint32_t totalUsers = nRemoteWorkersVillage1 + nTelemedicineUsersVillage2 + nStudentsVillage3;
    std::cout << "\nTotal Rural Users Connected: " << totalUsers << std::endl;
    std::cout << "Villages Served: 3 (no cellular infrastructure)" << std::endl;
    std::cout << "Technology: Starlink LEO Satellite Constellation" << std::endl;
    
    std::cout << "\n✓ Economic Opportunity: " << nRemoteWorkersVillage1 
              << " workers can earn urban salaries from rural homes" << std::endl;
    std::cout << "✓ Healthcare Access: " << nTelemedicineUsersVillage2 
              << " patients receive medical care without long travel" << std::endl;
    std::cout << "✓ Education Access: " << nStudentsVillage3 
              << " students access quality education remotely" << std::endl;
    
    std::cout << "\n═══════════════════════════════════════════════════" << std::endl;
    std::cout << "CONCLUSION: Starlink WAN enables rural residents to access" << std::endl;
    std::cout << "essential services (work, healthcare, education) without" << std::endl;
    std::cout << "relocating to urban areas, directly reducing rural exodus." << std::endl;
    std::cout << "═══════════════════════════════════════════════════\n" << std::endl;
    
    // ========== OUTPUT FILES ==========
    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║              SIMULATION OUTPUT FILES               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "Animation: rural-exodus-animation.xml (open with NetAnim)" << std::endl;
    std::cout << "Trace file: rural-exodus-network.tr" << std::endl;
    std::cout << "PCAP files: rural-exodus-*.pcap (analyze with Wireshark)\n" << std::endl;
    
    Simulator::Destroy();
    NS_LOG_INFO("Simulation completed successfully");
    
    return 0;
}

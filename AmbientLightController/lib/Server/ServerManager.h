#pragma once
#include <Arduino.h>
#include <ETH.h>
#include "AsyncUDP.h"
#include <vector>
#include <LogManager.hpp>

struct ClientInfo
{
    IPAddress ip;
    unsigned long lastSeen;
};

class ServerManager
{
public:
    static void init();
    static void tick();

private:
    static NetworkServer server;
    static AsyncUDP udp;
    static const uint16_t listenPort;
    static const size_t bufferSize;
    static char buffer[64];

    static std::vector<ClientInfo> clients;
    static std::vector<NetworkClient> connectedClients;

    static void handlePacket();
    static void registerClient(IPAddress ip);
};

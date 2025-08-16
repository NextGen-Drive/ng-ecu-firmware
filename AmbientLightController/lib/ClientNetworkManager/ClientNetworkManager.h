#pragma once
#include <Arduino.h>
#include <ETH.h>
#include "AsyncUDP.h"
#include "LogManager.hpp"

/**
 * Client-side communication
 */
class ClientNetworkManager
{
public:
    static void init(uint8_t id);
    static void tick(); // optional, z.B. für Heartbeat später

private:
    static void sendLoginPing();

    static AsyncUDP udp;
    static const IPAddress serverIP;
    static const uint16_t serverPort;
    static const char *pingMessage;
    static bool isInitialized;
};

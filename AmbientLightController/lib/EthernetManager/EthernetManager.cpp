#include "EthernetManager.h"

#define ETH_MISO_PIN 12
#define ETH_MOSI_PIN 11
#define ETH_SCLK_PIN 13
#define ETH_CS_PIN 14
#define ETH_INT_PIN 10
#define ETH_RST_PIN 9
#define ETH_ADDR 2

String EthernetManager::LogPrefix = "[EthernetManager] ";

void EthernetManager::init(uint8_t id)
{
    Log::println(LogPrefix + "Initializing...");

    IPAddress local_ip(192, 168, 0, id);
    IPAddress gateway(192, 168, 0, 1);
    IPAddress subnet(255, 255, 255, 0);
    ETH.config(local_ip, gateway, subnet);

    if (!ETH.begin(ETH_PHY_W5500, ETH_ADDR, ETH_CS_PIN, ETH_INT_PIN, ETH_RST_PIN,
                   SPI3_HOST,
                   ETH_SCLK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN))
    {
        Log::println(LogPrefix + " ETH.begin() failed");
    }

    ETH.config(local_ip, gateway, subnet);

    unsigned long startMillis = millis();
    const unsigned long timeout = 10000;

    while (!ETH.linkUp() || ETH.localIP() == IPAddress(0, 0, 0, 0))
    {
        if (millis() - startMillis > timeout)
        {
            Log::println("[NetworkManager] Timeout trying to establish connection");
            return;
        }

        delay(100);
    }

    Log::println(LogPrefix + "[ETH] Connected, IP: " + ETH.localIP().toString());
}
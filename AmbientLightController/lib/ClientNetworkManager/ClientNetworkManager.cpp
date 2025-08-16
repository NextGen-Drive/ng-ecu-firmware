#include "ClientNetworkManager.h"

AsyncUDP ClientNetworkManager::udp;
const IPAddress ClientNetworkManager::serverIP(192, 168, 0, 1);
const uint16_t ClientNetworkManager::serverPort = 4210;
const char *ClientNetworkManager::pingMessage = "hello_from_client";
bool ClientNetworkManager::isInitialized = false;
NetworkClient client;

void ClientNetworkManager::init(uint8_t id)
{
    isInitialized = true;
    Serial.println("[Client] Init abgeschlossen. ID: " + String(id));
    
    if (!client.connect(serverIP, serverPort)) {
        Serial.println("[Client] Verbindung fehlgeschlagen.");
    } else {
        Serial.println("[Client] Dauerhafte Verbindung aufgebaut.");
    }
}

void ClientNetworkManager::tick()
{
    sendLoginPing();
    delay(2000);
}

void ClientNetworkManager::sendLoginPing()
{
    if (!client.connected()) {
        Serial.println("[Client] Verbindung verloren, reconnect...");
        client.connect(serverIP, serverPort);
        return;
    }

    unsigned long startTime = millis();
    client.println(pingMessage);

    unsigned long timeout = startTime + 500;
    while (client.connected() && millis() < timeout)
    {
        if (client.available())
        {
            String response = client.readStringUntil('\n');
            unsigned long latency = millis() - startTime;
            Serial.println("[Client] Antwort: " + response);
            Serial.println("[Client] Latenz: " + String(latency) + " ms");
            break;
        }
    }
}
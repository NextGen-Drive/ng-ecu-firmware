#include "ServerManager.h"

const uint16_t ServerManager::listenPort = 4210;
NetworkServer ServerManager::server(ServerManager::listenPort);
const size_t ServerManager::bufferSize = 64;
char ServerManager::buffer[64];
std::vector<ClientInfo> ServerManager::clients;
std::vector<NetworkClient> ServerManager::connectedClients;

String LogPrefix = "[ServerManager] ";

void ServerManager::init()
{
    Log::println(LogPrefix + "Initializing...");
    Log::println(LogPrefix + "TCP Port: " + String(listenPort));

    server.begin();
    server.setNoDelay(true);
    Log::println(LogPrefix + "TCP Server gestartet");
}

void ServerManager::tick()
{
    handlePacket();
}

void ServerManager::handlePacket()
{
    // Neue Verbindungen annehmen
    NetworkClient newClient = server.accept();
    if (newClient)
    {
        Log::println(LogPrefix + "Neuer Client verbunden: " + newClient.remoteIP().toString());
        newClient.setNoDelay(true);
        connectedClients.push_back(newClient);
    }

    // Bestehende Clients durchgehen
    for (auto it = connectedClients.begin(); it != connectedClients.end();)
    {
        if (!it->connected())
        {
            Log::println(LogPrefix + "Client getrennt: " + it->remoteIP().toString());
            it = connectedClients.erase(it); // Entfernen & Iterator aktualisieren
            continue;
        }

        // Daten lesen
        if (it->available())
        {
            size_t len = it->readBytesUntil('\n', buffer, bufferSize - 1);
            buffer[len] = '\0';

            Log::println("[Server] Nachricht von " + it->remoteIP().toString() + ": " + String(buffer));

            if (strcmp(buffer, "hello_from_client") == 0)
            {
                registerClient(it->remoteIP());
                it->println("pong"); // Sofortige Antwort für Latenz-Messung
            }
            else
            {
                it->println("Unbekannter Befehl");
            }
        }

        ++it; // Weiter zum nächsten Client
    }
}

void ServerManager::registerClient(IPAddress ip)
{
    for (auto &client : clients)
    {
        if (client.ip == ip)
        {
            client.lastSeen = millis();
            Log::println(LogPrefix + "Client erneut gesehen: " + ip.toString());
            return;
        }
    }

    clients.push_back({ip, millis()});
    Log::println(LogPrefix + "Neuer Client registriert: " + ip.toString());
}

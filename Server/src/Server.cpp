#include "Server.hpp"

namespace xal
{

Server::Server()
{
    unsigned short port;
    std::string movie;

#ifdef DEBUG
    std::ifstream file("debug.txt");
    file >> port >> movie >> m_movieLength;
    file.close();

    std::cout << port << "\n";
#endif

    std::cout << sf::IpAddress::getLocalAddress() << "\n";

    // Create a socket to listen to new connections
    m_listener.listen(port);

    // Add the m_listener to the m_selector
    m_selector.add(m_listener);
}

void Server::run()
{
    while (true)
    {
        // Make the m_selector wait for data on any socket
        if (m_selector.wait())
        {
            // Test the m_listener
            if (m_selector.isReady(m_listener))
            {
                // The m_listener is ready: there is a pending connection
                sf::TcpSocket* client = new sf::TcpSocket;
                if (m_listener.accept(*client) == sf::Socket::Done)
                {
                    // Add the new client to the m_clients list
                    m_clients.push_back({client, ""});
                    // Add the new client to the m_selector so that we will
                    // be notified when he sends something

                    std::cout << "[CONNECTIONS]New connection from " << client->getRemoteAddress().toString() << ":" << client->getRemotePort() << "\n";

                    m_selector.add(*client);
                }
                else
                {
                    // Error, we won't get a new connection, delete the socket
                    delete client;
                }
            }
            else
            {
                // The m_listener socket is not ready, test all other sockets (the m_clients)
                for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
                {
                    sf::TcpSocket& client = *(it->first);
                    if (m_selector.isReady(client))
                    {
                        // The client has sent some data, we can receive it
                        sf::Packet packet;
                        if (client.receive(packet) == sf::Socket::Done)
                        {
                            int result = executeCommand(packet, client, it->second);
                            if (result == 1)
                            {
                                it = m_clients.erase(it);
                                if (it == m_clients.end())
                                    break;
                            }
                            else if (result == 2)
                            {
                                std::string username;
                                packet >> username;

                                it->second = username;

                                sendPacketToAllExcepts(sf::Packet() << "NEW USER" << username, username);
                            }
                        }
                    }
                }
            }
        }
    }
}

int Server::executeCommand(sf::Packet& packet, sf::TcpSocket& client, const std::string& username)
{
    std::string command;
    packet >> command;

    if (command == "CHECKMOVIE")
    {
        unsigned int movieLength;
        packet >> movieLength;

        std::cout << "[PACKET][CHECKMOVIE]movieLength:" << movieLength << "\n";

        packet.clear();
        if (m_movieLength == movieLength) packet << "GOOD";
        else packet << "BAD";

        sendPacket(packet, client);
    }
    else if (command == "PLAY")
    {
        if (checkAllReady())
            sendPacketToAll(sf::Packet() << "PLAY" << username);
        else
            sendPacket(sf::Packet() << "MESSAGE" << "Not all users are ready", client);
    }
    else if (command == "PAUSE")
    {
        sendPacketToAll(sf::Packet() << "PAUSE" << username);
    }
    else if (command == "USERNAME")
    {
        return 2;
    }
    else if (command == "DISCONNECT")
    {
        return 1;
    }

    return 0;
}

void Server::sendPacket(sf::Packet &packet, sf::TcpSocket& client)
{
    if (client.send(packet) == sf::Socket::Done)
    {
        std::cout << "[CLIENT]Packet sent to " << client.getRemoteAddress().toString() << ":" << client.getRemotePort() << "\n";
    }
    else
    {
        std::cout << "[EROARE]Can't sent packet!\n";
    }
}

void Server::sendPacketToAll(sf::Packet &packet)
{
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        sf::TcpSocket& client = *(it->first);
        sendPacket(packet, client);
    }
}

void Server::sendPacketToAllExcepts(sf::Packet &packet, std::string usernameToIgnore)
{
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        if (it->second != usernameToIgnore)
        {
            sf::TcpSocket& client = *(it->first);
            sendPacket(packet, client);
        }
    }
}

bool Server::checkAllReady()
{
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        if (it->second.empty()) return false;
    }

    return true;
}

}

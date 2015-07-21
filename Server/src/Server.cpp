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

    std::cout << sf::IpAddress::getPublicAddress() << "\n";

    // Create a socket to listen to new connections
    m_listener.listen(port);

    // Add the m_listener to the m_selector
    m_selector.add(m_listener);

    m_listener.setBlocking(false);

    m_secondsUntilCheck = 120;
}

void Server::run()
{
    m_clock.restart();
    while (true)
    {
        // Make the m_selector wait for data on any socket
        if (m_selector.wait(sf::seconds(60)))
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
                                std::cout << "[CONNECTION]The user \"" << it->second << "\":" << it->first->getRemoteAddress() << ":" << it->first->getRemotePort() << " has disconnected\n";

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

        if (m_clock.getElapsedTime().asSeconds() > m_secondsUntilCheck)
        {
            m_checkTimes.clear();
            sendPacketToAll(sf::Packet() << "CHECKTIME");
            m_clock.restart();
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
    else if (command == "CHECKTIME")
    {
        int hour, min, sec;
        sf::Int32 movieTime;

        packet >> hour >> min >> sec >> movieTime;
        m_checkTimes.push_back({ {hour, min, sec}, movieTime, username });

        if (m_checkTimes.size() == m_clients.size())
        {
            checkTime();
        }
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
        auto errorType = client.send(packet);
        std::cout << "[ERROR]Can't sent packet! Error:" << errorType << "\n";
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

void Server::checkTime()
{
    sf::Int32 smallestTime = m_checkTimes.front().movieTime;
    for (std::size_t i=1; i<m_checkTimes.size(); ++i)
    {
        if (smallestTime > m_checkTimes[i].movieTime)
        {
            smallestTime = m_checkTimes[i].movieTime;
        }
    }

    for (std::size_t i=1; i<m_checkTimes.size(); ++i)
    {
        auto difference = m_checkTimes[i].movieTime - smallestTime;
        if (difference > 50)
        {
            sendPacket(sf::Packet() << "WAIT" << difference - 10, *findSocket(m_checkTimes[i].username));
        }
    }
}

sf::TcpSocket* Server::findSocket(const std::string& username)
{
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it)
    {
        if (it->second == username)
            return it->first;
    }
    return nullptr;
}

}

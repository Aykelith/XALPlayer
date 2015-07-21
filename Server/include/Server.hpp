#ifndef XAL_SERVER_HPP
#define XAL_SERVER_HPP

#include "Global.hpp"

#include <SFML/Network.hpp>
#include <iostream>
#include <list>
#include <utility>

#ifdef DEBUG
    #include <fstream>
#endif

namespace xal
{

class Server
{
public:
    Server();

    void run();
private:
    int executeCommand(sf::Packet& packet, sf::TcpSocket& client, const std::string& username);
    void sendPacket(sf::Packet &packet, sf::TcpSocket& client);
    void sendPacketToAll(sf::Packet &packet);
    void sendPacketToAllExcepts(sf::Packet &packet, std::string usernameToIgnore);

    bool checkAllReady();

    void checkTime();

    sf::TcpSocket* findSocket(const std::string& username);

private:
    sf::TcpListener m_listener;
    std::list<std::pair<sf::TcpSocket*, std::string>> m_clients;
    sf::SocketSelector m_selector;

    unsigned int m_movieLength;

    sf::Clock m_clock;
    unsigned int m_secondsUntilCheck;
    struct ActualTime { int hour; int min; int sec; };
    struct CheckTime { ActualTime actualTime; sf::Int32 movieTime; std::string username; };
    std::vector<CheckTime> m_checkTimes;

};

}

#endif

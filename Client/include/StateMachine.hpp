#ifndef XAL_StateMachine_HPP
#define XAL_StateMachine_HPP

#include "Global.hpp"

#include <memory>
#include <stack>

#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Clock.hpp>

namespace xal
{

class State;

class StateMachine
{
public:
	StateMachine();

	void run( std::unique_ptr<State> state );

	void update();
	void draw();

	void nextState();
	void lastState();

	void quit();                                 // Exit app
	bool isRunning();                            // Check if app is running

	sf::TcpSocket& getSocket() { return m_socket; }
	void setMoviePath(const std::string& path) { m_moviePath = path; }
	std::string getMoviePath() const { return m_moviePath; }

	void setUsername(const std::string& username) { m_username = username; }
	std::string getUsername() const { return m_username; }

	int sendData(sf::Packet &packet, const sf::Time& waitTime)
	{
		sf::Clock clock;
		sf::Socket::Status status = m_socket.send(packet);
		while (clock.getElapsedTime() <= waitTime)
		{
			if (status == sf::Socket::Done)
				return 1;
			else if (status == sf::Socket::Disconnected)
				return 2;

			status = m_socket.send(packet);
		}

		return 0;
	}

	bool receiveData(sf::Packet &packet, const sf::Time& waitTime)
	{
		sf::Clock clock;
		sf::Socket::Status status = m_socket.receive(packet);
		while (clock.getElapsedTime() <= waitTime)
		{
			if (status == sf::Socket::Done)
				return 1;
			else if (status == sf::Socket::Disconnected)
				return 2;

			status = m_socket.receive(packet);
		}

		return 0;
	}

	// Build a new state
	template <typename T>
	static std::unique_ptr<T> build( StateMachine& machine, bool replace = true );

private:

private:
	bool m_running;
	bool m_resume;

	std::stack<std::unique_ptr<State> > m_states;

	sf::TcpSocket m_socket;
	std::string m_moviePath;
	std::string m_username;
};

template <typename T>
std::unique_ptr<T> StateMachine::build( StateMachine& machine, bool replace )
{
	return std::unique_ptr<T>( new T( machine, replace ) );
}

}

#endif

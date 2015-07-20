#ifndef XAL_STATE_HPP
#define XAL_STATE_HPP

#include "Global.hpp"

#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>

namespace xal
{

class StateMachine;

class State
{
public:
	State(StateMachine& machine, bool replace = true)
	: m_machine { machine }
	, m_timeSinceLastUpdate {sf::Time::Zero}
	, m_replace { replace }
	{}

	virtual ~State() = default;

	State ( const State& ) = delete;
	State& operator= ( const State& ) = delete;

	virtual void pause() = 0;
	virtual void resume() = 0;

	virtual void update() = 0;
	virtual void draw() = 0;

	bool isReplacing()
	{
		return m_replace;
	}

	std::unique_ptr<State> next()
	{
		return std::move( m_next);
	}

protected:
	StateMachine& m_machine;
    sf::RenderWindow m_window;

	sf::Clock m_clock;
	sf::Time  m_frame;
	sf::Time  m_timePerFrame;
	sf::Time  m_timeSinceLastUpdate;

	bool         m_replace;        // Verify if need to replace all windows
	std::unique_ptr<State> m_next; // Next state
};

}

#endif

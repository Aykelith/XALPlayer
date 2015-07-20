#include "StateMachine.hpp"
#include "State.hpp"

namespace xal
{

StateMachine::StateMachine()
: m_running (false)
, m_resume (false)
{
	m_socket.setBlocking(false);
}

void StateMachine::run(std::unique_ptr<State> state)
{
	// Prepare the StateMachine
	m_running = true;

	// Push first state
	m_states.push( std::move(state));
}

void StateMachine::update()
{
	// Let the state update the game
	m_states.top()->update();
}

void StateMachine::draw()
{
	// Let the state draw the screen
	m_states.top()->draw();
}

void StateMachine::nextState()
{
    if(m_resume)
    {
        // Cleanup the current state
        if ( !m_states.empty() )
        {
            m_states.pop();
        }

        // Resume previous state
        if ( !m_states.empty() )
        {
            m_states.top()->resume();
        }

        m_resume = false;
    }

    // There needs to be a state
    if ( !m_states.empty() )
    {
        std::unique_ptr<State> temp = m_states.top()->next();

        // Only change states if there's a next one existing
        if( temp != nullptr )
        {
            // Replace the running state
            if( temp->isReplacing() )
                m_states.pop();
            // Pause the running state
            else
                m_states.top()->pause();

			std::cout << "HERE\n";
            m_states.push( std::move( temp ) );
        }
    }
}

void StateMachine::lastState()
{
    m_resume = true;
}

void StateMachine::quit()
{
	sendData(sf::Packet() << "DISCONNECT", sf::seconds(5));
	m_running = false;
}

bool StateMachine::isRunning()
{
	return m_running;
}

}

#include "Client.hpp"

namespace xal
{

Client::Client()
{

}

void Client::run()
{
    m_machine.run( m_machine.build<SetupState>(m_machine, true));

    while (m_machine.isRunning())
    {
        m_machine.nextState();
        m_machine.update();
        m_machine.draw();
    }
}

}

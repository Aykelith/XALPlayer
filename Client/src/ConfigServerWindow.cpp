#include "ConfigServerWindow.hpp"

namespace xal
{

ConfigServerWindow::ConfigServerWindow()
{
    m_window.create({100,100}, "Configure server");
}

void ConfigServerWindow::update()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_window.close();
        }

        //m_gui.handleEvent(event);
    }
}

void ConfigServerWindow::draw()
{
    m_window.clear();

    m_window.display();
}

}

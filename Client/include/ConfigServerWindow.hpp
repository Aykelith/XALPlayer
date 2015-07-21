#ifndef XAL_CONFIGSERVERWINDOW_HPP
#define XAL_CONFIGSERVERWINDOW_HPP

#include "Global.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <TGUI/TGUI.hpp>

namespace xal
{

class ConfigServerWindow
{
public:
    ConfigServerWindow();

    void update();
    void draw();
private:

private:
    sf::RenderWindow m_window;
    tgui::Gui m_gui;
    
};

}

#endif

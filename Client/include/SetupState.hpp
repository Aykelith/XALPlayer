#ifndef XAL_SETUPSTATE_HPP
#define XAL_SETUPSTATE_HPP

#include "Global.hpp"

#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <TGUI/TGUI.hpp>
#include <sfeMovie/Movie.hpp>

#include "State.hpp"
#include "MovieState.hpp"

#include "ConfigServerWindow.hpp"

#ifdef DEBUG
    #include <fstream>
#endif

namespace xal
{

class StateMachine;

class SetupState: public State
{
public:
    SetupState(StateMachine& machine, bool replace = true);

    void pause();
	void resume();

	void update();
	void draw();

private:
    void showErrorMessage(const std::string& error);

private:
    tgui::Gui m_gui;

    ConfigServerWindow* m_configServerWindow;
};

}

#endif

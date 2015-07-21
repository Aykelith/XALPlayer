#ifndef XAL_MOVIESTATE_HPP
#define XAL_MOVIESTATE_HPP

#include "Global.hpp"

#include <SFML/Graphics.hpp>
#include <sfeMovie/Movie.hpp>
#include <TGUI/TGUI.hpp>
#include <ctime>
#include <cstdio>
#include <cstdlib>

#include "PopupMessageManager.hpp"

#include "State.hpp"

namespace xal
{

class StateMachine;

class MovieState: public State
{
public:
    MovieState(StateMachine& machine, bool replace = true);

    void pause();
	void resume();

	void update();
	void draw();

private:

private:
    tgui::Gui m_gui;
    sf::Font m_font;

    sfe::Movie m_movie;
    bool m_isPlaying;

    PopupMessageManager m_popupMessageManager;
};

}

#endif

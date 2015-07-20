#ifndef XAL_MOVIESTATE_HPP
#define XAL_MOVIESTATE_HPP

#include "Global.hpp"

#include <SFML/Graphics.hpp>
#include <sfeMovie/Movie.hpp>
#include <TGUI/TGUI.hpp>

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
    void showPopupMessage(const std::string& message);

private:
    tgui::Gui m_gui;
    sf::Font m_font;

    sfe::Movie m_movie;
    bool m_isPlaying;

    struct PopupMessage
    {
        sf::Text text;
        sf::Time elapsedTime;
        sf::Time maxTime;
    };
    PopupMessage* m_popupMessage;
};

}

#endif

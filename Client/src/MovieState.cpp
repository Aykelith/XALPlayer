#include "MovieState.hpp"
#include "StateMachine.hpp"

namespace xal
{

MovieState::MovieState(StateMachine& machine, bool replace)
: State {machine, replace}
, m_isPlaying {false}
{
    m_timePerFrame = sf::seconds(1.0f / 60.0f); // Number of frames per second

    m_gui.setGlobalFont("data/DejaVuSans.ttf");
    m_font.loadFromFile("data/DejaVuSans.ttf");

    m_window.create(sf::VideoMode::getFullscreenModes().at(0), m_machine.getMoviePath());

    if (!m_movie.openFromFile(m_machine.getMoviePath()))
    {
        // error
    }

    m_movie.fit(0, 0, m_window.getSize().x, m_window.getSize().y);

    m_machine.sendData(sf::Packet() << "USERNAME" << m_machine.getUsername(), sf::seconds(5));
}

void MovieState::pause()
{

}

void MovieState::resume()
{

}

void MovieState::update()
{
    m_frame = m_clock.restart();
	m_timeSinceLastUpdate += m_frame;

	while (m_timeSinceLastUpdate >= m_timePerFrame)
	{
        sf::Event event;
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                m_window.close();
                m_machine.quit();
            }
            else if (event.type == sf::Event::Resized)
            {
                m_movie.fit(0, 0, m_window.getSize().x, m_window.getSize().y);
                m_window.setView(sf::View(sf::FloatRect(0, 0, (float)m_window.getSize().x, (float)m_window.getSize().y)));
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space)
                {
                    if (m_isPlaying)
                    {
                        m_machine.sendData(sf::Packet() << "PAUSE", sf::seconds(5));
                    }
                    else
                    {
                        m_machine.sendData(sf::Packet() << "PLAY", sf::seconds(5));
                    }
                }
            }
        }

        m_movie.update();

        sf::Packet packet;
        if (m_machine.getSocket().receive(packet) == sf::Socket::Done)
        {
            std::string command;
            packet >> command;

            if (command == "NEW USER")
            {
                std::string username;
                packet >> username;

                showPopupMessage(username + " has connected");
            }
            else if (command == "PLAY")
            {
                std::string username;
                packet >> username;

                if (username == m_machine.getUsername()) username = "You";

                showPopupMessage(username + " played the movie");

                m_isPlaying = true;
                m_movie.play();
            }
            else if (command == "PAUSE")
            {
                std::string username;
                packet >> username;

                if (username == m_machine.getUsername()) username = "You";

                showPopupMessage(username + " paused the movie");

                m_isPlaying = false;
                m_movie.pause();
            }
            else if (command == "MESSAGE")
            {
                std::string message;
                packet >> message;

                showPopupMessage(message);
            }
        }

        if (m_popupMessage)
        {
            m_popupMessage->elapsedTime += m_timePerFrame;

            if (m_popupMessage->elapsedTime > m_popupMessage->maxTime)
            {
                m_popupMessage = nullptr;
            }
            else if (m_popupMessage->elapsedTime > m_popupMessage->maxTime/2.f)
            {
                auto alpha = (m_popupMessage->elapsedTime.asMilliseconds() - m_popupMessage->maxTime.asMilliseconds()/2) / m_popupMessage->maxTime.asMilliseconds()/2 * 255;
                m_popupMessage->text.setColor(sf::Color(0,255,0,alpha));
            }
        }

		m_timeSinceLastUpdate -= m_timePerFrame;
    }
}

void MovieState::draw()
{
    m_window.clear();

    m_window.draw(m_movie);

    if (m_popupMessage) m_window.draw(m_popupMessage->text);

    m_window.display();
}

void MovieState::showPopupMessage(const std::string& message)
{
    m_popupMessage = new PopupMessage;
    m_popupMessage->text.setFont(m_font);
    m_popupMessage->text.setCharacterSize(20);
    m_popupMessage->text.setColor(sf::Color::Green);
    m_popupMessage->text.setString(message);
    m_popupMessage->text.setPosition(10,10);

    m_popupMessage->maxTime = sf::seconds(4);
    m_popupMessage->elapsedTime = sf::Time::Zero;
}

}

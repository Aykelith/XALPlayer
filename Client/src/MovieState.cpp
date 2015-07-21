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
    m_popupMessageManager.setFont(m_font);

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

                m_popupMessageManager.add(username + " has connected");
            }
            else if (command == "PLAY")
            {
                std::string username;
                packet >> username;

                if (username == m_machine.getUsername()) username = "You";

                m_popupMessageManager.add(username + " played the movie");

                m_isPlaying = true;
                m_movie.play();
            }
            else if (command == "PAUSE")
            {
                std::string username;
                packet >> username;

                if (username == m_machine.getUsername()) username = "You";

                m_popupMessageManager.add(username + " paused the movie");

                m_isPlaying = false;
                m_movie.pause();
            }
            else if (command == "CHECKTIME")
            {
                time_t t = time(NULL);
                struct tm *tmp = gmtime(&t);

                m_machine.sendPacket(sf::Packet() << "CHECKTIME" << tmp->tm_hour << tmp->tm_min << tmp->tm_sec << m_movie.getPlayingOffset().asMilliseconds(), sf::seconds(3));
            }
            else if (command == "MESSAGE")
            {
                std::string message;
                packet >> message;

                m_popupMessageManager.add(message);
            }
        }

        m_popupMessageManager.update(m_timePerFrame);

		m_timeSinceLastUpdate -= m_timePerFrame;
    }
}

void MovieState::draw()
{
    m_window.clear();

    m_window.draw(m_movie);

    m_window.draw(m_popupMessageManager);

    m_window.display();
}

}

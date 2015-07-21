#include "SetupState.hpp"
#include "StateMachine.hpp"

namespace xal
{

SetupState::SetupState(StateMachine& machine, bool replace)
: State {machine, replace}
, m_configServerWindow { nullptr }
{
    m_timePerFrame = sf::seconds(1.0f / 60.0f); // Number of frames per second

    m_gui.setGlobalFont("data/DejaVuSans.ttf");

    auto usernameLabel = tgui::Label::create("data/Black.conf");
    usernameLabel->setText("Username:");
    m_gui.add(usernameLabel);

    auto usernameEditBox = tgui::EditBox::create("data/Black.conf");
    usernameEditBox->setSize(400, 30);
    usernameEditBox->setPosition(20 + usernameLabel->getSize().x + 20, 20);
    usernameEditBox->setText("AlexxanderX");

    m_gui.add(usernameEditBox);
    usernameLabel->setPosition(20, 20 + usernameEditBox->getSize().y/2 - usernameLabel->getSize().y/2);

    auto hostLabel = tgui::Label::create("data/Black.conf");
    hostLabel->setText("Host:");
    m_gui.add(hostLabel);

    auto hostEditBox = tgui::EditBox::create("data/Black.conf");
    hostEditBox->setSize(400, 30);
    hostEditBox->setPosition(usernameEditBox->getPosition().x, 20 + usernameEditBox->getSize().y + 20);
    hostEditBox->setText("192.168.1.2");

    m_gui.add(hostEditBox);
    hostLabel->setPosition(20, 20 + usernameEditBox->getSize().y + 20 + hostEditBox->getSize().y/2 - hostLabel->getSize().y/2);

    auto movieLabel = tgui::Label::create("data/Black.conf");
    movieLabel->setText("Movie:");
    m_gui.add(movieLabel);

    auto movieEditBox = tgui::EditBox::create("data/Black.conf");
    movieEditBox->setSize(400, 30);
    movieEditBox->setPosition(hostEditBox->getPosition().x,  hostEditBox->getPosition().y + hostEditBox->getSize().y + 20);

    m_gui.add(movieEditBox);
    movieLabel->setPosition(20, hostEditBox->getPosition().y + hostEditBox->getSize().y + 20 + movieEditBox->getSize().y/2 - movieLabel->getSize().y/2);

    auto connectButton = tgui::Button::create("data/Black.conf");
    connectButton->setText("Connect");
    connectButton->setSize(100,40);
    connectButton->setPosition((movieEditBox->getPosition().x + movieEditBox->getSize().x + 20)/2.f - connectButton->getSize().x/2, movieEditBox->getPosition().y + movieEditBox->getSize().y + 20);
    connectButton->connect("Pressed", [=](){
        // if (movieEditBox->getText().isEmpty())
        // {
        //     showErrorMessage("Error! The movie path cannot be empty!");
        // }

        sf::TcpSocket& socket = m_machine.getSocket();

        unsigned int port;
        std::string moviePath = "";

        #ifdef DEBUG
            std::ifstream file("debug.txt");
            file >> port >> moviePath;
            movieEditBox->setText(moviePath);
            file.close();
        #endif

        sf::Socket::Status status = socket.connect(sf::IpAddress(hostEditBox->getText().toAnsiString()), port, sf::seconds(5));
        if (status != sf::Socket::Done)
        {
            showErrorMessage("Error! Can't connect to the server!");
        }
        else
        {
            sfe::Movie movie;
            if (!movie.openFromFile(movieEditBox->getText()))
            {
                showErrorMessage("Error! Can't find the movie!");
            }

            if (m_machine.sendData(sf::Packet() << "CHECKMOVIE" << movie.getDuration().asMilliseconds(), sf::seconds(5)) == 1)
            {
                Debug("[NET]Packet sent");
            }

            sf::Packet packet;
            if (m_machine.receiveData(packet, sf::seconds(5)) == 1)
            {
                Debug("[NET]Packet received");

                std::string result;
                packet >> result;

                if (result == "GOOD")
                {
                    m_machine.setMoviePath(moviePath);
                    m_machine.setUsername(usernameEditBox->getText().toAnsiString());
                    m_next = m_machine.build<MovieState>(m_machine, true);
                }
                else
                {

                }
            }
        }
    });
    m_gui.add(connectButton);

    auto createServerButton = tgui::Button::create("data/Black.conf");
    createServerButton->setText("Create server");
    createServerButton->setSize(150, 40);
    createServerButton->setPosition(movieEditBox->getPosition().x + movieEditBox->getSize().x - 150, movieEditBox->getPosition().y + movieEditBox->getSize().y + 20);
    createServerButton->connect("Pressed", [=](){
        m_configServerWindow = new ConfigServerWindow();
    });
    m_gui.add(createServerButton);

    m_window.create({(unsigned int)(movieEditBox->getPosition().x + movieEditBox->getSize().x + 20), (unsigned int)(connectButton->getPosition().y + connectButton->getSize().y + 20)}, "XALPlayer - Setup");
    m_gui.setWindow(m_window);

    m_clock.restart();
}

void SetupState::pause()
{

}

void SetupState::resume()
{

}

void SetupState::update()
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

            //m_gui.handleEvent(event);
        }

        if (m_configServerWindow) m_configServerWindow->update();

		m_timeSinceLastUpdate -= m_timePerFrame;
    }
}

void SetupState::draw()
{
    m_window.clear();

    //m_gui.draw();

    m_window.display();

    if (m_configServerWindow) m_configServerWindow->draw();
}

void SetupState::showErrorMessage(const std::string& error)
{
    auto window = tgui::ChildWindow::create("data/Black.conf");
    window->setSize(m_window.getSize().x - 80, m_window.getSize().y - 80);
    window->setTitle("Error");
    m_gui.add(window);

    auto errorLabel = tgui::Label::create("data/Black.conf");
    errorLabel->setText(error);

    window->add(errorLabel);
    errorLabel->setPosition(window->getPosition().x + window->getSize().x/2 - errorLabel->getSize().x/2, window->getPosition().y + window->getSize().y/2 - errorLabel->getSize().y/2);
    window->setPosition(40,m_window.getSize().y/2 - window->getFullSize().y/2);
}


}

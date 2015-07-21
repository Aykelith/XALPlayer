#include "PopupMessageManager.hpp"

namespace xal
{

PopupMessage::PopupMessage(const sf::Font& font, const std::string& message)
: m_elapsedTime {sf::Time::Zero}
{
    m_text.setFont(font);
    m_text.setString(message);
    m_text.setCharacterSize(20);
    m_text.setColor(sf::Color::Green);

    m_text.setOrigin(m_text.getLocalBounds().left, m_text.getLocalBounds().top);
}

void PopupMessage::update(const sf::Time& dt)
{
    m_elapsedTime += dt;

    if (m_elapsedTime > m_maxTime/2.f && m_elapsedTime < m_maxTime)
    {
        float alpha = 255.f - (float(m_elapsedTime.asMilliseconds() - m_maxTime.asMilliseconds()/2.f) / float(m_maxTime.asMilliseconds()/2.f) * 255.f);
        m_text.setColor(sf::Color(0,255,0,alpha));
    }
    else if (m_elapsedTime > m_maxTime)
    {
        m_text.setColor(sf::Color(0,255,0,0));
    }
}

void PopupMessage::setMaxTime(const sf::Time &maxTime)
{
    m_maxTime = maxTime;
}

bool PopupMessage::needToDelete()
{
    return (m_text.getColor().a == 0);
}

sf::FloatRect PopupMessage::getGlobalBounds() const
{
    return {getPosition().x, getPosition().y, m_text.getLocalBounds().width, m_text.getLocalBounds().height};
}

void PopupMessage::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_text, states);
}

PopupMessageManager::PopupMessageManager()
{ }

void PopupMessageManager::update(const sf::Time& dt)
{
    for (auto it = m_messages.begin(); it != m_messages.end(); ++it)
    {
        (*it)->update(dt);

        if ((*it)->needToDelete())
        {
            it = m_messages.erase(it);
            if (it == m_messages.end())
                break;
            else
            {
                for (int i=m_messages.size()-1; i>=1; --i)
                {
                    m_messages[i]->setPosition(m_messages[i-1]->getPosition());
                }
                m_messages.front()->setPosition(10,10);
            }
        }
    }
}

void PopupMessageManager::add(const std::string& message)
{
    std::unique_ptr<PopupMessage> popupMessage(new PopupMessage(*m_font, message));
    popupMessage->setMaxTime(sf::seconds(4));
    if (m_messages.empty())
    {
        popupMessage->setPosition(10, 10);
    }
    else
    {
        popupMessage->setPosition(10, m_messages.back()->getPosition().y + m_messages.back()->getGlobalBounds().height + 10);
    }

    m_messages.push_back(std::move(popupMessage));
}

void PopupMessageManager::setFont(sf::Font& font)
{
    m_font = &font;
}

void PopupMessageManager::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (std::size_t i=0; i<m_messages.size(); ++i)
    {
        target.draw(*m_messages[i], states);
    }
}

}

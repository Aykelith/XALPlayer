#ifndef XAL_POPUPMESSAGEMANAGER_HPP
#define XAL_POPUPMESSAGEMANAGER_HPP

#include "Global.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <memory>

namespace xal
{

class PopupMessage: public sf::Drawable, public sf::Transformable
{
public:
    PopupMessage(const sf::Font& font, const std::string& message);

    void update(const sf::Time& dt);

    void setMaxTime(const sf::Time &maxTime);

    bool needToDelete();
    sf::FloatRect getGlobalBounds() const;
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    sf::Text m_text;
    sf::Time m_elapsedTime;
    sf::Time m_maxTime;
};

class PopupMessageManager: public sf::Drawable
{
public:
    PopupMessageManager();

    void update(const sf::Time& dt);

    void add(const std::string& message);
    void setFont(sf::Font& font);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    sf::Font* m_font;

    std::vector<std::unique_ptr<PopupMessage>> m_messages;
};

}

#endif

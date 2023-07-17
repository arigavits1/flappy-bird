#pragma once

#include <SFML/Graphics.hpp>

class Score
{
public:
    Score(sf::Font& font, const std::string& string, int size) : text(string, font, size)
    {
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setPosition(sf::Vector2f(10.0f, 10.0f));
    }

    void setString(const std::string& new_string)
    {
        text.setString(new_string);
    }

    void setPosition(sf::Vector2<float> new_pos)
    {
        text.setPosition(new_pos);
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(text);
    }

private:
    sf::Text text;
};
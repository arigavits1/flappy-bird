#pragma once
#include <SFML/Graphics.hpp>

class Object
{
public:

    Object(sf::Vector2<float> p_pos, float p_rotation, sf::Vector2<float> p_scale, sf::Texture* p_texture) : pos(p_pos), rotation(p_rotation), scale(p_scale), texture(p_texture)
    {
        sprite.setTexture(*texture);
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2, bounds.height / 2);
        sprite.setPosition(pos);
        sprite.setRotation(rotation);
        sprite.setScale(scale);
    }

    inline void move(sf::Vector2<float> speed)
    {
        sprite.setPosition(sf::Vector2(sprite.getPosition().x + speed.x, sprite.getPosition().y + speed.y));
    }

    inline void setPosition(sf::Vector2<float> position)
    {
        sprite.setPosition(position);
    }

    inline void rotate(float angle)
    {
        sprite.setRotation(angle);
    }

    inline sf::Sprite& getSprite()
    {
        return sprite;
    }

private:
    sf::Sprite sprite;
    sf::Vector2<float> pos;
    float rotation;
    sf::Vector2<float> scale;
    sf::Texture* texture;
};
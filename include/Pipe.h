#pragma once

#include <SFML/Graphics.hpp>
#include "Object.h"

class Pipe : public Object
{
public:
    Pipe(sf::Vector2<float> p_pos, float p_rotation, sf::Vector2<float> p_scale, sf::Texture* p_texture) : Object(p_pos, p_rotation, p_scale, p_texture)
    { }

private:
    
};
#pragma once
#include "win.hpp"

#include <SFML/Graphics.hpp> // @TODO: move up to draw.hpp

namespace sim
{

void init();
void update(float dt, const input_t &input);
void draw(sf::RenderWindow &view);

};


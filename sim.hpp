#pragma once
#include <SFML/Graphics.hpp> // @TODO: move up to draw.hpp

namespace sim
{

void init();
void update(float dt, float time);
void draw(sf::RenderWindow &view);

};


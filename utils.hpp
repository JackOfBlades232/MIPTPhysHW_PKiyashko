#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <cmath>

inline float vlen(sf::Vector2f v) { return sqrtf(v.x*v.x + v.y*v.y); }
inline sf::Vector2f vnormalize(sf::Vector2f v) { return v / vlen(v); }

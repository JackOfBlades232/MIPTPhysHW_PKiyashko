#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <cmath>

template <class T>
inline T vlen(sf::Vector2<T> v) { return sqrtf(v.x*v.x + v.y*v.y); }
template <class T>
inline sf::Vector2<T> vnormalize(sf::Vector2<T> v) { return v / vlen(v); }

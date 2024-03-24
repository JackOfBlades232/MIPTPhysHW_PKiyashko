#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <cmath>

#define MIN(_a, _b) ((_a) < (_b) ? (_a) : (_b))
#define MAX(_a, _b) ((_a) > (_b) ? (_a) : (_b))

using vec2d_t = sf::Vector2<double>;
struct box2d_t {
    vec2d_t min, max;
};

template <class T>
inline T vlen(sf::Vector2<T> v) { return sqrtf(v.x*v.x + v.y*v.y); }
template <class T>
inline sf::Vector2<T> vnormalize(sf::Vector2<T> v) { return v / vlen(v); }

template <class T>
inline sf::Vector2<T> vmin(sf::Vector2<T> v1, sf::Vector2<T> v2) {
    return sf::Vector2<T>(MIN(v1.x, v2.x), MIN(v1.y, v2.y));
}
template <class T>
inline sf::Vector2<T> vmax(sf::Vector2<T> v1, sf::Vector2<T> v2) {
    return sf::Vector2<T>(MAX(v1.x, v2.x), MAX(v1.y, v2.y));
}


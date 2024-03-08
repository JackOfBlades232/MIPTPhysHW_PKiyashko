#include "sim.hpp"
#include "draw.hpp"

#include <ascent/Ascent.h>

namespace sim
{

static sf::CircleShape body(35);
static Spring<30> spring(25.f, 50.f);
static sf::Vector2f spring_anchor = sf::Vector2f(960, 220);

static sf::Vector2f ball_pos;
static sf::Vector2f ball_center;
static sf::Vector2f ball_anchor;

static asc::state_t state;

static asc::Param pos_x(state), pos_y(state);
static asc::Param vel_x(state), vel_y(state);

static const double k = 2000.0;
static const double d = 5.0;
static const double g = 10.0;
static const double m = 1.0;

static double l0 = 200.0;

static asc::RK4 integrator;

static void system(const asc::state_t &x, asc::state_t &D, const double t)
{

}

void init()
{
    body.setFillColor(sf::Color::Black);
    body.setOutlineColor(sf::Color::White);
    body.setOutlineThickness(-4);

    pos_x = 960;
    pos_y = 540;
    vel_x = 0;
    vel_y = 0;

    state.reserve(100);
}

void update(float dt, float time)
{
    ball_pos = sf::Vector2f(960, 540) + 100.f * sf::Vector2f(sinf(time), cosf(time));
    ball_center = ball_pos + sf::Vector2f(body.getRadius(), body.getRadius());
    ball_anchor = ball_center + vnormalize(spring_anchor - ball_center) * body.getRadius();
}

void draw(sf::RenderWindow &view)
{
    body.setPosition(ball_pos);
    spring.SetPositions(spring_anchor, ball_anchor);

    view.clear();
    {
        view.draw(spring);
        view.draw(body);
    }
    view.display();
}

}

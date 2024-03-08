#include "sim.hpp"
#include "draw.hpp"
#include "phys.hpp"

#include <ascent/Ascent.h>

#include <vector>
#include <cmath>
#define M_PI 3.14159265358979323846   // pi

namespace sim
{

static constexpr int LINE_NUM_POINTS = 20;

static draw::CircularLine<LINE_NUM_POINTS> shape(7, sf::Color(255, 0, 0, 255));
static sf::Vector2f point_positions[LINE_NUM_POINTS];

static double time = 0.0;
static asc::Euler integrator;
static asc::state_t state;

static std::vector<phys::Body2d> mass_points = {};
static phys::Body2d *control_point           = nullptr;
static std::vector<phys::Spring2d> springs   = {};
static std::vector<phys::Damper2d> dampers   = {};

static void system(const asc::state_t &x, asc::state_t &D, const double t)
{
    for (auto &spring : springs)
        spring(x, D, t);
    for (auto &damper : dampers)
        damper(x, D, t);
    for (auto &point : mass_points) {
        if (&point == control_point)
            continue;

        point(x, D, t);
    }
}

void init()
{
    state.reserve(LINE_NUM_POINTS*4*2);
    phys::set_world_bbox(0, 0, 1920, 900); // @TODO: make api for resolution query

    for (int i = 0; i < LINE_NUM_POINTS; i++) {
        float angle = i * 2.f * M_PI / LINE_NUM_POINTS;
        sf::Vector2f pos(960 + 100*cosf(angle), 540 + 100*sinf(angle));
        mass_points.emplace_back(state);
        mass_points[i].m = 1.0;
        mass_points[i].x = pos.x;
        mass_points[i].y = pos.y;
        mass_points[i].vx = mass_points[i].vy = 0.0;
    }
    control_point = &mass_points[0];

    for (int i = 0; i < LINE_NUM_POINTS; i++) {
        springs.emplace_back(mass_points[i], mass_points[(i+1) % LINE_NUM_POINTS]);
        dampers.emplace_back(mass_points[i], mass_points[(i+1) % LINE_NUM_POINTS]);
        springs[i].k = 10000.0;
        dampers[i].c = 0.025;
    }
}

void update(float dt, const input_t &input)
{
    control_point->x = input.mouse_x;
    control_point->y = input.mouse_y;

    integrator(system, state, time, (double)dt);

    for (int i = 0; i < LINE_NUM_POINTS; i++)
        point_positions[i] = sf::Vector2f((float)mass_points[i].x, (float)mass_points[i].y);
}

void draw(sf::RenderWindow &view)
{
    shape.SetPositions(point_positions, LINE_NUM_POINTS);

    view.clear();
    {
        view.draw(shape);
    }
    view.display();
}

}

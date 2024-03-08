#include "sim.hpp"
#include "draw.hpp"
#include "phys.hpp"

#include <ascent/Ascent.h>

#include <vector>
#include <memory>

#include <cmath>
#define M_PI 3.14159265358979323846   // pi

namespace sim
{

static constexpr double CLICK_SPEED = 700.0;
static constexpr double MAX_ADDED_SPEED = 1500.0;

static constexpr int LINE_NUM_POINTS = 20;

static draw::CircularLine<LINE_NUM_POINTS> shape(7, sf::Color(255, 0, 0, 255));
static sf::Vector2f point_positions[LINE_NUM_POINTS];

static double time = 0.0;
static asc::Euler integrator;
static asc::state_t state;

static std::vector<phys::Body2d> mass_points      = {};
static std::vector<phys::Spring2d> springs        = {};
static std::vector<phys::Damper2d> dampers        = {};
static std::unique_ptr<phys::Pressure2d> pressure = nullptr;

static void system(const asc::state_t &x, asc::state_t &D, const double t)
{
    (*pressure)(x, D, t);
    for (auto &spring : springs)
        spring(x, D, t);
    for (auto &damper : dampers)
        damper(x, D, t);
    for (auto &point : mass_points)
        point(x, D, t);
}

void init()
{
    state.reserve(LINE_NUM_POINTS*4*2);
    phys::set_world_bbox(0, 0, 1800, 900); // @TODO: make api for resolution query

    mass_points.reserve(LINE_NUM_POINTS);
    springs.reserve(LINE_NUM_POINTS);
    dampers.reserve(LINE_NUM_POINTS);

    for (int i = 0; i < LINE_NUM_POINTS; ++i) {
        float angle = i * 2.f * M_PI / LINE_NUM_POINTS;
        sf::Vector2f pos(150 + 100*cosf(angle), 750 + 100*sinf(angle));
        mass_points.emplace_back(state);
        mass_points[i].m = 1.0;
        mass_points[i].x = pos.x;
        mass_points[i].y = pos.y;
        mass_points[i].vx = mass_points[i].vy = 0.0;
    }

    for (int i = 0; i < LINE_NUM_POINTS; ++i) {
        springs.emplace_back(mass_points[i], mass_points[(i+1) % LINE_NUM_POINTS]);
        dampers.emplace_back(mass_points[i], mass_points[(i+1) % LINE_NUM_POINTS]);
        springs[i].k = 1e4;
        dampers[i].c = 5.0;
    }

    pressure = std::make_unique<phys::Pressure2d>(springs.data(), springs.size());
    pressure->nt = 2000.0;
}

void update(float dt, const input_t &input)
{
    if (input.mouse_clicked) {
        vec2d_t mass_center{};
        for (const phys::Body2d &body : mass_points)
            mass_center += vec2d_t{body.x, body.y};
        mass_center /= (double)LINE_NUM_POINTS;

        vec2d_t offset = CLICK_SPEED * vnormalize(vec2d_t{input.mouse_x, input.mouse_y} - mass_center);

        for (const phys::Body2d &body : mass_points) {
            body.vx += offset.x;
            body.vy += offset.y;
            vec2d_t v{body.vx, body.vy};
            double speed = vlen(v);
            if (speed > MAX_ADDED_SPEED) {
                vec2d_t clampVelocityVal = vnormalize(v) * (MAX_ADDED_SPEED - speed);
                body.vx += clampVelocityVal.x;
                body.vy += clampVelocityVal.y;
            }
        }
    }

    box2d_t points_bbox = phys::get_body_family_bbox(mass_points.data(), mass_points.size());
    pressure->area = (points_bbox.max.x - points_bbox.min.x) * (points_bbox.max.y - points_bbox.min.y);

    integrator(system, state, time, (double)dt);

    for (int i = 0; i < LINE_NUM_POINTS; ++i)
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

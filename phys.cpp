#include "phys.hpp"

#include <cassert>

namespace phys
{

static box2d_t world_box{};

void set_world_bbox(double xmin, double ymin, double xmax, double ymax)
{
    world_box = box2d_t{vec2d_t{xmin, ymin}, vec2d_t{xmax, ymax}};
}

vec2d_t get_constraint_force_from_world_box(double x, double y, double vx, double vy)
{
    // Using both fake springs and dampers
    static constexpr double k = 1e3;
    static constexpr double c = 10.0;

    vec2d_t f{};

    if (x < world_box.min.x)
        f.x = k * (world_box.min.x - x) - c * vx;
    else if (x > world_box.max.x)
        f.x = k * (world_box.max.x - x) - c * vx;

    if (y < world_box.min.y)
        f.y = k * (world_box.min.y - y) - c * vy;
    else if (y > world_box.max.y)
        f.y = k * (world_box.max.y - y) - c * vy;

    return f;
}

box2d_t get_body_family_bbox(const Body2d *bodies, size_t springs_cnt)
{
    assert(springs_cnt > 0);
    box2d_t box = {{bodies[0].x, bodies[0].y}, {bodies[0].x, bodies[0].y}};
    for (const Body2d *body = bodies; body-bodies < springs_cnt; body++) {
        if (body->x < box.min.x)
            box.min.x = body->x;
        if (body->y < box.min.y)
            box.min.y = body->y;
        if (body->x > box.max.x)
            box.max.x = body->x;
        if (body->y > box.max.y)
            box.max.y = body->y;
    }
    return box;
}

}

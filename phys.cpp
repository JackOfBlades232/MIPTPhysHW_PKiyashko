#include "phys.hpp"

#include <cassert>

namespace phys
{

static box2d_t world_box{};

void set_world_bbox(double xmin, double ymin, double xmax, double ymax)
{
    world_box = box2d_t{vec2d_t{xmin, ymin}, vec2d_t{xmax, ymax}};
}

void clamp_body_params_by_world_box(asc::Param &x, asc::Param &y, asc::Param &vx, asc::Param &vy)
{
    if (x < world_box.min.x) {
        x = world_box.min.x;
        if (vx < 0)
            vx = 0;
    }
    else if (x > world_box.max.x) {
        x = world_box.max.x;
        if (vx > 0)
            vx = 0;
    }

    if (y < world_box.min.y) {
        y = world_box.min.y;
        if (vy < 0)
            vy = 0;
    }
    else if (y > world_box.max.y) {
        y = world_box.max.y;
        if (vy > 0)
            vy = 0;
    }
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

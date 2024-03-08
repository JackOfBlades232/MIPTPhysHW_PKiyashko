#include "phys.hpp"

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

}

#include "phys.hpp"

namespace phys
{

static box2d_t world_box{};

void set_world_bbox(double xmin, double ymin, double xmax, double ymax)
{
    world_box = box2d_t{vec2d_t{xmin, ymin}, vec2d_t{xmax, ymax}};
}

void verlet_apply_world_constraints(Span<VerletMassPoint2d> points)
{
    for (VerletMassPoint2d &point : points)
        point.m_pos = vmin(vmax(point.m_pos, world_box.min), world_box.max);
}

}

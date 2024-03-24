#pragma once
#include "utils.hpp"
#include "span.hpp"

#include <vector>

namespace phys
{

static constexpr double c_gravity = 9.80665;

class VerletMassPoint2d;

void set_world_bbox(double xmin, double ymin, double xmax, double ymax);
void verlet_apply_world_constraints(Span<VerletMassPoint2d> points);

// @NOTE(PKiyashko): it would be a good idea to use a PointSystem type class instead
//                   and make use of SOA for all attribute updates
class VerletMassPoint2d {
    friend void verlet_apply_world_constraints(Span<VerletMassPoint2d>);

    vec2d_t m_pos      {};
    vec2d_t m_prev_pos {};
    vec2d_t m_force    {};
    double m_inv_mass = 0;

public:
    VerletMassPoint2d() = default;
    // @TODO(PKiyashko): add init velocity
    explicit VerletMassPoint2d(vec2d_t init_pos, double mass) 
        : m_inv_mass(1.0/mass), m_pos(init_pos), m_prev_pos(init_pos) {};

    void Integrate(double dt) {
        vec2d_t acc = m_force*m_inv_mass;
        vec2d_t cur_pos = m_pos;
        m_pos += cur_pos - m_prev_pos + acc*dt*dt;
        m_prev_pos = cur_pos;
    }

    void AccumulateForces() { m_force = vec2d_t{0.0, c_gravity}; }

    vec2d_t GetPosition() const { return m_pos; }
};

}
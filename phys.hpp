#pragma once
#include "utils.hpp"
#include "span.hpp"

#include <cmath>
#include <vector>

namespace phys
{

static constexpr double c_force_scale = 150.0;
static constexpr double c_gravity     = 9.80665;

class VerletMassPoint2d;

void set_world_bbox(double xmin, double ymin, double xmax, double ymax);
void verlet_apply_world_constraints(Span<VerletMassPoint2d> points);

// @NOTE(PKiyashko): it would be a good idea to use a PointSystem type class instead
//                   and make use of SOA for all attribute updates
class VerletMassPoint2d {
    friend void verlet_apply_world_constraints(Span<VerletMassPoint2d>);
    friend class Verlet2PointsConstraint2d;
    friend class VerletPressureConstraint2d;

    vec2d_t m_pos      {};
    vec2d_t m_prev_pos {};
    vec2d_t m_force    {};

    double m_mass       = 0.0;
    double m_damp_factor = 0.0;

public:
    VerletMassPoint2d() = default;
    // @TODO(PKiyashko): add init velocity
    explicit VerletMassPoint2d(vec2d_t init_pos, double mass, double damp_factor)
        : m_mass(mass), m_damp_factor(damp_factor), m_pos(init_pos), m_prev_pos(init_pos) {};

    void Integrate(double dt) {
        vec2d_t acc = m_force/m_mass;
        vec2d_t cur_pos = m_pos;
        m_pos += (cur_pos - m_prev_pos) * m_damp_factor + acc*dt*dt;
        m_prev_pos = cur_pos;
    }

    void AccumulateForces() { m_force = c_force_scale * vec2d_t{0.0, c_gravity}; }

    vec2d_t GetPosition() const { return m_pos; }
    void SetVelocity(vec2d_t vel) { m_pos = m_prev_pos + vel; }
};

class Verlet2PointsConstraint2d {
    double m_rest_length      = 0.0;
    double m_relaxation_coeff = 1.0;

public:
    Verlet2PointsConstraint2d() = default;
    explicit Verlet2PointsConstraint2d(double rest_length, double relaxation_coeff)
        : m_rest_length(rest_length), m_relaxation_coeff(relaxation_coeff) {}

    // @SPEED(PKiyashko): this is not optimized at all (verbose & w/ 2xsqrt), and could be if it will be needed.
    void Apply(VerletMassPoint2d &p1, VerletMassPoint2d &p2) {
        vec2d_t offset = p1.m_pos - p2.m_pos;
        double length  = vlen(offset);

        vec2d_t shift_base        = vnormalize(offset) * (length - m_rest_length) * m_relaxation_coeff;
        double first_point_weight = p1.m_mass / (p1.m_mass + p2.m_mass);

        // Switch because heavier point gets offset less
        p1.m_pos -= shift_base * (1.0 - first_point_weight);
        p2.m_pos += shift_base * first_point_weight;
    }
};

}
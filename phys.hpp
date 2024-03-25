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

class VerletPressureConstraint2d {
    double m_rest_area = 0.0;
    double m_relaxation_coeff = 1.0;

public:
    VerletPressureConstraint2d() = default;
    explicit VerletPressureConstraint2d(double rest_area, double relaxation_coeff)
        : m_rest_area(rest_area), m_relaxation_coeff(relaxation_coeff) {}

    // @FEATURE(PKiyashko): this could be remade to take an interface/be templated to
    //                      be able to take any container, even not contiguous.
    // @SPEED(PKiyashko):   This is, again, not necessarily optimal.
    void Apply(Span<VerletMassPoint2d> points) {
        // Area by shoelace formula (sum of oriented trapezoid areas)
        double area = 0.0;
        vec2d_t mass_center{};
        for (size_t i = 0; i < points.Size(); ++i) {
            size_t ip1 = i == points.Size()-1 ? 0 : i+1;
            area += (points[i].m_pos.y + points[ip1].m_pos.y) * (points[i].m_pos.x - points[ip1].m_pos.x);
            mass_center += points[i].m_pos;
        }
        area *= 0.5;
        mass_center /= (double)points.Size();

        // @SPEED(PKiyashko): will storing inv_rest_area be faster?
        double area_coeff = area / m_rest_area;
        double linear_offset_coeff = sqrt(area_coeff) - 1.0;

        // @TODO(PKiyashko): This does not take masses into account.
        //                   I don't quite understand how to factor in masses properly,
        //                   meaning, that it is unclear how to distribute constraints to points w/
        //                   different masses. This may be useful to return to.
        std::vector<vec2d_t> required_offsets(points.Size());
        for (size_t i = 0; i < points.Size(); ++i) {
            vec2d_t radial_vector = points[i].m_pos - mass_center;
            // Negated because we are counteracting the offset
            double full_offset = -vlen(radial_vector) * linear_offset_coeff * m_relaxation_coeff;

            vec2d_t edge1 = points[i].m_pos - points[i == 0 ? points.Size()-1 : i-1].m_pos;
            vec2d_t edge2 = points[i == points.Size()-1 ? 0 : i+1].m_pos - points[i].m_pos;

            vec2d_t norm1 = vnormalize(vec2d_t{edge1.y, -edge1.x});
            vec2d_t norm2 = vnormalize(vec2d_t{edge2.y, -edge2.x});

            required_offsets[i] = vnormalize((norm1 + norm2) * 0.5) * full_offset;
        }
        for (size_t i = 0; i < points.Size(); ++i)
            points[i].m_pos += required_offsets[i];
    }
};

}
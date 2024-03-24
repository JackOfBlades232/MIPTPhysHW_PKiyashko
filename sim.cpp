#include "sim.hpp"
#include "draw.hpp"
#include "phys.hpp"

#include <ascent/Ascent.h>

#include <array>
#include <memory>

#include <cmath>
#define M_PI 3.14159265358979323846 // pi

namespace sim
{

/// State for all simulations ///
static std::vector<const sf::Drawable *> render_shapes{};
static constexpr double c_fixed_dt = 1.0/120.0;
static double accumulated_dt = 0.0;

/// Sim specific state ///
static constexpr int c_line_num_points = 20;

// @NOTE(PKiyashko): setting these constants separately in two places, while they are matched, lazy)
static draw::Box                             world_box(88, 88, 1832, 992);
static draw::CircularLine<c_line_num_points> softbody_shape(7, sf::Color(255, 0, 0, 255));

static constexpr int c_constraint_iterations = 10;

static std::array<phys::VerletMassPoint2d, c_line_num_points>         mass_points;
static std::array<phys::Verlet2PointsConstraint2d, c_line_num_points> edge_constraints;
static phys::VerletGasPressureConstraint2d                            pressure_constraint;

static double time = 0.0;

void init()
{
    render_shapes.push_back(&world_box);
    render_shapes.push_back(&softbody_shape);

    for (size_t id = 0; id < c_line_num_points; ++id) {
        double angle = 2.0 * M_PI * id / c_line_num_points;
        mass_points[id]      = phys::VerletMassPoint2d(vec2d_t{400.0, 200.0} + 100.0*vec2d_t{cos(angle), sin(angle)}, 1.0, 0.995);
        edge_constraints[id] = phys::Verlet2PointsConstraint2d(31.286893008, 0.5);
    }
    pressure_constraint = phys::VerletGasPressureConstraint2d(59432.35113, 0.05);

    phys::set_world_bbox(100, 100, 1820, 980);
}

void update(float dt, const input_t &input)
{
    accumulated_dt += dt;
    while (accumulated_dt >= c_fixed_dt) {
        // @TODO(PKiyashko): this would really really be better with SOA, redo at least in next HW.
        for (phys::VerletMassPoint2d &mass_point : mass_points)
            mass_point.AccumulateForces();
        for (phys::VerletMassPoint2d &mass_point : mass_points)
            mass_point.Integrate(c_fixed_dt);

        for (int i = 0; i < c_constraint_iterations; ++i) {
            //pressure_constraint.Apply(make_span(mass_points.data(), mass_points.size()));
            for (size_t j = 0; j < c_line_num_points; ++j)
                edge_constraints[j].Apply(mass_points[j], mass_points[(j + 1) % c_line_num_points]);
            phys::verlet_apply_world_constraints(make_span(mass_points.data(), mass_points.size()));
        }

        accumulated_dt -= c_fixed_dt;
    }
    
    for (size_t id = 0; id < c_line_num_points; ++id)
        softbody_shape.SetPointPosition(id, sf::Vector2f(mass_points[id].GetPosition()));
}

// @NOTE(PKiyashko): this function should not be changed, I think, just go through the
//                   homogeneous collection of custom-implemented drawables.
void draw(sf::RenderWindow &view)
{
    view.clear();
    for (const sf::Drawable *shape : render_shapes)
        view.draw(*shape);
    view.display();
}

}

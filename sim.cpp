#include "sim.hpp"
#include "draw.hpp"
#include "phys.hpp"

#include <ascent/Ascent.h>

#include <array>
#include <memory>

#include <cmath>
#define M_PI 3.14159265358979323846 // pi

// @NOTE(PKiyashko): I am sincerely sorry for all the for (y) for (x) {}

namespace sim
{

/// State for all simulations ///
static std::vector<const sf::Drawable *> render_shapes{};
static constexpr double c_fixed_dt = 1.0/120.0;
static double accumulated_dt = 0.0;

/// Sim specific state ///
static constexpr int c_sponge_dim = 9;

// @NOTE(PKiyashko): setting these constants separately in two places, while they are matched, lazy)
static draw::Box                  world_box(103, 103, 991, 991);
static draw::Sponge<c_sponge_dim> sponge(5, sf::Color(255, 0, 0, 255));

static constexpr int c_constraint_iterations = 5;

static phys::VerletMassPoint2d         mass_points[c_sponge_dim][c_sponge_dim];
static phys::Verlet2PointsConstraint2d edge_constraint;
static phys::Verlet2PointsConstraint2d diag_constraint;

static constexpr double c_click_velocity = 15.0;

static double time = 0.0;

void init()
{
    render_shapes.push_back(&world_box);
    render_shapes.push_back(&sponge);

    for (size_t y = 0; y < c_sponge_dim; ++y)
        for (size_t x = 0; x < c_sponge_dim; ++x) {
            mass_points[y][x] = phys::VerletMassPoint2d(
                vec2d_t{400.0, 200.0} + 200.0*vec2d_t{(double)x, (double)y}/(double)c_sponge_dim, 
                1.0, 0.99);
        }
    edge_constraint = phys::Verlet2PointsConstraint2d(25.0, 0.3);
    diag_constraint = phys::Verlet2PointsConstraint2d(35.355339, 0.3);

    phys::set_world_bbox(100, 100, 980, 980);
}

void update(float dt, const input_t &input)
{
    accumulated_dt += dt;
    if (input.mouse_clicked) {
        // @TODO(PKiyashko): This DEFINITELY should be done against a center of masses,
        //                   but I am half asleep at the moment)
        for (size_t y = 0; y < c_sponge_dim; ++y)
            for (size_t x = 0; x < c_sponge_dim; ++x) {
                phys::VerletMassPoint2d &point = mass_points[y][x];
                point.SetVelocity(
                    vnormalize(vec2d_t{input.mouse_x, input.mouse_y} - point.GetPosition()) * c_click_velocity);
            }
    }
    while (accumulated_dt >= c_fixed_dt) {
        // @TODO(PKiyashko): this would really really be better with SOA, redo at least in next HW.
        for (size_t y = 0; y < c_sponge_dim; ++y)
            for (size_t x = 0; x < c_sponge_dim; ++x)
                mass_points[y][x].AccumulateForces();
        for (size_t y = 0; y < c_sponge_dim; ++y)
            for (size_t x = 0; x < c_sponge_dim; ++x)
                mass_points[y][x].Integrate(c_fixed_dt);

        for (int iter = 0; iter < c_constraint_iterations; ++iter) {
            // @SPEED(PKiyashko): this is a stupid n^2, but it is simpler to write and it's time to be done.
            for (size_t y = 0; y < c_sponge_dim; ++y)
                for (size_t x = 0; x < c_sponge_dim; ++x)
                    for (size_t i = 0; i < c_sponge_dim; ++i)
                        for (size_t j = 0; j < c_sponge_dim; ++j) {
                            if ((i-y == 1 && j-x == 1) || (i-y == -1 && j-x == 1))
                                diag_constraint.Apply(mass_points[y][x], mass_points[i][j]);
                            else if ((i-y == 1 && j-x == 0) || (i-y == 0 && j-x == 1))
                                edge_constraint.Apply(mass_points[y][x], mass_points[i][j]);
                        }

            phys::verlet_apply_world_constraints(
                make_span(&mass_points[0][0], sizeof(mass_points)/sizeof(mass_points[0][0])));
        }

        accumulated_dt -= c_fixed_dt;
    }
    
    for (size_t y = 0; y < c_sponge_dim; ++y)
        for (size_t x = 0; x < c_sponge_dim; ++x) {
            vec2d_t pos = mass_points[y][x].GetPosition();
            sponge.SetPointPosition(x, y, sf::Vector2f(pos.x, pos.y));
        }
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

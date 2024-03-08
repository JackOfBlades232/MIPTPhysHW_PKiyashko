#pragma once
#include "utils.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics.hpp>
#include <ascent/Ascent.h>

namespace phys
{

using vec2d_t = sf::Vector2<double>;
struct box2d_t {
    vec2d_t min, max;
};

static constexpr double GRAVITY = 9.80665;
static constexpr double FORCE_SCALE = 10.0;

void set_world_bbox(double xmin, double ymin, double xmax, double ymax);
void clamp_body_params_by_world_box(asc::Param &x, asc::Param &y, asc::Param &vx, asc::Param &vy);

// @TODO: make gravity optional
struct Body2d {
    asc::Param x, y;   // position
    asc::Param vx, vy; // velocity
    double m{}; // mass
    vec2d_t f{}; // force

    Body2d(asc::state_t &state) : x(state), y(state), vx(state), vy(state) {}

    void operator()(const asc::state_t &, asc::state_t &D, const double)
    {
        x(D) = vx;
        y(D) = vy;

        if (m > 0.0) {
            vx(D) = FORCE_SCALE * f.x/m;
            vy(D) = FORCE_SCALE * (f.y/m + GRAVITY);
        } else
            vx(D) = vy(D) = 0.0;

        f.x = f.y = 0.0;

        clamp_body_params_by_world_box(x, y, vx, vy);
    }

    vec2d_t Pos() const { return vec2d_t((double)x, (double)y); }
    vec2d_t Vel() const { return vec2d_t((double)vx, (double)vy); }
};

struct Damper2d {
    Body2d &b0;
    Body2d &b1;

    vec2d_t dv{}; // velocity difference
    vec2d_t f{}; // force
    double c{}; // damping coefficient

    Damper2d(Body2d &b0, Body2d &b1) : b0(b0), b1(b1) {}

    void operator()(const asc::state_t &, asc::state_t &, const double)
    {
        dv = b0.Vel() - b1.Vel();
        f = c*dv;

        b0.f -= f;
        b1.f += f;
    }
};

struct Spring2d {

    Body2d &b0;
    Body2d &b1;

    double l0{}; // initial spring length (distance between masses)
    double ds{}; // spring compression/extension
    double k{}; // spring coefficient
    vec2d_t f{}; // force

    Spring2d(Body2d &b0, Body2d &b1) : b0(b0), b1(b1) { l0 = vlen(b1.Pos() - b0.Pos()); }

    void operator()(const asc::state_t &, asc::state_t &, const double)
    {
        vec2d_t diff = b1.Pos() - b0.Pos();
        ds = l0 - vlen(diff);
        f = ds * vnormalize(diff);

        b0.f -= f;
        b1.f += f;
    }
};

}
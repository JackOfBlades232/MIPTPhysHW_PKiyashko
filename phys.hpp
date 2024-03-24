#pragma once
#include "utils.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics.hpp>
#include <ascent/Ascent.h>

namespace phys
{

static constexpr double GRAVITY = 9.80665;
static constexpr double R = 8.314462618;

static constexpr double FORCE_SCALE = 10.0;

void set_world_bbox(double xmin, double ymin, double xmax, double ymax);
vec2d_t get_constraint_force_from_world_box(double x, double y, double vx, double vy);

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

        f += get_constraint_force_from_world_box(x, y, vx, vy);

        if (m > 0.0) {
            vx(D) = FORCE_SCALE * f.x/m;
            vy(D) = FORCE_SCALE * (f.y/m + GRAVITY);
        } else
            vx(D) = vy(D) = 0.0;

        f.x = f.y = 0.0;
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

struct Pressure2d {
    Spring2d *springs;
    size_t springs_cnt;

    double area{};
    double nt{}; // nu of the gas * temperature (presume constant)

    Pressure2d(Spring2d *springs, size_t cnt) : springs(springs), springs_cnt(cnt) {}

    void operator()(const asc::state_t &, asc::state_t &, const double)
    {
        for (size_t i = 0; i < springs_cnt; i++) {
            Spring2d &spring = springs[i];
            
            vec2d_t side = vec2d_t{spring.b1.x, spring.b1.y} - vec2d_t{spring.b0.x, spring.b0.y};
            vec2d_t norm = vnormalize(vec2d_t{side.y, -side.x});
            double len = vlen(side);

            double f = len * nt * R / area; // pressure = nu*R*T/V, force = pressure*area, V->area, area->len in 2d case
            spring.b0.f += norm * f;
            spring.b1.f += norm * f;
        }
    }
};

box2d_t get_body_family_bbox(const Body2d *bodies, size_t springs_cnt);

}
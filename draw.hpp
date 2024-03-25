#pragma once
#include "utils.hpp"

#include <SFML/Graphics.hpp>

#include <cassert>

namespace draw
{

class Box : public sf::Drawable {
    box2d_t m_box{};

public:
    Box(box2d_t box) : m_box(box) {}
    Box(double xmin, double ymin, double xmax, double ymax)
        : m_box(box2d_t{vec2d_t{xmin, ymin}, vec2d_t{xmax, ymax}}) {}

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override { 
        sf::Vertex nodes[5];
        nodes[0].position = sf::Vector2f(m_box.min.x, m_box.min.y);
        nodes[1].position = sf::Vector2f(m_box.min.x, m_box.max.y);
        nodes[2].position = sf::Vector2f(m_box.max.x, m_box.max.y);
        nodes[3].position = sf::Vector2f(m_box.max.x, m_box.min.y);
        nodes[4].position = nodes[0].position;
        target.draw(nodes, 5, sf::LinesStrip, states);
    }
};

template <size_t t_dim>
class Sponge : public sf::Drawable {
    sf::CircleShape m_points[t_dim][t_dim];

    // @SPEED(PKiyashko): if this needs to go faster, 
    //                    I should cache the lines connecting the dots.

public:
    Sponge(float point_rad, sf::Color point_color) {
        for (size_t y = 0; y < t_dim; ++y)
            for (size_t x = 0; x < t_dim; ++x) {
                sf::CircleShape &point = m_points[y][x];
                point = sf::CircleShape(point_rad);
                point.setFillColor(point_color);
            }
    }

    void SetPointPosition(size_t point_x_id, size_t point_y_id, sf::Vector2f position) {
        assert(point_x_id < t_num_points);
        assert(point_y_id < t_num_points);
        m_points[point_y_id][point_x_id].setPosition(position);
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override { 
        sf::Vertex line[2];
        float radius = m_points[0][0].getRadius();
        sf::Vector2f rad_offset = sf::Vector2f(radius, radius);
        for (size_t y = 0; y < t_dim; ++y)
            for (size_t x = 0; x < t_dim; ++x) {
                line[0].position = m_points[y][x].getPosition() + rad_offset;
                // Left
                if (x < t_dim-1) {
                    line[1].position = m_points[y][x+1].getPosition() + rad_offset;
                    target.draw(line, 2, sf::LinesStrip, states);
                }
                // Down
                if (y < t_dim-1) {
                    line[1].position = m_points[y+1][x].getPosition() + rad_offset;
                    target.draw(line, 2, sf::LinesStrip, states);
                }
                // Diag down to the right
                if (x < t_dim-1 && y < t_dim-1) {
                    line[1].position = m_points[y+1][x+1].getPosition() + rad_offset;
                    target.draw(line, 2, sf::LinesStrip, states);
                }
                // Diag up to the right
                if (x < t_dim-1 && y > 0) {
                    line[1].position = m_points[y-1][x+1].getPosition() + rad_offset;
                    target.draw(line, 2, sf::LinesStrip, states);
                }
            }
        for (size_t y = 0; y < t_dim; ++y)
            for (size_t x = 0; x < t_dim; ++x)
                target.draw(m_points[y][x], states);
    }
};

}

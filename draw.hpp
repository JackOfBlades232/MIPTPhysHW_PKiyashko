#pragma once
#include "utils.hpp"

#include <SFML/Graphics.hpp>

#include <cassert>

namespace draw
{

template <size_t t_num_points>
class CircularLine : public sf::Drawable {
    sf::CircleShape m_points[t_num_points];
    sf::Vertex m_line_nodes[t_num_points + 1];

public:
    CircularLine(float point_rad, sf::Color point_color) {
        for (auto &point : m_points) {
            point = sf::CircleShape(point_rad);
            point.setFillColor(point_color);
        }
    }

    void SetPositions(const sf::Vector2f *positions, size_t position_cnt) {
        assert(position_cnt == t_num_points);
        for (size_t i = 0; i < t_num_points; i++) {
            m_points[i].setPosition(positions[i]);
            m_line_nodes[i].position = positions[i] + sf::Vector2f(m_points[i].getRadius(), m_points[i].getRadius());
        }
        m_line_nodes[t_num_points].position = m_line_nodes[0].position;
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override { 
        target.draw(m_line_nodes, t_num_points + 1, sf::LinesStrip, states);
        for (auto &point : m_points)
            target.draw(point, states);
    }
};

}

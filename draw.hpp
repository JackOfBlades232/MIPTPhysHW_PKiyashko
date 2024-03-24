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

    void SetPointPosition(size_t point_id, sf::Vector2f position) {
        assert(point_id < t_num_points);
        m_points[point_id].setPosition(position);
        m_line_nodes[point_id].position = position + sf::Vector2f(m_points[point_id].getRadius(), m_points[point_id].getRadius());
        if (point_id == 0)
            m_line_nodes[t_num_points].position = m_line_nodes[point_id].position;
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override { 
        target.draw(m_line_nodes, t_num_points + 1, sf::LinesStrip, states);
        for (auto &point : m_points)
            target.draw(point, states);
    }
};

}

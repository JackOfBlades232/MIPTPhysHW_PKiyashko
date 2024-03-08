#pragma once
#include "utils.hpp"

#include <SFML/Graphics.hpp>

#include <cassert>

template <size_t t_num_segments>
class Spring : public sf::Drawable {
    float m_width, m_padding;
    sf::Vertex m_segment_points[t_num_segments + 3] = {};

public:
    Spring(float width, float padding) : m_width(width), m_padding(padding) {}

    void SetPositions(sf::Vector2f start, sf::Vector2f end) {
        float len = vlen(end - start);
        assert(len > 0.01f);

        float half_w = m_width*0.5f;
        float proj = (len - 2.f * m_padding) / ((t_num_segments - 1) * 2);

        sf::Vector2f xdir = vnormalize(end - start);
        sf::Vector2f ydir = { -xdir.y, xdir.x };
        sf::Vector2f xstep = xdir * proj;
        sf::Vector2f ystep = ydir * half_w;

        sf::Vector2f segment_positions[t_num_segments + 3];
        segment_positions[0] = start;
        segment_positions[1] = start + xdir * m_padding;
        segment_positions[2] = segment_positions[1] + xstep + ystep;
        int mul = -1;
        for (int i = 3; i < t_num_segments + 1; i++)
        {
            segment_positions[i] = segment_positions[i-1] + 2.f * xstep + 2.f * mul * ystep;
            mul *= -1;
        }
        segment_positions[t_num_segments + 1] = end - xdir * m_padding;
        segment_positions[t_num_segments + 2] = end;

        for (int i = 0; i < t_num_segments + 3; i++)
            m_segment_points[i] = sf::Vertex(segment_positions[i]);
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override { 
        target.draw(m_segment_points, t_num_segments + 3, sf::LinesStrip, states);
    }
};
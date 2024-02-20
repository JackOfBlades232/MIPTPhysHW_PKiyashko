#include <SFML/Graphics.hpp>
#include <ascent/Ascent.h>
#include <windows.h>

#include <iostream>
#include <functional>

#include <cmath>
#include <cassert>

LRESULT CALLBACK on_event(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CLOSE: 
    {
        PostQuitMessage(0);
        return 0;
    } break;
    }

    return DefWindowProc(handle, message, wParam, lParam);
}

float vlen(sf::Vector2f v) { return sqrtf(v.x*v.x + v.y*v.y); }
sf::Vector2f vnormalize(sf::Vector2f v) { return v / vlen(v); }

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

template<size_t t_dim_x, size_t t_dim_y>
class VectorField : public sf::Drawable {
    struct Arrow : public sf::Drawable {
        sf::Vector2f orig, dir;

        virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override { 
            // @TODO: settable widths/sizes, cached shapes
            float tri_h = vlen(dir)/3;
            float tri_hw = tri_h/3;
            sf::Vertex body[] = { orig, orig + dir };
            sf::Vector2f tri_tip = orig+dir;
            sf::Vector2f xstep = vnormalize(-dir);
            sf::Vector2f ystep = { -xstep.y, xstep.x };
            sf::Vertex tri[] = { tri_tip, tri_tip + xstep*tri_h + ystep*tri_hw, tri_tip + xstep*tri_h - ystep*tri_hw };

            target.draw(body, 2, sf::Lines, states);
            target.draw(tri, 3, sf::Triangles, states);
        }
    };

    Arrow m_arrows[t_dim_y][t_dim_x];

public:
    VectorField(sf::Vector2f base, sf::Vector2f step) {
        for (size_t y = 0; y < t_dim_y; y++)
            for (size_t x = 0; x < t_dim_x; x++)
                m_arrows[y][x].orig = { base.x + (float)x * step.x, base.y + (float)y * step.y };
    }

    void SetArrowDirections(std::function<sf::Vector2f(sf::Vector2f)> field_func) {
        for (size_t y = 0; y < t_dim_y; y++)
            for (size_t x = 0; x < t_dim_x; x++)
                m_arrows[y][x].dir = field_func(m_arrows[y][x].orig);
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override { 
        for (size_t y = 0; y < t_dim_y; y++)
            for (size_t x = 0; x < t_dim_x; x++)
                target.draw(m_arrows[y][x], states);
    }

};

int main()
{
    HINSTANCE instance = GetModuleHandle(NULL);

    WNDCLASS windowClass;
    windowClass.style = 0;
    windowClass.lpfnWndProc = &on_event;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = instance;
    windowClass.hIcon = NULL;
    windowClass.hCursor = 0;
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BACKGROUND);
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = TEXT("Phys HW");
    RegisterClass(&windowClass);

    HWND window = CreateWindow(TEXT("Phys HW"), TEXT("Phys HW"), WS_SYSMENU | WS_VISIBLE, 0, 0, 1920, 1080, NULL, NULL, instance, NULL);

    sf::RenderWindow view(window);

    // @TODO: test spring

    sf::CircleShape body(35);
    body.setFillColor(sf::Color::Black);
    body.setOutlineColor(sf::Color::White);
    body.setOutlineThickness(-4);

    Spring<30> spring(25.f, 50.f);
    sf::Vector2f spring_anchor = sf::Vector2f(960, 220);

    //VectorField<1920/50, 1080/50> vf({ 25.f, 25.f }, { 50.f, 50.f });

    sf::Clock clock;

    MSG message;
    message.message = static_cast<UINT>(~WM_QUIT);
    while (message.message != WM_QUIT) {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        } else {
            float time = clock.getElapsedTime().asSeconds();

            sf::Vector2f ball_pos = sf::Vector2f(960, 540) + 100.f * sf::Vector2f(sinf(time), cosf(time));
            sf::Vector2f ball_center = ball_pos + sf::Vector2f(body.getRadius(), body.getRadius());
            sf::Vector2f ball_anchor = ball_center + vnormalize(spring_anchor - ball_center) * body.getRadius();
            auto field_func = [&time](sf::Vector2f v) -> sf::Vector2f { return 25.f * sf::Vector2f(sinf(3.f*time), cosf(3.f*time)); };

            view.clear();

            body.setPosition(ball_pos);
            spring.SetPositions(spring_anchor, ball_anchor);
            //vf.SetArrowDirections(field_func);

            //view.draw(vf);
            view.draw(spring);
            view.draw(body);

            view.display();
        }
    }

    view.close();
    DestroyWindow(window);

    UnregisterClass(TEXT("Phys HW"), instance);

    return EXIT_SUCCESS;
}

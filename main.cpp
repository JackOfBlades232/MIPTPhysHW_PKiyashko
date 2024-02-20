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


    asc::state_t state;
    state.reserve(100);

    asc::Param pos_x(state), pos_y(state);
    asc::Param vel_x(state), vel_y(state);

    double k = 2000.0;
    double d = 5.0;
    double g = 10.0;
    double m = 1.0;

    double l0 = 200.0;

    pos_x = 960;
    pos_y = 540;
    vel_x = 0;
    vel_y = 0;

    asc::RK4 integrator;

    // @TODO:  finish this as a test, port back and do the analytic with linearization
    auto system = [&](const asc::state_t &x, asc::state_t &D, const double t) {
            
        };


    sf::Clock clock;
    float prev_time = 0.f;

    MSG message;
    message.message = static_cast<UINT>(~WM_QUIT);
    while (message.message != WM_QUIT) {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        } else {
            float time = clock.getElapsedTime().asSeconds();
            float dt = time - prev_time;
            prev_time = time;



            sf::Vector2f ball_center = ball_pos + sf::Vector2f(body.getRadius(), body.getRadius());
            sf::Vector2f ball_anchor = ball_center + vnormalize(spring_anchor - ball_center) * body.getRadius();
            auto field_func = [&time](sf::Vector2f v) -> sf::Vector2f { return 25.f * sf::Vector2f(sinf(3.f*time), cosf(3.f*time)); };

            view.clear();

            body.setPosition(ball_pos);
            spring.SetPositions(spring_anchor, ball_anchor);

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

#include "win.hpp"
#include "sim.hpp"

#include <SFML/Graphics.hpp>
#include <ascent/Ascent.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include <iostream>
#include <functional>

#include <cmath>
#include <cassert>

static input_t input = {};

LRESULT CALLBACK on_event(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CLOSE: 
    {
        PostQuitMessage(0);
        return 0;
    } break;

    case WM_MOUSEMOVE:
    {
        input.mouse_x = GET_X_LPARAM(lParam);
        input.mouse_y = GET_Y_LPARAM(lParam);
    } break;

    case WM_LBUTTONDOWN:
    {
        input.mouse_down = true;
    } break;

    case WM_LBUTTONUP:
    {
        if (input.mouse_down)
            input.mouse_clicked = true;
        input.mouse_down = false;
    }

    default:
        break;
    }

    return DefWindowProc(handle, message, wParam, lParam);
}

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

    sim::init();

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

            sim::update(dt, input);
            input.mouse_clicked = false; // @TODO(PKiyashko): remake input with SFML

            sim::draw(view);
        }
    }

    view.close();
    DestroyWindow(window);

    UnregisterClass(TEXT("Phys HW"), instance);

    return EXIT_SUCCESS;
}

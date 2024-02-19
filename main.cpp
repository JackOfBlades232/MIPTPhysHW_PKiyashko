#include <Eigen/Dense>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <windows.h>
#include <cmath>

HWND button;


LRESULT CALLBACK on_event(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CLOSE: 
    {
        PostQuitMessage(0);
        return 0;
    } break;

    case WM_COMMAND: 
    {
        if (reinterpret_cast<HWND>(lParam) == button) {
            PostQuitMessage(0);
            return 0;
        }
    } break;
    }

    return DefWindowProc(handle, message, wParam, lParam);
}


int main()
{
    // Eigen test
    Eigen::MatrixXd m(2, 2);
    m(0, 0) = 3;
    m(1, 0) = 2.5;
    m(0, 1) = -1;
    m(1, 1) = m(1, 0) + m(0, 1);
    std::cout << m << std::endl;

    // SFML test
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
    windowClass.lpszClassName = TEXT("SFML App");
    RegisterClass(&windowClass);

    HWND window = CreateWindow(TEXT("SFML App"), TEXT("SFML Win32"), WS_SYSMENU | WS_VISIBLE, 200, 200, 660, 520, NULL, NULL, instance, NULL);

    button = CreateWindow(TEXT("BUTTON"), TEXT("Quit"), WS_CHILD | WS_VISIBLE, 560, 440, 80, 40, window, NULL, instance, NULL);

    HWND view1 = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 20, 20, 300, 400, window, NULL, instance, NULL);
    HWND view2 = CreateWindow(TEXT("STATIC"), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 340, 20, 300, 400, window, NULL, instance, NULL);
    sf::RenderWindow SFMLView1(view1);
    sf::RenderWindow SFMLView2(view2);

    sf::Texture texture1, texture2;
    if (!texture1.loadFromFile("Assets/image1.jpg") || !texture2.loadFromFile("Assets/image2.jpg"))
        return EXIT_FAILURE;
    sf::Sprite sprite1(texture1);
    sf::Sprite sprite2(texture2);
    sprite1.setOrigin(sf::Vector2f(texture1.getSize()) / 2.f);
    sprite1.setPosition(sprite1.getOrigin());

    sf::Clock clock;

    MSG message;
    message.message = static_cast<UINT>(~WM_QUIT);
    while (message.message != WM_QUIT) {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessage(&message);
        } else {
            float time = clock.getElapsedTime().asSeconds();

            SFMLView1.clear();
            SFMLView2.clear();

            sprite1.setRotation(time * 100);
            SFMLView1.draw(sprite1);

            sprite2.setPosition(std::cos(time) * 100.f, 0.f);
            SFMLView2.draw(sprite2);

            SFMLView1.display();
            SFMLView2.display();
        }
    }

    SFMLView1.close();
    SFMLView2.close();

    DestroyWindow(window);

    UnregisterClass(TEXT("SFML App"), instance);

    return EXIT_SUCCESS;
}

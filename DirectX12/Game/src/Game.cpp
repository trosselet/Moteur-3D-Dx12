#include "pch.h"

using namespace Engine;

int main()
{
    Engine::TWindow window(Vector2i(1920, 1080), "Window");

    Engine::Event event;

    while (window.IsOpen())
    {
        while (window.PollEvent(event))
        {
            if (event.type == Engine::Event::Type::Closed)
            {
                window.Close();
            }
        }

    }

    return 0;
}
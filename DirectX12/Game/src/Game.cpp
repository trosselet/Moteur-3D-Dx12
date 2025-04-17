#include "pch.h"

using namespace Engine;

int main()
{
    Engine::TWindow window(Vector2i(800, 600), "Test");

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
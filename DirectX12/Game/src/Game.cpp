#include "pch.h"

#include "TWindow.h"
#include "TColor.h"

int main()
{
    Engine::TWindow window(Vector2i(800, 600), "Window");

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

        window.Clear(Render::Color::Green);
        window.Draw();
        window.Display();

    }

    return 0;
}
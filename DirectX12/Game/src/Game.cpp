#include "pch.h"

#include "TWindow.h"
#include "../../Core/include/TColor.h"
#include "TRectangle.h"
#include "Game.h"

int main()
{
    Engine::TWindow window(Vector2i(800, 600), "Window");

    Engine::Event event;

    Render::RectangleShape* rectangle = new Render::RectangleShape();

    while (window.IsOpen())
    {
        while (window.PollEvent(event))
        {
            if (event.type == Engine::Event::Type::Closed)
            {
                window.Close();
            }
        }

        window.Clear(Color::Green);
        window.Draw(*rectangle);
        window.Display();

    }

    delete rectangle;

    return 0;
}
#include "pch.h"
#include "Game.h"


#include "TWindow.h"
#include "TRectangle.h"
#include "TCircle.h"
#include "TCube.h"

int main()
{
    Engine::TWindow window(Vector2i(800, 600), "Window");

    Engine::Event event;

    Render::RectangleShape* rectangle = new Render::RectangleShape();
    Render::CircleShape* circle = new Render::CircleShape(Vector3f(-2.0f, 0.0f, 0.0f), 1.0f, Color::Red);
    Render::CubeShape* cube = new Render::CubeShape(Vector3f(1.0f, 0.0f, 0.0f), 1.0f, Color::Green);

    float angle = 0;

    while (window.IsOpen())
    {
        while (window.PollEvent(event))
        {
            if (event.type == Engine::Event::Type::Closed)
            {
                window.Close();
            }
        }

        bool Z = GetAsyncKeyState('Z') < 0;
        bool Q = GetAsyncKeyState('Q') < 0;
        bool S = GetAsyncKeyState('S') < 0;
        bool D = GetAsyncKeyState('D') < 0;

        bool A = GetAsyncKeyState('A') < 0;
        bool E = GetAsyncKeyState('E') < 0;

        Vector3f forward = window.camera->Forward();
        Vector3f right = window.camera->Right();

        if (Z) 
        {
            window.camera->Translate(Vector3f{ forward.x * 0.05f, 0, forward.z * 0.05f });
        }
        if (Q) 
        {
            window.camera->Translate(Vector3f{ -right.x * 0.05f, 0, -right.z * 0.05f });
        }
        if (S) 
        {
            window.camera->Translate(Vector3f{ -forward.x * 0.05f, 0, -forward.z * 0.05f });
        }
        if (D) 
        {
            window.camera->Translate(Vector3f{ right.x * 0.05f, 0, right.z * 0.05f });
        }

        if (A) 
        {
            window.camera->RotateYPR(Vector3f(-0.01f, 0.0f, 0.0f));
        }
        if (E) 
        {
            window.camera->RotateYPR(Vector3f(0.01f, 0.0f, 0.0f));
        }
        

        //rectangle->RotateZ(0.1f);
        

        window.Clear(Color::Black);
        window.Draw(*rectangle);
        window.Draw(*circle);
        window.Draw(*cube);
        window.Display();

    }

    delete cube;
    delete circle;
    delete rectangle;

    return 0;
}
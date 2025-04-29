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
    Render::CircleShape* circle = new Render::CircleShape(Vector3f(-1.5f, 0.0f, 0.0f), 1.0f, Color::Red);
    Render::CubeShape* cube = new Render::CubeShape(Vector3f(1.0f, 0.0f, 0.0f), 1.0f, Color::Red);

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

        bool LEFT = GetAsyncKeyState(VK_LEFT) < 0;
        bool RIGHT = GetAsyncKeyState(VK_RIGHT) < 0;

        Vector3f forward = window.camera->Forward();
        Vector3f right = window.camera->Right();
        Vector3f up = window.camera->Up();

        if (Z) 
        {
            window.camera->Translate(Vector3f{ forward.x * 0.05f, forward.y * 0.05f, forward.z * 0.05f });
        }
        if (Q) 
        {
            window.camera->Translate(Vector3f{ -right.x * 0.05f, -right.y * 0.05f, -right.z * 0.05f });
        }
        if (S) 
        {
            window.camera->Translate(Vector3f{ -forward.x * 0.05f, -forward.y * 0.05f, -forward.z * 0.05f });
        }
        if (D) 
        {
            window.camera->Translate(Vector3f{ right.x * 0.05f, right.y * 0.05f, right.z * 0.05f });
        }
        if (A) 
        {
            window.camera->Translate(Vector3f{ 0, -up.y * 0.05f, 0 });
        }
        if (E) 
        {
            window.camera->Translate(Vector3f{ 0, up.y * 0.05f, 0 });
        }

        if (LEFT)
        {
            window.camera->RotateY(-0.01f);
        }
        if (RIGHT)
        {
            window.camera->RotateY(0.01f);
        }
        

        //rectangle->RotateZ(0.1f);
        //circle->RotateY(0.1f);
        //cube->RotateX(0.1f);

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
#include "pch.h"
#include "Game.h"


#include "TWindow.h"
#include "TRectangle.h"

int main()
{
    Engine::TWindow window(Vector2i(800, 600), "Window");

    Engine::Event event;

    Render::RectangleShape* rectangle = new Render::RectangleShape();

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

        if (Z) {
            window.camera->Translate(Vector3f{ forward.x * 0.05f, 0, forward.z * 0.05f });
        }
        if (Q) {
            window.camera->Translate(Vector3f{ -right.x * 0.05f, 0, -right.z * 0.05f });
        }
        if (S) {
            window.camera->Translate(Vector3f{ -forward.x * 0.05f, 0, -forward.z * 0.05f });
        }
        if (D) {
            window.camera->Translate(Vector3f{ right.x * 0.05f, 0, right.z * 0.05f });
        }

        if (A) {
            angle -= 0.01f;
        }
        if (E) {
            angle += 0.01f;
        }
        window.camera->SetRotationX(angle);

        //rectangle->RotateZ(0.1f);

        window.Clear(Color::Black);
        window.Draw(*rectangle);
        window.Display();

    }

    delete rectangle;

    return 0;
}
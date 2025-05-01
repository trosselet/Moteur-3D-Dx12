#include "pch.h"
#include "Game.h"


#include "TWindow.h"
#include "TRectangle.h"
#include "TCircle.h"
#include "TCube.h"
#include "TSphere.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Engine::TWindow window(hInstance, Vector2i(800, 600), "Window");

    Engine::Event event;

    Render::CubeShape* baseCube = new Render::CubeShape(Vector3f(0.0f, -1.0f, 0.0f), 1.0f, Color::White, L"texture/grid_placeholder_material.dds");
    baseCube->SetScale(Vector3f{ 1.5f, 0.5f, 1.5f });

    Render::SphereShape* skyBox = new Render::SphereShape(Vector3f(0.0f, 0.0f, 0.0f), -490.0f, Color::White, L"texture/sky.dds");

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

        window.Clear(Color::Black);
        window.Draw(*skyBox);
        window.Draw(*baseCube);
        window.Display();

    }

    delete skyBox;
    delete baseCube;

    return 0;
}
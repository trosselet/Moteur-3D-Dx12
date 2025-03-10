#include "pch.h"
#include "main.h"
#include "AppWindow.h"

using namespace Engine;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
    AppWindow app;

    /*if (app.Initialize(1280, 720))*/
    if (app.Initialize(1920, 1080))
    {

        while (app.IsRunning())
        {
            app.Update();
        }
    }

    return 0;
}
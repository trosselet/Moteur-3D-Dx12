#include "pch.h"
#include "AppWindow.h"
#pragma comment(lib, "winmm.lib")

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_NCCREATE:
    {
        LPCREATESTRUCT param = reinterpret_cast<LPCREATESTRUCT>(lparam);
        Engine::AppWindow* pointer = reinterpret_cast<Engine::AppWindow*>(param->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pointer));
        PRINT_CONSOLE_OUTPUT("Sent create message" << std::endl);
        break;
    }
    case WM_CREATE: 
    {
        Engine::AppWindow* pointer = reinterpret_cast<Engine::AppWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        pointer->OnCreate(hwnd);
        break;
    }
    case WM_DESTROY: 
    {
        Engine::AppWindow* pointer = reinterpret_cast<Engine::AppWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        pointer->OnDestroy();
        PostQuitMessage(0);
        break;
    }

    default :
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

Engine::AppWindow::~AppWindow()
{
    
}

bool Engine::AppWindow::Initialize(int width, int height)
{
    mWidth = width;
    mHeight = height;

    WNDCLASS wc;
    wc.style = NULL;
    wc.lpfnWndProc = &WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = NULL;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"MyWindowClass";

    if (!RegisterClass(&wc))
        return false;

    mHwnd = CreateWindow(L"MyWindowClass", L"DirectXGameEngine", WS_OVERLAPPEDWINDOW, 0, 0, mWidth, mHeight, 0, 0, 0, this);

    if (!mHwnd)
        return false;

    ShowWindow(mHwnd, SW_SHOW);
    UpdateWindow(mHwnd);

    mIsRunning = true;

    return mIsRunning;
}

void Engine::AppWindow::OnCreate(HWND hwnd)
{
    PRINT_CONSOLE_OUTPUT("Created the window" << std::endl);
    mpGraphicEngine->Get()->Initialize(hwnd, mWidth, mHeight);
    mECSManager->Get()->Initialize();
    scene.Initialize();
}


void Engine::AppWindow::Update()
{
    MSG message;

    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    if (mIsRunning)
    {
        if (UpdateTime())
        {
            SetDeltaTime(m_elapsedTime);

            scene.Update(GetDeltaTime());
            mpGraphicEngine->Get()->UpdateDraw();
            mECSManager->Get()->Update();

            mAccumulatedDt += GetDeltaTime();
            while (mAccumulatedDt >= FIXED_DT)
            {
                FixedUpdate();
                mAccumulatedDt -= FIXED_DT;
            }

        }
    }

}

void Engine::AppWindow::FixedUpdate()
{
    scene.FixedUpdate(GetDeltaTime());
}

bool Engine::AppWindow::IsRunning()
{
    return mIsRunning;
}

bool Engine::AppWindow::UpdateTime()
{
    static DWORD previousTime = timeGetTime();
    DWORD currentTime = timeGetTime();
    DWORD elapsedSysTime = currentTime - previousTime;

    if (elapsedSysTime == 0)
        return false;

    m_elapsedTime = elapsedSysTime / 1000.0f;

    float maxDeltaTime = 0.04f;

    if (m_elapsedTime > maxDeltaTime)
    {
        m_elapsedTime = maxDeltaTime;
    }

    previousTime = currentTime;
    m_time += m_elapsedTime;
    return true;
}

void Engine::AppWindow::OnDestroy()
{
    mIsRunning = false;
    PRINT_CONSOLE_OUTPUT("Closed the window" << std::endl);
}
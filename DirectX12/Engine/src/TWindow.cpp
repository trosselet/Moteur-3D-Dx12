#include "pch.h"
#include "TWindow.h"

Engine::TWindow::TWindow() : m_windowSize(0, 0), m_windowTitle(""), m_windowStyle(0), m_isRunning(false)
{
}

Engine::TWindow::TWindow(Vector2i windowSize, const Int8* windowTitle, Uint32 style)
{
    m_windowSize = windowSize;
    m_windowTitle = windowTitle;
    m_windowStyle = style;
    Initialize();
}

Engine::TWindow::~TWindow()
{
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_NCCREATE:
    {
        LPCREATESTRUCT param = reinterpret_cast<LPCREATESTRUCT>(lparam);
        Engine::TWindow* pointer = reinterpret_cast<Engine::TWindow*>(param->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pointer));
        break;
    }
    case WM_DESTROY:
    {
        Engine::TWindow* pointer = reinterpret_cast<Engine::TWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        pointer->OnDestroy();
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool Engine::TWindow::Initialize()
{
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

    m_hwnd = CreateWindow(L"MyWindowClass", Utils::GetWchar(m_windowTitle) , WS_OVERLAPPEDWINDOW, 0, 0, m_windowSize.x, m_windowSize.y, 0, 0, 0, this);

    if (!m_hwnd)
        return false;

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    m_isRunning = true;

    return m_isRunning;
}

void Engine::TWindow::OnDestroy()
{
    m_isRunning = false;
}

bool Engine::TWindow::PollEvent(Event& event)
{
    MSG message;
    if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        switch (message.message)
        {
        case WM_QUIT:
        case WM_CLOSE:
        case WM_DESTROY:
        {
            event.type = Event::Type::Closed;
            return true;
        }
        default:
        {
            event.type = Event::Type::None;
            return false;
        }
        }
    }
}

void Engine::TWindow::Close()
{
    if (m_hwnd)
    {
        CloseWindow(m_hwnd);

        std::cout << "Windows closed by user\n";

        m_isRunning = false;

    }
}

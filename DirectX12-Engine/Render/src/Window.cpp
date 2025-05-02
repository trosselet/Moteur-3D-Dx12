#include "pch.h"
#include "Window.h"
#include "GraphicEngine.h"

const wchar_t* GetWchar(const int8* c)
{
    size_t charSize = strlen((const char*)c) + 1;
    wchar_t* wc = new wchar_t[charSize];

    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, wc, charSize, (const char*)c, _TRUNCATE);

    return wc;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_NCCREATE:
    {
        LPCREATESTRUCT param = reinterpret_cast<LPCREATESTRUCT>(lparam);
        Window* pointer = reinterpret_cast<Window*>(param->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pointer));
        break;
    }
    case WM_CLOSE:
    {
        Window* pointer = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        pointer->Close();
        return 0;
    }
    case WM_SIZE:
    {
        Window* pointer = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (pointer && pointer->GetGraphics() && pointer->GetGraphics()->IsInit())
        {
            int width = LOWORD(lparam);
            int height = HIWORD(lparam);

            if (width < 8)
            {
                width = 8;
            }
            if (height < 8)
            {
                height = 8;
            }

            pointer->GetGraphics()->ResizeWindow(width, height);
        }
        break;
    }
    case WM_DESTROY:
    {
        Window* pointer = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        pointer->OnDestroy();
        PostQuitMessage(0);
        break;
    }

    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

Window::Window(HINSTANCE hInstance, Vector2i windowSize, const int8* windowTitle)
{
    m_windowSize = windowSize;
    m_windowTitle = windowTitle;
    m_hInstance = hInstance;
    Initialize();
    m_pGraphicEngine = new GraphicEngine(this);
}

Window::~Window()
{
    delete m_pGraphicEngine;
}

void Window::Close()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
    }
}

void Window::OnDestroy()
{
    m_isRunning = false;
}

bool Window::Initialize()
{
    WNDCLASS wc = {};
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = &WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = m_hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"MyWindowClass";

    if (!RegisterClass(&wc))
    {
        MessageBox(nullptr, L"RegisterClass Failed.", nullptr, 0);
        assert(false);
    }

    RECT rect = RECT(0, 0, m_windowSize.x, m_windowSize.y);

    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    int32 const rectWidth = rect.right - rect.left;
    int32 const rectHeight = rect.bottom - rect.top;

    m_windowSize.x = rectWidth;
    m_windowSize.y = rectHeight;

    m_hwnd = CreateWindow(wc.lpszClassName, GetWchar(m_windowTitle), WS_OVERLAPPEDWINDOW, 0, 0, m_windowSize.x, m_windowSize.y, 0, 0, m_hInstance, this);

    if (m_hwnd == nullptr)
    {
        MessageBox(nullptr, L"CreateWindow Failed.", nullptr, 0);
        assert(false);
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    m_isRunning = true;

    return m_isRunning;
}

void Window::Update()
{
    MSG msg = { nullptr };

    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        if (msg.message == WM_QUIT)
            return;
        DispatchMessage(&msg);
        return;
    }
}
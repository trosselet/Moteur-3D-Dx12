#include "pch.h"
#include "TWindow.h"

#include "GraphicEngine.h"
#include "TDrawable.h"


Engine::TWindow::TWindow() : m_windowSize(0, 0), m_windowTitle(""), m_windowStyle(0), m_isRunning(false), m_pGraphicEngine(new Render::GraphicEngine())
{
}

Engine::TWindow::TWindow(HINSTANCE hInstance, Vector2i windowSize, const int8* windowTitle, uint32 style)
{
    m_windowSize = windowSize;
    m_windowTitle = windowTitle;
    m_windowStyle = style;
    m_hInstance = hInstance;
    m_pGraphicEngine = new Render::GraphicEngine();
    Initialize();
}

Engine::TWindow::~TWindow()
{
    if (camera)
    {
        delete camera;
    }

    if (m_pGraphicEngine)
    {
        delete m_pGraphicEngine;
    }

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
    case WM_CLOSE:
    {
        PostMessage(hwnd, WM_APP + 1, 0, 0);
        return 0;
    }
    case WM_SIZE:
    {
        Engine::TWindow* pointer = reinterpret_cast<Engine::TWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        PostMessage(hwnd, WM_APP + 2, 0, 0);
        if (pointer && pointer->GetGraphicEngine() && pointer->GetGraphicEngine()->IsInitialize())
        {
            int width = LOWORD(lparam);
            int height = HIWORD(lparam);
            pointer->GetGraphicEngine()->ResizeWindow(width, height);
        }
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
    WNDCLASS wc = {};
    wc.style = NULL;
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
        return false;

    m_hwnd = CreateWindow(L"MyWindowClass", Utils::GetWchar(m_windowTitle) , WS_OVERLAPPEDWINDOW, 0, 0, m_windowSize.x, m_windowSize.y, 0, 0, m_hInstance, this);

    if (!m_hwnd)
        return false;

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    camera = new Camera();

    m_pGraphicEngine->Initialize(m_hwnd, m_windowSize.x, m_windowSize.y, &camera->GetTransform());

    m_pGraphicEngine->SetViewport(m_windowSize.x, m_windowSize.y);

    m_isRunning = true;

    return m_isRunning;
}

void Engine::TWindow::OnDestroy()
{
    m_isRunning = false;
}

void Engine::TWindow::Clear(Color clearColor)
{
    m_pGraphicEngine->BeginFrame(clearColor);
}

void Engine::TWindow::Draw(Render::Shape& shape, const char* shaderPath)
{
    m_pGraphicEngine->RenderFrame(shape, shaderPath);
}

void Engine::TWindow::Draw(Mesh* pMesh, const char* shaderPath)
{

}

void Engine::TWindow::Display()
{
    m_pGraphicEngine->EndFrame();
}

bool Engine::TWindow::PollEvent(Event& event)
{
    m_pGraphicEngine->Update();

    MSG message;
    if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
    {
        

        switch (message.message)
        {
        case WM_APP + 1:
        {
            event.type = Event::Type::Closed;
            return true;
        }
        case WM_APP + 2:
        {
            event.type = Event::Type::Resized;
            return true;
        }
        default:
        {
            event.type = Event::Type::None;
            TranslateMessage(&message);
            DispatchMessage(&message);
            return true;
        }
        }
    }

    return false;
}

void Engine::TWindow::Close()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);

        PRINT_CONSOLE_OUTPUT("Windows closed by user\n");
    }
}

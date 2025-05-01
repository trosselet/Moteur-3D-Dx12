#include "Window.h"
#ifndef WINDOW__INL
#define WINDOW__INL


inline bool Window::IsOpen() const
{
	return m_isRunning;
}

inline uint32 Window::GetWidth() const
{
	return m_windowSize.x;
}

inline uint32 Window::GetHeight() const
{
	return m_windowSize.y;
}

inline HINSTANCE Window::GetHInstance() const
{
	return m_hInstance;
}

inline HWND Window::GetHWND() const
{
	return m_hwnd;
}

inline GraphicEngine* Window::GetGraphics()
{
	return m_pGraphicEngine;
}

#endif // !WINDOW__INL

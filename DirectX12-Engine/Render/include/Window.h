#ifndef TWINDOW__H
#define TWINDOW__H

#include "Define.h"
#include "Vector.hpp"
#include "Color.h"


class GraphicEngine;

class Window
{
public:

	Window(HINSTANCE hInstance, Vector2i windowSize, const int8* windowTitle = "Window");
	~Window();

	bool IsOpen() const;
	uint32 GetWidth() const;
	uint32 GetHeight() const;
	HINSTANCE GetHInstance() const;
	HWND GetHWND() const;

	GraphicEngine* GetGraphics();

	void Update();

	void Close();
	void OnDestroy();

private:
	bool Initialize();

private:
	inline static Window* s_pInstance = nullptr;

	//Window Initialisation
	Vector2i m_windowSize;
	const int8* m_windowTitle;

	//Window param
	bool m_isRunning;

	HWND m_hwnd;
	HINSTANCE m_hInstance;

	//Graphics
	GraphicEngine* m_pGraphicEngine;

	friend class GraphicEngine;
};

#include "Window.inl"

#endif // !TWINDOW__H
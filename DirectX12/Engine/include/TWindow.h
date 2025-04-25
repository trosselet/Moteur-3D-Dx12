#ifndef TWINDOW__H
#define TWINDOW__H

#include "../../Core/include/Define.h"
#include "../../Core/include/TVector.hpp"
#include "../../Core/include/TColor.h"

#include "TWindowStyle.h"
#include "TEvent.h"
#include "Utils.h"


namespace Core
{
	class IDrawable;
}

namespace Render
{
	class GraphicEgine;
}

namespace Engine
{
	class TWindow
	{
	public:
		

		TWindow();
		TWindow(Vector2i windowSize, const int8* windowTitle = "Window", int16 style = WindowStyle::Default);
		~TWindow();

		inline bool IsOpen() const { return m_isRunning; };

		bool PollEvent(Event& event);

		void Close();
		void OnDestroy();

		void Clear(Color clearColor = Color(1.0f, 1.0f, 1.0f, 1.0f));
		void Draw(Core::IDrawable& drawable, const char* shaderPath = "../Game/shader/DefaultShader.hlsl");
		void Display();

		Render::GraphicEgine* GetGraphicEngine() { return m_pGraphicEngine; };

	private:
		bool Initialize();

	private:
		//Window Initialisation
		Vector2i m_windowSize;
		const int8* m_windowTitle;
		int16 m_windowStyle;

		//Window param
		bool m_isRunning;
		HWND m_hwnd;

		//Graphics
		Render::GraphicEgine* m_pGraphicEngine;

	};
}

#endif // !TWINDOW__H
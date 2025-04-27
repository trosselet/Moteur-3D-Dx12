#ifndef TWINDOW__H
#define TWINDOW__H

#include "../../Core/include/Define.h"
#include "../../Core/include/TVector.hpp"
#include "../../Core/include/TColor.h"

#include "../../Core/include/TTransform.h"

#include "TWindowStyle.h"
#include "TEvent.h"
#include "Utils.h"
#include "TCamera.h"

namespace Render
{
	class GraphicEngine;
	class Shape;
}

namespace Engine
{

	class TWindow
	{
	public:
		

		TWindow();
		TWindow(Vector2i windowSize, const int8* windowTitle = "Window", uint32 style = WindowStyle::Default);
		~TWindow();

		inline bool IsOpen() const { return m_isRunning; };

		bool PollEvent(Event& event);

		void Close();
		void OnDestroy();

		void Clear(Color clearColor = Color(1.0f, 1.0f, 1.0f, 1.0f));
		void Draw(Render::Shape& shape, const char* shaderPath = "../Game/shader/DefaultShader.hlsl");
		void Display();

		Render::GraphicEngine* GetGraphicEngine() { return m_pGraphicEngine; };

	public:
		Camera* camera;

	private:
		bool Initialize();

	private:
		//Window Initialisation
		Vector2i m_windowSize;
		const int8* m_windowTitle;
		uint32 m_windowStyle;

		//Window param
		bool m_isRunning;
		HWND m_hwnd;

		//Graphics
		Render::GraphicEngine* m_pGraphicEngine;

	};
}

#endif // !TWINDOW__H
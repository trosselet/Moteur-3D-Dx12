#ifndef TWINDOW__H
#define TWINDOW__H

#include "TVector2.h"
#include "TWindowStyle.h"
#include "TEvent.h"
#include "Utils.h"

namespace Engine
{
	class TWindow
	{
	public:

		TWindow();
		TWindow(Vector2i windowSize, const Int8* windowTitle = "Window", Uint32 style = WindowStyle::Default);
		~TWindow();

		inline bool IsOpen() const { return m_isRunning; };

		bool PollEvent(Event& event);

		void Close();
		void OnDestroy();

	private:
		bool Initialize();

	private:
		//Window Initialisation
		Vector2i m_windowSize;
		const Int8* m_windowTitle;
		Uint32 m_windowStyle;

		//Window param
		bool m_isRunning;
		HWND m_hwnd;
	};
}

#endif // !TWINDOW__H
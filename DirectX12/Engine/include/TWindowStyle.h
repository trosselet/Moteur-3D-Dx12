#ifndef T_WINDOW_STYLE__H
#define T_WINDOW_STYLE__H

namespace Engine
{
	namespace WindowStyle
	{
		enum 
		{
			None		= 0,
			TitleBar	= 1 << 0,
			Resize		= 1 << 1,
			Close		= 1 << 2,
			FullScreen	= 1 << 3,
			Default		= TitleBar | Resize | Close,
		};
	}
}


#endif // !T_WINDOW_STYLE__H

#ifndef RENDERSYSTEM__H
#define RENDERSYSTEM__H

#include <array>
#include <vector>

#include "Define.h"

class Render;
class GraphicEngine;


namespace Engine
{
	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;


		void HandleRendering();

		GraphicEngine* m_pGraphics;
		Render* m_pRender;

	private:

		friend class Scene;
		friend class GameManager;

	};
}


#endif // !RENDERSYSTEM__H
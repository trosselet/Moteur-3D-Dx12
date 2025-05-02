#ifndef RENDERSYSTEM__H
#define RENDERSYSTEM__H

#include <array>
#include <vector>

#include "Define.h"
#include "PipelineStateObjectManager.h"

class Render;
class GraphicEngine;

namespace Engine { struct MeshRenderer; };

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

		std::array<std::vector<MeshRenderer const*>, 16> m_meshRenderers{};

		PipelineStateObjectManager::PipelineStateConfig* GetShader(int8 const* path);

	private:

		friend class Scene;
		friend class GameManager;

	};
}


#endif // !RENDERSYSTEM__H
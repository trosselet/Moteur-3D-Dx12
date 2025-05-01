#include "pch.h"
#include "Systems/RenderSystem.h"

#include "Render.h"
#include "GraphicEngine.h"

#include "GameObject.h"
#include "GameManager.h"

namespace Engine
{
	void RenderSystem::HandleRendering()
	{
		m_pGraphics->BeginFrame();

		m_pGraphics->EndFrame();
	}

}
#include "pch.h"
#include "Systems/RenderSystem.h"

#include "Render.h"
#include "GraphicEngine.h"

#include "GameObject.h"
#include "GameManager.h"


#include "Components/Camera.h"
#include "Components/MeshRenderer.h"

namespace Engine
{
	void RenderSystem::HandleRendering()
	{
		m_pGraphics->BeginFrame();

		GameObject* pCamera = GameManager::GetActiveScene().GetMainCamera();
		Camera& cameraComponent = pCamera->GetComponent<Camera>();
		m_pGraphics->UpdateCameraTo(
			Vector3f{ pCamera->m_pTransform->GetPositionFLOAT().x, pCamera->m_pTransform->GetPositionFLOAT().y, pCamera->m_pTransform->GetPositionFLOAT().z },
			Vector3f{ pCamera->m_pTransform->Forward().x, pCamera->m_pTransform->Forward().y, pCamera->m_pTransform->Forward().z },
			Vector3f{ pCamera->m_pTransform->Up().x, pCamera->m_pTransform->Up().y, pCamera->m_pTransform->Up().z },
			cameraComponent.viewWidth,
			cameraComponent.viewHeight,
			cameraComponent.fov,
			cameraComponent.nearZ,
			cameraComponent.farZ,
			cameraComponent.projectionMatrix,
			cameraComponent.viewMatrix
		);


		for (std::vector<MeshRenderer const*> const& meshRendererLayer : m_meshRenderers)
		{
			for (MeshRenderer const* const pMeshRenderer : meshRendererLayer)
			{
				if (pMeshRenderer->m_active)
					m_pGraphics->RenderFrame(pMeshRenderer->m_pMesh, pMeshRenderer->m_pMaterial, pMeshRenderer->m_pOwner->m_pTransform->GetMatrixFLOAT());
			}
		}

		m_pGraphics->EndFrame();
	}

	PipelineStateObjectManager::PipelineStateConfig* RenderSystem::GetShader(int8 const* path)
	{
		return m_pGraphics->GetRender()->GetPSOManager()->Get(path);
	}

}
#include "pch.h"
#include "Components/MeshRenderer.h"
#include "Systems/RenderSystem.h"

#include "PrimitiveFactory.h"
#include "Geometry.h"
#include "Mesh.h"
#include "Material.h"

#include "GraphicEngine.h"

#include "Window.h"
#include "GameManager.h"

namespace Engine
{
	void MeshRenderer::SetRectangle()
	{
		Free();
		GraphicEngine& graphics = *GameManager::GetWindow().GetGraphics();
		
		m_pGeometry = graphics.CreatePrimitiveGeometry(SQUARE);

		
		m_pShader = GameManager::GetRenderSystem().GetShader("../Gameplay/shader/DefaultShader.hlsl");
		Texture* pTexture = graphics.CreateTexture("../Gameplay/texture/DefaultTex.dds");

		m_pMesh = graphics.CreateMesh(m_pGeometry);
		m_pMaterial = graphics.CreateMaterial(m_pShader);
		m_pMaterial->SetTexture(pTexture);

		m_primitive = true;

	}

	void MeshRenderer::SetRectangle(const char* texturePath)
	{
		Free();
		GraphicEngine& graphics = *GameManager::GetWindow().GetGraphics();

		m_pGeometry = graphics.CreatePrimitiveGeometry(SQUARE);


		m_pShader = GameManager::GetRenderSystem().GetShader("../Gameplay/shader/DefaultShader.hlsl");
		Texture* pTexture = graphics.CreateTexture(texturePath);

		m_pMesh = graphics.CreateMesh(m_pGeometry);
		m_pMaterial = graphics.CreateMaterial(m_pShader);
		m_pMaterial->SetTexture(pTexture);

		m_primitive = true;
	}
	
	void MeshRenderer::SetCircle()
	{
		Free();
		GraphicEngine& graphics = *GameManager::GetWindow().GetGraphics();

		m_pGeometry = graphics.CreatePrimitiveGeometry(CIRCLE);


		m_pShader = GameManager::GetRenderSystem().GetShader("../Gameplay/shader/DefaultShader.hlsl");
		Texture* pTexture = graphics.CreateTexture("../Gameplay/texture/DefaultTex.dds");

		m_pMesh = graphics.CreateMesh(m_pGeometry);
		m_pMaterial = graphics.CreateMaterial(m_pShader);
		m_pMaterial->SetTexture(pTexture);

		m_primitive = true;
	}
	
	void MeshRenderer::SetCircle(const char* texturePath)
	{
		Free();
		GraphicEngine& graphics = *GameManager::GetWindow().GetGraphics();

		m_pGeometry = graphics.CreatePrimitiveGeometry(CIRCLE);


		m_pShader = GameManager::GetRenderSystem().GetShader("../Gameplay/shader/DefaultShader.hlsl");
		Texture* pTexture = graphics.CreateTexture(texturePath);

		m_pMesh = graphics.CreateMesh(m_pGeometry);
		m_pMaterial = graphics.CreateMaterial(m_pShader);
		m_pMaterial->SetTexture(pTexture);

		m_primitive = true;
	}
	
	void MeshRenderer::SetSphere()
	{
		Free();
		GraphicEngine& graphics = *GameManager::GetWindow().GetGraphics();

		m_pGeometry = graphics.CreatePrimitiveGeometry(SPHERE);


		m_pShader = GameManager::GetRenderSystem().GetShader("../Gameplay/shader/DefaultShader.hlsl");
		Texture* pTexture = graphics.CreateTexture("../Gameplay/texture/DefaultTex.dds");

		m_pMesh = graphics.CreateMesh(m_pGeometry);
		m_pMaterial = graphics.CreateMaterial(m_pShader);
		m_pMaterial->SetTexture(pTexture);

		m_primitive = true;
	}
	
	void MeshRenderer::SetSphere(const char* texturePath)
	{
		Free();
		GraphicEngine& graphics = *GameManager::GetWindow().GetGraphics();

		m_pGeometry = graphics.CreatePrimitiveGeometry(SPHERE);


		m_pShader = GameManager::GetRenderSystem().GetShader("../Gameplay/shader/DefaultShader.hlsl");
		Texture* pTexture = graphics.CreateTexture(texturePath);

		m_pMesh = graphics.CreateMesh(m_pGeometry);
		m_pMaterial = graphics.CreateMaterial(m_pShader);
		m_pMaterial->SetTexture(pTexture);

		m_primitive = true;
	}
	
	void MeshRenderer::SetCube()
	{
		Free();
		GraphicEngine& graphics = *GameManager::GetWindow().GetGraphics();

		m_pGeometry = graphics.CreatePrimitiveGeometry(CUBE);


		m_pShader = GameManager::GetRenderSystem().GetShader("../Gameplay/shader/DefaultShader.hlsl");
		Texture* pTexture = graphics.CreateTexture("../Gameplay/texture/DefaultTex.dds");

		m_pMesh = graphics.CreateMesh(m_pGeometry);
		m_pMaterial = graphics.CreateMaterial(m_pShader);
		m_pMaterial->SetTexture(pTexture);

		m_primitive = true;
	}
	
	void MeshRenderer::SetCube(const char* texturePath)
	{
		Free();
		GraphicEngine& graphics = *GameManager::GetWindow().GetGraphics();

		m_pGeometry = graphics.CreatePrimitiveGeometry(CUBE);


		m_pShader = GameManager::GetRenderSystem().GetShader("../Gameplay/shader/DefaultShader.hlsl");
		Texture* pTexture = graphics.CreateTexture(texturePath);

		m_pMesh = graphics.CreateMesh(m_pGeometry);
		m_pMaterial = graphics.CreateMaterial(m_pShader);
		m_pMaterial->SetTexture(pTexture);

		m_primitive = true;
	}
	
	MeshRenderer::~MeshRenderer()
	{
		Free();
	}
	
	void MeshRenderer::Free()
	{
		if (m_primitive == false) return;
		delete m_pGeometry;
		delete m_pMaterial;
		delete m_pMesh;
		delete m_pShader;
		m_pGeometry = nullptr;
		m_pMaterial = nullptr;
		m_pMesh = nullptr;
		m_pShader = nullptr;
		m_primitive = false;
	}
}
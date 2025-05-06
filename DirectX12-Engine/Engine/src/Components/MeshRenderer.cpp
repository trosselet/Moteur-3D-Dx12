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


#include <future>

namespace Engine
{
	void MeshRenderer::SetRectangle()
	{
		Free();
		GraphicEngine& graphics = *GameManager::GetWindow().GetGraphics();
		
		m_pGeometry = graphics.CreatePrimitiveGeometry(SQUARE);

		
		m_pShader = GameManager::GetRenderSystem().GetShader("../Gameplay/shader/DefaultShader.hlsl");
		Texture* pTexture = graphics.CreateTexture("DefaultTex.dds");

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
		Texture* pTexture = graphics.CreateTexture("DefaultTex.dds");

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
		Texture* pTexture = graphics.CreateTexture("DefaultTex.dds");

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
		Texture* pTexture = graphics.CreateTexture("DefaultTex.dds");

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

	void MeshRenderer::SetObjFile(const char* objPath)
	{
		SetObjFileInternal(objPath, "DefaultTex.dds");
	}

	void MeshRenderer::SetObjFile(const char* objPath, const char* texturePath)
	{
		SetObjFileInternal(objPath, texturePath);
	}

	void MeshRenderer::SetObjFileInternal(const char* objPath, const char* texturePath)
	{
		Free();
		GraphicEngine& graphics = *GameManager::GetWindow().GetGraphics();

		std::future<Geometry*> futureGeometry = std::async(std::launch::async, [&graphics, objPath]()
		{
			return graphics.CreateGeometryFromObjFile(objPath);
		});

		m_pGeometry = futureGeometry.get();


		m_pShader = GameManager::GetRenderSystem().GetShader("../Gameplay/shader/DefaultShader.hlsl");
		Texture* pTexture = graphics.CreateTexture(texturePath);

		m_pMesh = graphics.CreateMesh(m_pGeometry);
		m_pMaterial = graphics.CreateMaterial(m_pShader);
		m_pMaterial->SetTexture(pTexture);

		m_primitive = true;
	}

	void MeshRenderer::SetMaterialProperties(Vector<float32, 4> lightColor, Vector<float32, 4> ambient, Vector<float32, 4> diffuse, Vector<float32, 4> specular, float32 shininess, bool ingoreLighting)
	{
		m_pMaterial->SetMaterialProperties(lightColor, ambient, diffuse, specular, shininess, ingoreLighting);
	}
	
	MeshRenderer::~MeshRenderer()
	{
		Free();
	}
	
	void MeshRenderer::Free()
	{
		if (m_primitive == false) return;
		
		if (m_pGeometry)
		{
			delete m_pGeometry;
			m_pGeometry = nullptr;
		}

		if (m_pMaterial)
		{
			delete m_pMaterial;
			m_pMaterial = nullptr;
		}

		if (m_pMesh)
		{
			delete m_pMesh;
			m_pMesh = nullptr;
		}
		m_primitive = false;
	}
}
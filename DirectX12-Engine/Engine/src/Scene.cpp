#include "pch.h"
#include "Scene.h"

#include "Systems/RenderSystem.h"

#include "GameObject.h"
#include "GameManager.h"
#include "GraphicEngine.h"

namespace Engine
{
	Scene::~Scene()
	{
		for (GameObject* pGameObject : m_gameObjects)
		{
			pGameObject->Destroy();
		}

		HandleDestruction();
	}

	Scene& Scene::Create()
	{
		std::vector<Scene>& scenes = GameManager::GetScenes();
		scenes.push_back(Scene());
		return scenes.back();
	}

	void Scene::Load()
	{
		if (m_toBeLoaded || m_loaded) return;

		m_toBeLoaded = true;

		GameManager::m_pInstance->m_scenesToLoad.push_back(this);

	}

	void Scene::Unload()
	{
		if (m_toBeUnloaded || IsActive()) return;

		m_toBeUnloaded = true;

		for (GameObject* const gameObject : m_gameObjects)
		{
			gameObject->Destroy();
		}
	}

	void Scene::SetActive()
	{
		GameManager& gameManager = *GameManager::m_pInstance;
		if (gameManager.m_pActiveScene == this || gameManager.m_pNextActiveScene) return;

		if (m_loaded = false)
		{
			m_active = true;
			return;
		}
		gameManager.m_pNextActiveScene = this;
	}

	void Scene::HandleCreation()
	{
		for (GameObject* const pGameObject : m_gameObjectsToCreate)
		{

			pGameObject->m_toBeCreated = false;
			pGameObject->m_created = true;
		}
		m_gameObjectsToCreate.clear();

		for (MeshRenderer* const pMeshRenderer : m_meshRenderersToCreate)
		{

			pMeshRenderer->m_toBeCreated = false;
			pMeshRenderer->m_created = true;

			GameManager::GetRenderSystem().m_meshRenderers[pMeshRenderer->renderLayer].push_back(pMeshRenderer);
		}
		m_meshRenderersToCreate.clear();

		for (Camera* const pCamera : m_camerasToCreate)
		{

			pCamera->m_toBeCreated = false;
			pCamera->m_created = true;
		}
		m_camerasToCreate.clear();

		for (Light* const pLight : m_lightsToCreate)
		{
			pLight->m_toBeCreated = false;
			pLight->m_created = true;

			GameManager::GetWindow().GetGraphics()->AddLight(&pLight->m_lightCb);
		}
		m_lightsToCreate.clear();
		
		for (AudioComponent* const pAudioComponent : m_audioComponentToCreate)
		{
			pAudioComponent->m_toBeCreated = false;
			pAudioComponent->m_created = true;

			GameManager::GetAudioSystem().m_audioComponents.push_back(pAudioComponent);
		}
		m_audioComponentToCreate.clear();

	}

	void Scene::HandleDestruction()
	{
		for (uint16 i = 0; i < m_audioComponents.size(); i++)
		{
			AudioComponent* const pAudio = m_audioComponents[i];
			if (pAudio->m_toBeDeleted == false) continue;

			m_audioComponents.erase(m_audioComponents.begin() + i);
			delete pAudio;
		}

		for (uint16 i = 0; i < m_lights.size(); i++)
		{
			Light* const pLight = m_lights[i];
			if (pLight->m_toBeDeleted == false) continue;

			m_lights.erase(m_lights.begin() + i);
			delete pLight;
		}

		for (uint16 i = 0; i < m_cameras.size(); i++)
		{
			Camera* const pCamera = m_cameras[i];
			if (pCamera->m_toBeDeleted == false) continue;

			m_cameras.erase(m_cameras.begin() + i);
			delete pCamera;
		}

		for (GameObject* const pGameObject : m_gameObjectsToDelete)
		{
			m_gameObjectsIDs.push_back(pGameObject->m_id);
			m_gameObjects.erase(m_gameObjects.begin() + pGameObject->m_id);
			delete pGameObject;
		}
		m_gameObjectsToDelete.clear();

		for (uint16 i = 0; i < m_meshRenderers.size(); i++)
		{
			MeshRenderer* const pMeshRenderer = m_meshRenderers[i];
			if (pMeshRenderer->m_toBeDeleted == false) continue;

			m_meshRenderers.erase(m_meshRenderers.begin() + i);
			delete pMeshRenderer;
		}
	}

}
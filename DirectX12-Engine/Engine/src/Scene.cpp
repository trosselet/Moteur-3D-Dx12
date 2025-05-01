#include "pch.h"
#include "Scene.h"

#include "GameObject.h"
#include "GameManager.h"

namespace Engine
{
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
			if ( pGameObject->m_toBeDestroy ) continue;

			pGameObject->m_toBeCreated = false;
			pGameObject->m_created = true;
		}
		m_gameObjectsToCreate.clear();
	}

	void Scene::HandleDestruction()
	{
		for (GameObject* const pGameObject : m_gameObjectsToDelete)
		{
			m_gameObjectsIDs.push_back(pGameObject->m_id);
			m_gameObjects.erase(m_gameObjects.begin() + pGameObject->m_id);
			delete pGameObject;
		}
		m_gameObjectsToDelete.clear();
	}

}
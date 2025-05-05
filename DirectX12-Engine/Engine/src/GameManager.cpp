#include "pch.h"
#include "GameManager.h"

#include "GraphicEngine.h"

namespace Engine
{
	GameManager::GameManager(HINSTANCE hInstance) : 
		m_pWindow(new Window(hInstance, Vector2i{ 1920, 1080 }, "GameEngine")),
		m_pRenderSystem(new RenderSystem()),
		m_pScriptSystem(new ScriptSystem())
	{
		m_pRenderSystem->m_pGraphics = m_pWindow->GetGraphics();
		m_pRenderSystem->m_pRender = m_pWindow->GetGraphics()->GetRender();
	}

	GameManager::~GameManager()
	{
		delete m_pWindow;
		delete m_pRenderSystem;
		delete m_pScriptSystem;
	}
	
	void GameManager::GameLoop()
	{
		while (m_pWindow->IsOpen())
		{
			m_pWindow->Update();

			m_elapsedTime += m_fixedDeltaTime;

			HandleCreations();
			HandleDeletions();

			while (m_elapsedTime >= m_fixedDeltaTime)
			{
				m_pScriptSystem->OnFixedUpdate();
				m_elapsedTime -= m_fixedDeltaTime;
			}

			m_pScriptSystem->OnUpdate();
			m_pRenderSystem->HandleRendering();

		}

	}
	
	void GameManager::HandleCreations()
	{
		for (Scene* const pScene : m_scenesToLoad)
		{
			if (pScene->m_toBeUnloaded) continue;

			pScene->m_toBeLoaded = false;
			pScene->m_loaded = true;
			m_loadedScenes.push_back(pScene);
			if (pScene->m_active) m_pNextActiveScene = pScene;
		}
		m_scenesToLoad.clear();

		for (Scene* const pScene : m_loadedScenes)
			pScene->HandleCreation();

		if (m_pNextActiveScene != nullptr)
		{
			m_pActiveScene = m_pNextActiveScene;
			m_pNextActiveScene = nullptr;
		}
	}
	
	void GameManager::HandleDeletions()
	{
		for (Scene* const pScene : m_loadedScenes)
			pScene->HandleDestruction();

		for (uint8 i = 0; i < m_loadedScenes.size(); i++)
		{
			Scene& scene = *m_loadedScenes[i];
			if (scene.m_toBeUnloaded == false) continue;

			if (scene.m_toBeLoaded)
			{
				scene.m_toBeLoaded = false;
				scene.m_toBeUnloaded = false;
				continue;
			}

			scene.m_toBeUnloaded = false;
			scene.m_loaded = false;
			scene.m_gameObjects.clear();
			m_loadedScenes.erase(m_loadedScenes.begin() + i--);
		}
	}
}
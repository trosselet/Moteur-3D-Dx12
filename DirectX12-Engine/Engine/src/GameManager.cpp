#include "pch.h"
#include "GameManager.h"

#include "GraphicEngine.h"

#include "Profiler.h"
#include "AudioManager.h"

#include <future>
#include <optional>

namespace Engine
{
	GameManager::GameManager(HINSTANCE hInstance) : 
		m_pWindow(new Window(hInstance, Vector2i{ 1920, 1080 }, "GameEngine")),
		m_pRenderSystem(new RenderSystem()),
		m_pScriptSystem(new ScriptSystem()),
		m_pAudioSystem( new AudioSystem() )
	{
		m_pRenderSystem->m_pGraphics = m_pWindow->GetGraphics();
		m_pRenderSystem->m_pRender = m_pWindow->GetGraphics()->GetRender();

		Engine::AudioManager::GetInstance().Initialize();
	}

	GameManager::~GameManager()
	{
		delete m_pWindow;
		delete m_pRenderSystem;
		delete m_pScriptSystem;
		delete m_pAudioSystem;

		Engine::AudioManager::GetInstance().Shutdown();

		IDXGIDebug1* debug = nullptr;

		if (DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)) == S_OK)
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
			debug->Release();
			debug = nullptr;
		}

	}
	
	void GameManager::GameLoop()
	{
		Profiler profiler;
		

		std::optional<std::future<void>> renderFuture;
		std::optional<std::future<void>> audioFuture;

		while (m_pWindow->IsOpen())
		{
			profiler.NewTask("Window handling messages");
			m_pWindow->Update();
			profiler.EndTask(1.5);

			m_elapsedTime += m_fixedDeltaTime;


			profiler.NewTask("GameManager handling Creations");
			HandleCreations();
			profiler.EndTask(0.8);

			profiler.NewTask("GameManager handling Deletions");
			HandleDeletions();
			profiler.EndTask(0.8);

			while (m_elapsedTime >= m_fixedDeltaTime)
			{
				profiler.NewTask("FixedUpdate script system");
				m_pScriptSystem->OnFixedUpdate();
				profiler.EndTask(0.9);
				
				
				m_elapsedTime -= m_fixedDeltaTime;
			}

			profiler.NewTask("Update script system");
			m_pScriptSystem->OnUpdate();
			profiler.EndTask(0.9);

			if (renderFuture && renderFuture->valid())
			{
				renderFuture->get();
			}

			profiler.NewTask("Draw Global");
			renderFuture = std::async(std::launch::async, [&]()
				{
					m_pRenderSystem->HandleRendering();
				});
			profiler.EndTask(7);
			
			profiler.NewTask("Audio Global");
			renderFuture = std::async(std::launch::async, [&]()
				{
					m_pAudioSystem->UpdateAudio();
				});
			profiler.EndTask(10);

			system("cls");
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
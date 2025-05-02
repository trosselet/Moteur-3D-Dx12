#ifndef GAMEMANAGER__H
#define GAMEMANAGER__H

#include <vector>
#include <Windows.h>
#include "Define.h"
#include "Scene.h"
#include "Systems/RenderSystem.h"
#include "Window.h"

namespace Engine
{
	class GameManager
	{
	public:
		static void Init(HINSTANCE hInstance);
		static void Run();
		static void Release();

		static std::vector<Scene>& GetScenes();
		static Scene& GetActiveScene();
		static float32& GetFixedDeltaTime();

		static RenderSystem& GetRenderSystem();

		static Window& GetWindow() { return *m_pInstance->m_pWindow; };

	private:
		inline static GameManager* m_pInstance = nullptr;
		GameManager(HINSTANCE hInstance);
		~GameManager();

		void GameLoop();
		void HandleCreations();
		void HandleDeletions();

	private:
		Window* m_pWindow = nullptr;
		RenderSystem* m_pRenderSystem = nullptr;

		std::vector<Scene>  m_scenes;
		std::vector<Scene*> m_loadedScenes;
		Scene* m_pActiveScene = nullptr;
		Scene* m_pNextActiveScene = nullptr;
		std::vector<Scene*> m_scenesToLoad;

		float32 m_fixedDeltaTime = 0.016f;
		float32 m_elapsedTime = 0.0f;

		friend class GameObject;
		friend class Scene;
	};
}
#include "GameManager.inl"

#endif // !GAMEMANAGER__H


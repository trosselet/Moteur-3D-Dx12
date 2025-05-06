#ifndef SCENE__H
#define SCENE__H

#include "../../Core/include/Define.h"

namespace Engine { class GameObject; }

namespace Engine
{
	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		static Scene& Create();
		void Load();
		void Unload();
		void SetActive();

		bool IsActive() const;
		bool IsLoaded() const;

		GameObject const* GetMainCamera() const;
		GameObject* GetMainCamera();

		std::vector<GameObject*> const& GetGameObjects() const;
		std::vector<GameObject*>& GetGameObjects();

		void HandleCreation();
		void HandleDestruction();

	private:

		bool m_active = false;
		bool m_loaded = false;
		bool m_toBeLoaded = false;
		bool m_toBeUnloaded = false;

		GameObject* m_pMainCamera = nullptr;

		std::vector<GameObject*> m_gameObjects;
		std::vector<uint32> m_gameObjectsIDs;

		std::vector<GameObject*> m_gameObjectsToCreate;

		std::vector<GameObject*> m_gameObjectsToDelete;



		friend class GameObject;
		friend class GameManager;
	};
}

#include "Scene.inl"


#endif // !SCENE__H
#ifndef SCENE__H
#define SCENE__H

#include <vector>

#include "Define.h"

#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Light.h"
#include "Components/AudioComponent.h"



namespace Engine { class GameObject; }

namespace Engine
{
	class Scene
	{
	public:
		Scene() = default;
		~Scene();

		static Scene& Create();
		void Load();
		void Unload();
		void SetActive();

		bool IsActive() const;
		bool IsLoaded() const;

		GameObject const* GetMainCamera() const;
		GameObject* GetMainCamera();

		std::vector<GameObject*> const& GetGameObjects() const;
		std::vector<MeshRenderer*> const& GetMeshRenderers() const;
		std::vector<Camera*> const& GetCameras() const;
		std::vector<Light*> const& GetLights() const;
		std::vector<AudioComponent*> const& GetAudioComponents() const;


		std::vector<GameObject*>& GetGameObjects();
		std::vector<MeshRenderer*>& GetMeshRenderers();
		std::vector<Camera*>& GetCameras();
		std::vector<Light*>& GetLights();
		std::vector<AudioComponent*>& GetAudioComponents();

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
		std::vector<MeshRenderer*> m_meshRenderers;
		std::vector<Camera*> m_cameras;
		std::vector<Light*> m_lights;
		std::vector<AudioComponent*> m_audioComponents;


		std::vector<GameObject*> m_gameObjectsToCreate;
		std::vector<MeshRenderer*> m_meshRenderersToCreate;
		std::vector<Camera*> m_camerasToCreate;
		std::vector<Light*> m_lightsToCreate;
		std::vector<AudioComponent*> m_audioComponentToCreate;

		std::vector<GameObject*> m_gameObjectsToDelete;


		friend struct Camera;
		friend struct MeshRenderer;
		friend struct AudioComponent;
		friend struct Light;
		friend class GameObject;
		friend class GameManager;
	};
}

#include "Scene.inl"


#endif // !SCENE__H
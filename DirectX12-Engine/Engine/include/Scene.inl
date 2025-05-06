#ifndef SCENE__INL
#define SCENE__INL
#include "Scene.h"

namespace Engine
{	
	inline bool Scene::IsActive() const { return m_active; }
	inline bool Scene::IsLoaded() const { return m_loaded; }


	inline GameObject const* Scene::GetMainCamera() const { return m_pMainCamera; }
	inline GameObject* Scene::GetMainCamera() { return m_pMainCamera; }


	inline std::vector<GameObject*> const& Scene::GetGameObjects() const { return m_gameObjects; }
	inline std::vector<MeshRenderer*> const& Scene::GetMeshRenderers() const { return m_meshRenderers; }
	inline std::vector<Camera*> const& Scene::GetCameras() const { return m_cameras; }
	inline std::vector<Light*> const& Scene::GetLights() const { return m_lights; }
	inline std::vector<AudioComponent*> const& Scene::GetAudioComponents() const { return m_audioComponents; }



	inline std::vector<GameObject*>& Scene::GetGameObjects() { return m_gameObjects; }
	inline std::vector<MeshRenderer*>& Scene::GetMeshRenderers() { return m_meshRenderers; }
	inline std::vector<Camera*>& Scene::GetCameras() { return m_cameras; }
	inline std::vector<Light*>& Scene::GetLights() { return m_lights; }
	inline std::vector<AudioComponent*>& Scene::GetAudioComponents() { return m_audioComponents; }
}


#endif // !SCENE__INL
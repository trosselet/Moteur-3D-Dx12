#include "Scene.h"
#ifndef SCENE__INL
#define SCENE__INL

namespace Engine
{	
	inline bool Scene::IsActive() const { return m_active; }
	inline bool Scene::IsLoaded() const { return m_loaded; }


	inline GameObject const* Scene::GetMainCamera() const { return m_pMainCamera; }
	inline GameObject* Scene::GetMainCamera() { return m_pMainCamera; }


	inline std::vector<GameObject*> const& Scene::GetGameObjects() const { return m_gameObjects; }
	inline std::vector<GameObject*>& Scene::GetGameObjects() { return m_gameObjects; }
}


#endif // !SCENE__INL
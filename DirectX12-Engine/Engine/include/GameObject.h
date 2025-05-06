#ifndef GAMEOBJECT__H
#define GAMEOBJECT__H

#include <unordered_map>
#include "Define.h"
#include "Transform.h"

#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Light.h"

#include "IScript.h"
#include "Systems/ScriptSystem.h"

#include "GameObject.h"

#include "Scene.h"

#include "GameManager.h"
#include "Window.h"

namespace Engine
{
	class IScript;

	class GameObject
	{
	public:
		GameObject(Scene& scene);
		~GameObject() = default;

		void Destroy();

		template<class ComponentClass> bool HasComponent() const;
		template<class ComponentClass> ComponentClass const& GetComponent() const;
		template<class ComponentClass> ComponentClass& GetComponent();
		template<class ComponentClass> ComponentClass& AddComponent();
		template<class ComponentClass> void RemoveComponent();

		template<typename T, typename ... Args>
		T* AddScript(Args&&... args);

		template<typename T>
		T* GetScript();

		bool IsActive() const;
		void SetActive(bool active);
		void Activate();
		void Desactivate();

		uint32 GetID() const;
		int8 const* GetName() const;
		Scene const& GetScene() const;
		GameObject const* GetParent() const;
		std::vector<GameObject*> const& GetChildren() const;

		Scene& GetScene();
		GameObject* GetParent();
		std::vector<GameObject*>& GetChildren();

		void SetName(int8 const* name);

		TRANSFORM* m_pTransform;

	private:
		bool m_created = false;
		bool m_toBeCreated = true;
		bool m_toBeDestroy = false;
		bool m_active = true;

		uint32 m_id = 0;
		int8 const* m_name = nullptr;

		Scene* m_pScene = nullptr;
		GameObject* m_pParent = nullptr;
		std::vector<GameObject*> m_pChildren;
		std::unordered_map<uint16, Component*> m_components;
		std::vector<IScript*> m_scripts;

		uint64 m_componentBitmask = 0;

		friend class Transformable;
		friend class Scene;


	};

	template<typename T, typename ...Args>
	inline T* GameObject::AddScript(Args && ...args)
	{
		static_assert(std::is_base_of<IScript, T>::value, "T must be derived from IScript");
		T* script = new T(std::forward<Args>(args)...);
		script->SetOwner(this);
		script->OnStart();
		m_scripts.push_back(script);
		GameManager::GetScriptSystem().m_scriptsByEntity[GetID()].push_back(script);
		return script;
	}

	template<typename T>
	inline T* GameObject::GetScript()
	{
		for (int i = 0; i < m_scripts.size(); i++)
		{
			if (T * foundScript = dynamic_cast<T*>(m_scripts[i]))
			{
				return foundScript;
			}
		}
		return nullptr;
	}


	template <>
	inline MeshRenderer& GameObject::AddComponent<MeshRenderer>()
	{
		assert((HasComponent<MeshRenderer>() == false));

		MeshRenderer* const pMeshRenderer = new MeshRenderer();
		m_pScene->m_meshRenderers.push_back(pMeshRenderer);
		pMeshRenderer->m_pOwner = this;
		m_pScene->m_meshRenderersToCreate.push_back(pMeshRenderer);

		m_components[MeshRenderer::Tag] = pMeshRenderer;
		m_componentBitmask |= 1 << (MeshRenderer::Tag - 1);

		return *pMeshRenderer;
	}

	template <>
	inline Camera& GameObject::AddComponent<Camera>()
	{
		assert((HasComponent<Camera>() == false));

		Camera* const pCamera = new Camera();
		m_pScene->m_cameras.push_back(pCamera);
		pCamera->m_pOwner = this;
		m_pScene->m_camerasToCreate.push_back(pCamera);
		if (m_pScene->m_pMainCamera == nullptr) m_pScene->m_pMainCamera = pCamera->m_pOwner;

		m_components[Camera::Tag] = pCamera;
		m_componentBitmask |= 1 << (Camera::Tag - 1);

		return *pCamera;
	}

	template<>
	inline Light& GameObject::AddComponent<Light>()
	{
		assert((HasComponent<Light>() == false));

		Light* const pLight = new Light();
		m_pScene->m_lights.push_back(pLight);
		pLight->m_pOwner = this;
		m_pScene->m_lightsToCreate.push_back(pLight);

		m_components[Light::Tag] = pLight;
		m_componentBitmask |= 1 << (Light::Tag - 1);

		return *pLight;
	}

	template <>
	inline void GameObject::RemoveComponent<MeshRenderer>()
	{
		assert(HasComponent<MeshRenderer>());
		m_components[MeshRenderer::Tag]->Destroy();
	}


	template <>
	inline void GameObject::RemoveComponent<Camera>()
	{
		assert(HasComponent<Camera>());
		m_components[Camera::Tag]->Destroy();
	}

	template <>
	inline void GameObject::RemoveComponent<Light>()
	{
		assert(HasComponent<Light>());
		m_components[Light::Tag]->Destroy();
	}
	
}

#include "GameObject.inl"

#endif // !GAMEOBJECT__H
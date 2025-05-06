#ifndef GAMEOBJECT__H
#define GAMEOBJECT__H

#include "../../Core/include/Define.h"
#include "../../Core/include/TTransformable.h"

namespace Engine
{
	struct Component;
	class Scene;
}

namespace Engine
{
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

		Core::Transformable transform = Core::Transformable::Identity;

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
		//std::unordered_map<uint16, Script*> m_scripts;

		uint64 m_componentBitmask = 0;

		friend class Transformable;
		friend class Scene;


	};

	
}

#include "GameObject.inl"

#endif // !GAMEOBJECT__H
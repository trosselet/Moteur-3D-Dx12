#ifndef GAMEOBJECT__INL
#define GAMEOBJECT__INL

#include "Scene.h"
#include "GameObject.h"

namespace Engine
{
	template<class ComponentClass>
	bool GameObject::HasComponent() const { return m_componentBitmask & (1 << (ComponentClass::Tag - 1)); }

	template<class ComponentClass>
	inline ComponentClass const& GameObject::GetComponent() const
	{
		assert(HasComponent<ComponentClass>());
		return *static_cast<ComponentClass const*>(m_components[ComponentClass::Tag]);
	}

	template<class ComponentClass>
	inline ComponentClass& GameObject::GetComponent()
	{
		assert(HasComponent<ComponentClass>());
		return *static_cast<ComponentClass*>(m_components[ComponentClass::Tag]);
	}

	

	inline void GameObject::SetActive(bool const active) { m_active = active; }
	inline void GameObject::Activate() { m_active = true; }
	inline void GameObject::Desactivate() { m_active = false; }

	inline uint32 GameObject::GetID() const { return m_id; }
	inline int8 const* GameObject::GetName() const { return m_name; }
	inline Scene const& GameObject::GetScene() const { return *m_pScene; }
	inline GameObject const* GameObject::GetParent() const { return m_pParent; }
	inline std::vector<GameObject*> const& GameObject::GetChildren() const { return m_pChildren; }

	inline Scene& GameObject::GetScene() { return *m_pScene; }
	inline GameObject* GameObject::GetParent() { return m_pParent; }
	inline std::vector<GameObject*>& GameObject::GetChildren() { return m_pChildren; }

	inline void GameObject::SetName(int8 const* name) { m_name = name; }












}

#endif // !GAMEOBJECT__INL
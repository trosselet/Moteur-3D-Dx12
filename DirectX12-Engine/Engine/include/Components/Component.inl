#ifndef COMPONENT__INL
#define COMPONENT__INL

namespace Engine
{
	inline void Component::SetActive(bool const active) { m_active = active; }
	inline void Component::Activate() { m_active = true; }
	inline void Component::Deactivate() { m_active = false; }

	inline GameObject const& Component::GetOwner() const { return *m_pOwner; }

	inline GameObject& Component::GetOwner() { return *m_pOwner; }
}

#endif // !COMPONENT__INL

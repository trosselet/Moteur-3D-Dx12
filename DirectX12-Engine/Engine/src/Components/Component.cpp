#include "pch.h"
#include "Components/Component.h"

#include "GameObject.h"

namespace Engine
{
	void Component::Destroy()
	{
		if (m_toBeDeleted) return;

		m_toBeDeleted = true;
	}

	bool Component::IsActive() const
	{
		return m_active && m_pOwner->IsActive();
	}
	
}
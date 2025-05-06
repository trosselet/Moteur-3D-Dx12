#include "pch.h"
#include "GameObject.h"

#include "Component.h"

#include "Scene.h"
#include "GameManager.h"


namespace Engine
{

	GameObject::GameObject(Scene& scene) :
		m_pScene(&scene)
	{

        if (m_pScene->m_gameObjectsIDs.empty())
        {
            m_id = m_pScene->m_gameObjectsIDs.size();
            m_pScene->m_gameObjects.push_back(this);
        }
        else
        {
            m_id = m_pScene->m_gameObjectsIDs.back();
            m_pScene->m_gameObjectsIDs.pop_back();
            m_pScene->m_gameObjects[m_id] = this;
        }
        m_pScene->m_gameObjectsToCreate.push_back(this);

	}

    void GameObject::Destroy()
    {
        if (m_toBeDestroy) return;

        m_toBeDestroy = true;

        for (GameObject* const pChild : m_pChildren)
            pChild->Destroy();

        for (auto const& [tag, pComponent] : m_components)
            pComponent->Destroy();

        GameManager::GetActiveScene().m_gameObjectsToDelete.push_back(this);
    }

    bool GameObject::IsActive() const
    {
        if (m_pParent != nullptr) 
        { 
            return m_active && m_pParent->IsActive(); 
        }

        return m_active && m_pScene->IsActive();
    }

}
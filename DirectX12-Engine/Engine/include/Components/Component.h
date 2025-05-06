#ifndef COMPONENT__H
#define COMPONENT__H

#include "Define.h"

namespace Engine
{
	class GameObject;
}

namespace Engine
{
    struct Component
    {
        enum Tag : uint16 {
            None,
            MeshRenderer,
            SphereCollider,
            BoxCollider,
            RigidBody,
            CharacterController,
            Camera,
            Light,
            Shot,
            Audio,
        };

        void Destroy();

        bool IsActive() const;
        void SetActive(bool active);
        void Activate();
        void Deactivate();

        GameObject const& GetOwner() const;

        GameObject& GetOwner();

    protected:
        Component() = default;
        virtual ~Component() = default;

        bool m_active = true;  
        bool m_created = false; 
        bool m_toBeCreated = true;  
        bool m_toBeDeleted = false; 

        GameObject* m_pOwner = nullptr;

        friend class GameObject;
        friend class Scene;

    };
}

#include "Component.inl"


#endif // !COMPONENT__H
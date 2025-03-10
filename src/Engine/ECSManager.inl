#pragma once
#include "ECSManager.h"
#include "Component.h"


template <typename T>
inline T* ECSManager::GetComponent(int intComponent, EntityID entity)
{
	if (mEntityPComponent.find(entity) != mEntityPComponent.end()) {
		for (Component* component : mEntityPComponent[entity]) {
			int compkey = component->GetID();
			if (component->GetID() == intComponent) {
				return dynamic_cast<T*>(component);
			}
		}
	}
	return nullptr;
}
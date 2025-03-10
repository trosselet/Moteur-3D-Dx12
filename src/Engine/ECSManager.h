#pragma once
#include <unordered_map>
#include <vector>
#include <list>



class Component;

class System;

class Entity;

class ECSManager
{
public:
	ECSManager() = default;
	virtual ~ECSManager();

	enum enumComponent {
		enumComponent,				//0 
		enumTransformComponent,		//1
		enumMeshComponent,			//2
		enumStatsComponent,			//3
		enumInputControlerComponent,//4
		enumBehaviourComponent,		//5
		enumCameraComponent,		//6
	};
	using EntityID = uint32_t;

	void Initialize();

	void addComponent(Component* pComponent, EntityID entity);

	template <typename T>
	T* GetComponent(int intComponent, EntityID entity);

	void Update();

	ECSManager* Get();

	EntityID Create();
	void Destroy(EntityID entity);


private:
	std::unordered_map<int, std::vector<Component*>> mEntityPComponent;

	std::vector<Component*> allpComponents;
	std::vector<System*> allSystems;

	std::list<int> entitiesToRemove;

	EntityID nextID = 1;
	std::vector<EntityID> recycledIDs;
};

#include "ECSManager.inl"

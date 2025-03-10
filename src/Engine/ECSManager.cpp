#include "pch.h"
#include "Component.h"
#include "ECSManager.h"
#include "System.h"
#include "StatsSystem.h"
#include "InputSystem.h"
#include "BehaviourSystem.h"
#include "CameraSystem.h"


ECSManager::~ECSManager()
{
    mEntityPComponent.clear();
    allpComponents.clear();

    for (System* sys : allSystems)
    {
        delete sys;
    }
    allSystems.clear();
}

void ECSManager::Initialize()
{
    allpComponents = {};
    allSystems = {};

    StatsSystem* statsSystem = new StatsSystem();
    InputSystem* inputSystem = new InputSystem();
    BehaviourSystem* behaviourSystem = new BehaviourSystem();
    CameraSystem* cameraSystem = new CameraSystem();

    allSystems.push_back(statsSystem);
    allSystems.push_back(inputSystem);
    allSystems.push_back(behaviourSystem);
    allSystems.push_back(cameraSystem);

    inputSystem->Initialize();
    cameraSystem->Initialize();

}

void ECSManager::addComponent(Component* pComponent, EntityID entity)
{
    pComponent->entity = entity;
    allpComponents.push_back(pComponent);

    mEntityPComponent[pComponent->entity].push_back(pComponent);
}


void ECSManager::Update()
{
    for (const auto& elem : mEntityPComponent)
    {
        for (auto system : allSystems) {
            std::vector<Component*> pWantedComponents = system->HasWantedComponent(elem.second);

            if (!pWantedComponents.empty())
            {
                system->OnUpdate(pWantedComponents);
            }
        }
    }

    for (auto it = entitiesToRemove.begin(); it != entitiesToRemove.end();)
    {
        if (mEntityPComponent.find(*it) != mEntityPComponent.end()) {
            mEntityPComponent.erase(*it);
            it = entitiesToRemove.erase(it);
        }
        else {
            ++it;
        }
    }
    entitiesToRemove.clear();
}


ECSManager* ECSManager::Get()
{
    static ECSManager mInstance;

    return &mInstance;
}


ECSManager::EntityID ECSManager::Create()
{
    if (!recycledIDs.empty()) {
        EntityID id = recycledIDs.back();
        recycledIDs.pop_back();
        return id;
    }
    return nextID++;
}

void ECSManager::Destroy(EntityID entity)
{
    if (mEntityPComponent.find(entity) != mEntityPComponent.end())
    {
        for (Component* component : mEntityPComponent[entity])
        {
            delete component;
        }
        mEntityPComponent.erase(entity);
    }
    entitiesToRemove.push_back(entity);
    recycledIDs.push_back(entity);
}
#pragma once
#include "pch.h"
#include "ECSManager.h"
#include "System.h"

class Entity;

class StatsSystem : public System
{
public:

    StatsSystem();
    virtual ~StatsSystem() override;

    void OnUpdate(std::vector<Component*>) override;
    
    void TakeDamage(float& hp, float damage);
    void Heal(float& hp, float heal);
    void SetHealth(float& hp, float health);
};


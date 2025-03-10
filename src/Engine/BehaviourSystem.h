#pragma once
#include "pch.h"
#include <vector>
#include "ECSManager.h"
#include "System.h"
#include "Component.h"

class BehaviourSystem : public System
{
public:
	BehaviourSystem();
	virtual ~BehaviourSystem() override;

	void Initialize();

	void OnUpdate(std::vector<Component*>);


};

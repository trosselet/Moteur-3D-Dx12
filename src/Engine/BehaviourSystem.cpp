#include "pch.h"
#include "BehaviourSystem.h"

BehaviourSystem::BehaviourSystem()
{
}

BehaviourSystem::~BehaviourSystem()
{
}

void BehaviourSystem::Initialize()
{
	mNeededComponent = { 4, 6};
}

void BehaviourSystem::OnUpdate(std::vector<Component*> components)
{
	components[0];
		
}

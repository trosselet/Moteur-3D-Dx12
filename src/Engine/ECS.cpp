#include "pch.h"
#include "ECS.h"

Entity ECS::CreateEntity()
{
	return nextEntity++;
}

void ECS::AddComponent(Entity entity, Component pComponent)
{
	entityComponents[entity].emplace_back(pComponent);
}

std::unordered_map<Entity, std::vector<Component>>& ECS::GetEntities()
{
	return entityComponents;
}

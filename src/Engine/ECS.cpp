#include "pch.h"
#include "ECS.h"

Entity ECS::CreateEntity()
{
	return nextEntity++;
}

std::unordered_map<Entity, std::vector<Component*>>& ECS::GetEntities()
{
	return entityComponents;
}

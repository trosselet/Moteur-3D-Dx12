#include "pch.h"
#include "Entity.h"

void Entity::Initialize()
{

}

std::vector<Component> Entity::GetComponentList()
{
	return mComponentList;
}

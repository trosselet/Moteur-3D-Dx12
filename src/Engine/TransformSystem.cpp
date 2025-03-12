#include "pch.h"
#include "TransformSystem.h"


void TransformSystem::Update(ECS& ecs, float deltaTime)
{
	for (auto& [entity, componentList] : ecs.GetEntities())
	{
		TransformComponent* transform = ecs.GetComponent<TransformComponent>(entity);

		if (transform)
		{
			transform->x += 1 * deltaTime;
			transform->y += 1 * deltaTime;
		}
	}
}

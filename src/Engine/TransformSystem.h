#ifndef TRANSFORM_SYSTEM_H__
#define TRANSFORM_SYSTEM_H__

#include "System.h"

class TransformSystem : public System
{
public:
	TransformSystem() = default;
	~TransformSystem() = default;

	void Update(ECS& ecs, float deltaTime) override;
};


#endif // !TRANSFORM_SYSTEM_H__
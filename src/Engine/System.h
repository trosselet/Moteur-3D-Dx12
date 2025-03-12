#ifndef SYSTEM_H__
#define SYSTEM_H__

class ECS;

class System
{
public:
	System() = default;
	virtual ~System() = default;

	virtual void Update(ECS& ecs, float deltaTime) = 0;
};


#endif // !SYSTEM_H__
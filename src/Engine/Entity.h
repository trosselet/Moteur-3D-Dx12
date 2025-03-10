#ifndef ENTITY_H__
#define ENTITY_H__

#include "Component.h"
#include <vector>

class Entity
{
public:
	Entity() = default;
	virtual ~Entity() = default;

	void Initialize();

	std::vector<Component> GetComponentList();

private:
	std::vector<Component> mComponentList;
};

#endif // !ENTITY_H__

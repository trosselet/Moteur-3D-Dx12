#ifndef ECS_H__
#define ECS_H__

#include "vector"
#include "unordered_map"

class Component;
using Entity = int;

class ECS
{
public:
	ECS() = default;
	virtual ~ECS() = default;

	template <typename T>
	T* GetComponent(Entity entity);

	void AddComponent(Entity entity, Component pComponent);

	Entity CreateEntity();

	std::unordered_map<Entity, std::vector<Component>>& GetEntities();

private:
	std::unordered_map<Entity, std::vector<Component>> entityComponents;
	Entity nextEntity = 0;
};


#endif // !ECS_H__

template<typename T>
inline T* ECS::GetComponent(Entity entity)
{
	auto it = entityComponents.find(entity);
	if (it != entityComponents.end())
	{
		for (Component component : it->second)
		{
			if (T* comp = dynamic_cast<T*>(component))
			{
				return comp;
			}
		}
	}
	return nullptr;
}

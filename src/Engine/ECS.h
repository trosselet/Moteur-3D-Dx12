#ifndef ECS_H__
#define ECS_H__

class Component;
class Entity;

class ECS
{
public:
	ECS() = default;
	virtual ~ECS() = default;

	template <typename T>
	T GetComponent(Entity entity);

	void AddComponent(Entity entity, Component component);
	Entity CreateEntity();

};


#endif // !ECS_H__

template<typename T>
inline T ECS::GetComponent(Entity entity)
{
	for (Component comp : entity.GetComponentList())
	{
		if (comp == T)
		{
			return comp;
		}
	}

	return T();
}

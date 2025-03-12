#ifndef COMPONENT_H__
#define COMPONENT_H__


class Component
{
public:
	Component() = default;
	virtual ~Component() = default;
};

#endif // !COMPONENT_H__


struct TransformComponent : public Component
{
	float x;
	float y;
};
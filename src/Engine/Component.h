#ifndef COMPONENT_H_INCLUDED
#define COMPONENT_H_INCLUDED


struct Component
{
};



struct TransformComponent : public Component
{
	float x;
	float y;
};


#endif // !COMPONENT_H_INCLUDED
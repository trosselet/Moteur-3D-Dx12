#ifndef COMPONENTBASE__H
#define COMPONENTBASE__H

#include "Define.h"

#include "Component.h"

namespace Engine
{
	template<uint16 T>
	struct ComponentBase : Component
	{
		static inline uint16 const Tag = T;
	};
}

#endif // !COMPONENTBASE__H

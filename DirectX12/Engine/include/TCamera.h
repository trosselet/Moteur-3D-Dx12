#ifndef TCAMERA__H
#define TCAMERA__H

#include "../../Core/include/TTransformable.h"
#include "../../Core/include/TTransform.h"

namespace Engine
{
	class Camera : public Core::Transformable
	{
	public:
		Camera();
		~Camera();

		TRANSFORM& GetTransform() { return m_transform; };
	};
}

#endif // !TCAMERA__H

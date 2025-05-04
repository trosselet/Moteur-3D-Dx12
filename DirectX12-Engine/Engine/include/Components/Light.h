#ifndef LIGHT__H
#define LIGHT__H

#include "Define.h"
#include "UploadBuffer.h"
#include "Vector3.hpp"
#include "ComponentBase.h"

namespace Engine
{
	struct Light : public Engine::ComponentBase<Component::Light>
	{
        void SetColor(Vector3f color);
        void SetType(int32 type);
        void SetPosition(Vector3f position);
        void SetIntensity(float32 intensity);
        void SetDirection(Vector3f direction);
        void SetSpotAngle(float32 angle);

    private:
        LightCB m_lightCb = {};
        friend class Scene;
	};
}


#endif // !LIGHT__H

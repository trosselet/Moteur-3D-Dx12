#ifndef CAMERA__H
#define CAMERA__H

#include <DirectXMath.h>
#include "Define.h"

#include "ComponentBase.h"

namespace Engine
{
	struct Camera : public Engine::ComponentBase<Component::Camera>
	{
		float32 viewWidth  = 1920;
		float32 viewHeight = 1080;

		float32 fov = 0.25 * DirectX::XM_PI;

		float32 nearZ = 0.1f;
		float32 farZ = 10000000.0f;

		DirectX::XMMATRIX projectionMatrix;
		DirectX::XMMATRIX viewMatrix;
	};
}

#endif // !CAMERA__H


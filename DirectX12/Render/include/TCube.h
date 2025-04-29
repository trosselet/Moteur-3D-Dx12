#ifndef CUBE_SHAPE__H
#define CUBE_SHAPE__H

#include "TShape.h"

#include "../../Core/include/Define.h"
#include "../../Core/include/TVector.hpp"

namespace Render
{
	class CubeShape : public Shape
	{
	public:
		CubeShape(Vector3f position = Vector3f{ 0.0f, 0.0f, 0.0f }, float32 size = 1.0f, Color color = Color::White, LPCWSTR texturePath = L"../Game/texture/DefaultTex.dds");
		~CubeShape();

	private:
		void BuildBuffers() override;
	};
}

#endif // !CUBE_SHAPE__H

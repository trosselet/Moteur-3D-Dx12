#ifndef CIRCLE_SHAPE__H
#define CIRCLE_SHAPE__H

#include "TShape.h"

#include "../../Core/include/Define.h"
#include "../../Core/include/TVector.hpp"

namespace Render
{
	class CircleShape : public Shape
	{
	public:
		CircleShape(Vector3f position = Vector3f{ 0.0f, 0.0f, 0.0f }, float32 radius = 1, Color color = Color::White, LPCWSTR texturePath = L"../Game/texture/DefaultTex.dds");
		~CircleShape();

	private:
		void BuildBuffers() override;

	private:
		float32 m_radius;
		uint32 m_detailLevel;
	};
}

#endif // !CIRCLE_SHAPE__H

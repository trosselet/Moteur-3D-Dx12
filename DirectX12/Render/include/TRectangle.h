#ifndef RECTANGLE_SHAPE__H
#define RECTANGLE_SHAPE__H

#include "TShape.h"

#include "../../Core/include/Define.h"
#include "../../Core/include/TVector.hpp"

namespace Render
{
	class RectangleShape : public Shape
	{
	public:
		RectangleShape(Vector3f position = Vector3f{0.0f, 0.0f, 0.0f}, Vector3f scale = Vector3f{ 1.0f, 1.0f, 1.0f }, Color color = Color::White, LPCWSTR texturePath = L"../Game/texture/wood.dds");
		~RectangleShape();

	private:
		void BuildBuffers() override;
	};

}

#endif // !RECTANGLE_SHAPE__H
#ifndef RECTANGLE_SHAPE__H
#define RECTANGLE_SHAPE__H

#include "TShape.h"

namespace Render
{
	class RectangleShape : public Shape
	{
	public:
		RectangleShape();
		~RectangleShape();

	private:
		void BuildBuffers() override;
	};

}

#endif // !RECTANGLE_SHAPE__H
#ifndef PRIMITIVE_FACTORY__H
#define PRIMITIVE_FACTORY__H


#include <vector>
#include "Geometry.h"

#include "GraphicEngine.h"



namespace Render
{
	enum PrimitiveType : uint8
	{
		SQUARE,
		CIRCLE,
		CUBE,
		SPHERE,


		PRIMITIVE_COUNT
	};

	class PrimitiveFactory
	{
		friend class GraphicEngine;

	public:

		static PrimitiveFactory* Get();

		static Geometry* GetPrimitiveGeometry(PrimitiveType primitiveType);

	private:
		static void InitializePrimitiveGeometry();

	private:
		std::vector<Geometry> m_geometries;
	};
}


#endif // !PRIMITIVE_FACTORY__H
#include "pch.h"
#include "TRectangle.h"

Render::RectangleShape::RectangleShape(Vector3f position, Vector3f scale)
{
	SetPosition(position);
	SetScale(scale);
}

Render::RectangleShape::~RectangleShape()
{

}

void Render::RectangleShape::BuildBuffers()
{
	Vertex vertices[] =
	{
		{{-0.5f,  0.5f, 0.0f}, {1, 0, 0, 1}},
		{{ 0.5f,  0.5f, 0.0f}, {0, 1, 0, 1}},
		{{-0.5f, -0.5f, 0.0f}, {0, 0, 1, 1}},
		{{ 0.5f, -0.5f, 0.0f}, {1, 1, 0, 1}},
	};

	UINT indices[] =
	{
		0, 1, 2,
		2, 1, 3
	};

	UploadBuffers(vertices, _countof(vertices), indices, _countof(indices));
}

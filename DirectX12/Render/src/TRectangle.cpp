#include "pch.h"
#include "TRectangle.h"

Render::RectangleShape::RectangleShape(Vector3f position, Vector3f scale, Color color)
{
	SetPosition(position);
	SetScale(scale);
	m_color = color;
}

Render::RectangleShape::~RectangleShape()
{

}

void Render::RectangleShape::BuildBuffers()
{
	Vertex vertices[] =
	{
		{{-0.5f,  0.5f, 0.0f}, {m_color.r, m_color.g, m_color.b, 1}},
		{{ 0.5f,  0.5f, 0.0f}, {m_color.r, m_color.g, m_color.b, 1}},
		{{-0.5f, -0.5f, 0.0f}, {m_color.r, m_color.g, m_color.b, 1}},
		{{ 0.5f, -0.5f, 0.0f}, {m_color.r, m_color.g, m_color.b, 1}},
	};

	UINT indices[] =
	{
		0, 1, 2,
		2, 1, 3
	};

	UploadBuffers(vertices, _countof(vertices), indices, _countof(indices));
}

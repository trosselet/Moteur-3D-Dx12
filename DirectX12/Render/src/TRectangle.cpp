#include "pch.h"
#include "TRectangle.h"

Render::RectangleShape::RectangleShape()
{

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

	m_vertexCount = sizeof(vertices) / sizeof(Vertex);
	m_indexCount = sizeof(indices) / sizeof(UINT);

	m_pVertexBuffer = new UploadBuffer<Vertex>(m_pDevice, m_vertexCount, false);
	
	for (UINT i = 0; i < m_vertexCount; ++i)
	{
		m_pVertexBuffer->CopyData(i, vertices[i]);
	}

	
	m_pIndexBuffer = new UploadBuffer<UINT>(m_pDevice, m_indexCount, false);
	
	for (UINT i = 0; i < m_indexCount; ++i)
	{
		m_pIndexBuffer->CopyData(i, indices[i]);
	}

	m_isInitialize = true;
}

#include "pch.h"
#include "TCube.h"

Render::CubeShape::CubeShape(Vector3f position, float32 size, Color color, LPCWSTR texturePath)
{
	SetPosition(position);
	SetScale(Vector3f(size, size, size));
	m_color = color;
	m_pTextureHeap = m_pAssetManager->GetTexture(texturePath);
}

Render::CubeShape::~CubeShape()
{

}

void Render::CubeShape::BuildBuffers()
{
    float32 half = GetScale().x * 0.5f;

    std::vector<Vertex> vertices =
    {
        // FRONT
        {{-half,  half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 1.0f}},
        {{ half,  half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 1.0f}},
        {{-half, -half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 0.0f}},
        {{ half, -half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 0.0f}},

        // BACK
        {{-half,  half, half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 1.0f}}, 
        {{ half,  half, half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 1.0f}}, 
        {{-half, -half, half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 0.0f}}, 
        {{ half, -half, half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 0.0f}}, 
    };

    std::vector<uint32> indices =
    {
        // FRONT
        0, 1, 2,
        2, 1, 3,

        // BACK
        5, 4, 7,
        7, 4, 6,

        // LEFT
        4, 0, 6,
        6, 0, 2,

        // RIGHT
        1, 5, 3,
        3, 5, 7,

        // UP
        4, 5, 0,
        0, 5, 1,

        // DOWN
        2, 3, 6,
        6, 3, 7,
    };

    UploadBuffers(vertices.data(), (UINT)vertices.size(), indices.data(), (UINT)indices.size());
}


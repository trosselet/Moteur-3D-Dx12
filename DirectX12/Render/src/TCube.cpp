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
        {{-half,  half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 0.0f}},
        {{ half,  half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 0.0f}},
        {{-half, -half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 1.0f}},
        {{ half, -half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 1.0f}},

        // BACK
        {{ half,  half, half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 0.0f}},
        {{-half,  half, half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 0.0f}},
        {{ half, -half, half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 1.0f}},
        {{-half, -half, half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 1.0f}},

        // LEFT
        {{-half,  half, half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 0.0f}},
        {{-half,  half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 0.0f}},
        {{-half, -half, half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 1.0f}},
        {{-half, -half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 1.0f}},

        // RIGHT
        {{ half,  half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 0.0f}},
        {{ half,  half, half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 0.0f}},
        {{ half, -half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 1.0f}},
        {{ half, -half, half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 1.0f}},

        // TOP
        {{-half,  half, half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 0.0f}},
        {{ half,  half, half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 0.0f}},
        {{-half,  half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 1.0f}},
        {{ half,  half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 1.0f}},

        // BOTTOM
        {{-half, -half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 0.0f}},
        {{ half, -half, -half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 0.0f}},
        {{-half, -half, half}, {m_color.r, m_color.g, m_color.b, 1}, {0.0f, 1.0f}},
        {{ half, -half, half}, {m_color.r, m_color.g, m_color.b, 1}, {1.0f, 1.0f}},
    };

    std::vector<uint32> indices =
    {
        // FRONT
        0, 1, 2,
        2, 1, 3,

        // BACK
        4, 5, 6,
        6, 5, 7,

        // LEFT
        8, 9,10,
        10, 9,11,

        // RIGHT
        12,13,14,
        14,13,15,

        // TOP
        16,17,18,
        18,17,19,

        // BOTTOM
        20, 21, 22,
        22, 21, 23,
    };

    UploadBuffers(vertices.data(), (UINT)vertices.size(), indices.data(), (UINT)indices.size());
}


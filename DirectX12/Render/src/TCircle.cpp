#include "pch.h"
#include "TCircle.h"

Render::CircleShape::CircleShape(Vector3f position, float32 radius, Color color) : 
    m_radius(radius), m_detailLevel(64)
{
    SetPosition(position);
    SetScale(Vector3f( 1.0f, 1.0f, 1.0f ));
    m_color = color;
}

Render::CircleShape::~CircleShape()
{
}

void Render::CircleShape::BuildBuffers()
{
    std::vector<Vertex> vertices;
    std::vector<uint32> indices;

    vertices.push_back({ {0.0f, 0.0f, 0.0f}, {m_color.r, m_color.g, m_color.b, 1} });

    float32 angleStep = DirectX::XM_2PI / m_detailLevel;
    for (uint32 i = 0; i < m_detailLevel; ++i)
    {
        float32 angle = i * angleStep;
        float32 x = m_radius * cosf(angle);
        float32 y = m_radius * sinf(angle);

        vertices.push_back({ {x, y, 0.0f}, {m_color.r, m_color.g, m_color.b, 1} });
    }

    for (uint32 i = 0; i < m_detailLevel; ++i)
    {
        uint32 current = i + 1;
        uint32 next = (i + 1) % m_detailLevel + 1;

        indices.push_back(0);     
        indices.push_back(next);  
        indices.push_back(current);
    }

    UploadBuffers(vertices.data(), (UINT)vertices.size(), indices.data(), (UINT)indices.size());
}


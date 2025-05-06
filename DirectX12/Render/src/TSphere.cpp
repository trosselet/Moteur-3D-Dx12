#include "pch.h"
#include "TSphere.h"

Render::SphereShape::SphereShape(Vector3f position, float32 radius, Color color, LPCWSTR texturePath)
{
    SetPosition(position);
    SetScale(Vector3f(1.0f, 1.0f, 1.0f));
    m_color = color;
    m_pTextureHeap = m_pAssetManager->GetTexture(texturePath);
    m_radius = radius;
    m_detailLevel = 32;
}

Render::SphereShape::~SphereShape()
{
}

void Render::SphereShape::BuildBuffers()
{
    const uint32 latitudeSegments = m_detailLevel;
    const uint32 longitudeSegments = m_detailLevel;

    std::vector<Vertex> vertices;
    std::vector<uint32> indices;

    for (uint32 lat = 0; lat <= latitudeSegments; ++lat)
    {
        float32 theta = static_cast<float32>(lat) * DirectX::XM_PI / latitudeSegments;
        float32 sinTheta = sinf(theta);
        float32 cosTheta = cosf(theta);

        for (uint32 lon = 0; lon <= longitudeSegments; ++lon)
        {
            float32 phi = static_cast<float32>(lon) * DirectX::XM_2PI / longitudeSegments;
            float32 sinPhi = sinf(phi);
            float32 cosPhi = cosf(phi);

            float32 x = m_radius * sinTheta * cosPhi;
            float32 y = m_radius * cosTheta;
            float32 z = m_radius * sinTheta * sinPhi;

            float32 u = 1.0f - (float32)lon / longitudeSegments;
            float32 v = 1.0f - (float32)lat / latitudeSegments;

            vertices.push_back({ { x, y, z }, {m_color.r, m_color.g, m_color.b, 1}, { u, v } });
        }
    }

    for (uint32 lat = 0; lat < latitudeSegments; ++lat)
    {
        for (uint32 lon = 0; lon < longitudeSegments; ++lon)
        {
            uint32 first = (lat * (longitudeSegments + 1)) + lon;
            uint32 second = first + longitudeSegments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    for (uint32 i = 0; i < indices.size(); i += 3)
    {
        std::swap(indices[i + 1], indices[i + 2]);
    }

    UploadBuffers(vertices.data(), (UINT)vertices.size(), indices.data(), (UINT)indices.size());
}


#include "pch.h"
#include "PrimitiveFactory.h"

PrimitiveFactory* PrimitiveFactory::Get()
{
    static PrimitiveFactory instance;

    return &instance;
}

Geometry* PrimitiveFactory::GetPrimitiveGeometry(PrimitiveType primitiveType)
{
    return &PrimitiveFactory::Get()->m_geometries[primitiveType];
}

void PrimitiveFactory::InitializePrimitiveGeometry()
{
    PrimitiveFactory* pInstance = PrimitiveFactory::Get();
    
    Geometry square;
    Geometry circle;
    Geometry cube;
    Geometry sphere;

    square.positions =
    {
        {-0.5f,  0.5f, 0.0f},
        { 0.5f,  0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f},
        { 0.5f, -0.5f, 0.0f}
    };

    square.indexes =
    {
        0, 1, 2,
        2, 1, 3
    };

    square.UVs =
    {
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f }
    };

    square.normals =
    {
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f }
    };

    square.indiciesNumber = 6;
    pInstance->m_geometries.push_back(square);


    uint32 detailLevel = 50;
    float32 radius = 0.5;

    circle.positions.push_back({ 0.0f, 0.0f, 0.0f });
    circle.UVs.push_back({ 0.5f, 0.5f });

    float32 angleStep = DirectX::XM_2PI / detailLevel;
    for (uint32 i = 0; i < detailLevel; ++i)
    {
        float32 angle = i * angleStep;
        float32 x = radius * cosf(angle);
        float32 y = radius * sinf(angle);

        float32 u = 0.5f + 0.5f * cosf(angle);
        float32 v = 0.5f + 0.5f * sinf(angle);

        circle.positions.push_back({ x, y, 0.0f });
        circle.UVs.push_back({ u, v });
    }

    for (uint32 i = 0; i < detailLevel; ++i)
    {
        uint32 current = i + 1;
        uint32 next = (i + 1) % detailLevel + 1;

        circle.indexes.push_back(0);
        circle.indexes.push_back(next);
        circle.indexes.push_back(current);
    }
    circle.indiciesNumber = (uint32)sphere.indexes.size();
    pInstance->m_geometries.push_back(circle);

    cube.positions = {
        { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, // Front face
        { 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, -0.5f }, // Right face
        { 0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 0.5f }, // Back face
        { -0.5f, -0.5f, 0.5f }, { -0.5f, -0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, { -0.5f, 0.5f, 0.5f }, // Left face
        { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, 0.5f }, { -0.5f, 0.5f, 0.5f }, // Top face
        { -0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f }  // Bottom face
    };

    cube.indexes = {
        0,  1,  2,  0,  2,  3,
        4,  5,  6,  4,  6,  7,
        8,  9, 10,  8, 10, 11,
       12, 13, 14, 12, 14, 15,
       16, 17, 18, 16, 18, 19,
       20, 21, 22, 20, 22, 23
    };

    cube.indiciesNumber = 36;

    cube.UVs = {
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f },
        { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f }
    };


    cube.normals = {
        { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, // Front face
        { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, // Right face
        { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, // Back face
        { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, // Left face
        { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, // Top face
        { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }  // Bottom face
    };

    pInstance->m_geometries.push_back(cube);

    float phi = 0.f;
    float theta = 0.f;

    for (int i = 0; i <= detailLevel; ++i)
    {
        phi = DirectX::XM_PI * i / detailLevel;
        for (int j = 0; j <= detailLevel; ++j)
        {
            theta = 2 * DirectX::XM_PI * j / detailLevel;
            DirectX::XMFLOAT3 vertex;
            vertex.x = radius * sin(phi) * cos(theta);
            vertex.y = radius * cos(phi);
            vertex.z = radius * sin(phi) * sin(theta);
            sphere.positions.push_back(vertex);

            float u = 1.0f - (j / (float)detailLevel);
            float v = 1.0f - (i / (float)detailLevel);

            sphere.UVs.push_back(DirectX::XMFLOAT2(u, v));

            DirectX::XMFLOAT3 normal = { vertex.x, vertex.y, vertex.z };
            float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= length;
            normal.y /= length;
            normal.z /= length;
            sphere.normals.push_back(normal);
        }
    }

    for (int i = 0; i < detailLevel; ++i)
    {
        for (int j = 0; j < detailLevel; ++j)
        {
            int index = i * (detailLevel + 1) + j;
            sphere.indexes.push_back(index);
            sphere.indexes.push_back(index + 1);
            sphere.indexes.push_back(index + detailLevel + 1);

            sphere.indexes.push_back(index + 1);
            sphere.indexes.push_back(index + detailLevel + 2);
            sphere.indexes.push_back(index + detailLevel + 1);
        }
    }
    sphere.indiciesNumber = (uint32)sphere.indexes.size();
    pInstance->m_geometries.push_back(sphere);
}

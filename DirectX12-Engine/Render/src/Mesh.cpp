#include "pch.h"
#include "Mesh.h"

#include "Render.h"
#include "DeviceResources.h"
#include "Geometry.h"

Mesh::Mesh(Geometry* pGeometry, Render* pRender) :
	m_pGeometry(pGeometry), m_pRender(pRender)
{
	UploadGeometry();
}

Mesh::~Mesh()
{
    ReleaseUploadBuffers();

    if (m_pVertexBuffer)
    {
        delete m_pVertexBuffer;
        m_pVertexBuffer = nullptr;
    }

    if (m_pVertexBuffer)
    {
        delete m_pIndexBuffer;
        m_pIndexBuffer = nullptr;
    }
}

void Mesh::UpdateGeometry()
{
}

D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBuffer()
{
    return *m_pIndexBuffer;
}

D3D12_VERTEX_BUFFER_VIEW Mesh::GetVertexBuffer()
{
    return *m_pVertexBuffer;
}

UINT Mesh::GetIndexCount()
{
    return static_cast<UINT>(m_pGeometry->indiciesNumber);
}

void Mesh::ReleaseUploadBuffers()
{
	if (m_pIndexBufferUploader)
	{
        m_pIndexBufferUploader->Release();
        m_pIndexBufferUploader = nullptr;
	}
	
	if (m_pVertexBufferUploader)
	{
        m_pVertexBufferUploader->Release();
		m_pVertexBufferUploader = nullptr;
	}
}

void Mesh::UploadGeometry()
{
    if (!m_pGeometry) return;

    std::vector<float> geometryData;
    int size = 3;

    bool hasColor = !m_pGeometry->colors.empty();
    bool hasUV = !m_pGeometry->UVs.empty();
    bool hasNormal = !m_pGeometry->normals.empty();

    if (hasColor) size += 4;
    if (hasUV) size += 2;
    if (hasNormal) size += 3;

    geometryData.reserve(m_pGeometry->positions.size() * size);

    for (size_t i = 0; i < m_pGeometry->positions.size(); ++i)
    {
        geometryData.push_back(m_pGeometry->positions[i].x);
        geometryData.push_back(m_pGeometry->positions[i].y);
        geometryData.push_back(m_pGeometry->positions[i].z);

        if (hasColor)
        {
            geometryData.push_back(m_pGeometry->colors[i].x);
            geometryData.push_back(m_pGeometry->colors[i].y);
            geometryData.push_back(m_pGeometry->colors[i].z);
            geometryData.push_back(m_pGeometry->colors[i].w);
        }

        if (hasUV)
        {
            geometryData.push_back(m_pGeometry->UVs[i].x);
            geometryData.push_back(m_pGeometry->UVs[i].y);
        }

        if (hasNormal)
        {
            geometryData.push_back(m_pGeometry->normals[i].x);
            geometryData.push_back(m_pGeometry->normals[i].y);
            geometryData.push_back(m_pGeometry->normals[i].z);
        }
    }

    UploadBuffers(
        geometryData.data(),
        static_cast<UINT>(geometryData.size()),
        m_pGeometry->indexes.data(),
        static_cast<UINT>(m_pGeometry->indexes.size()),
        size
    );
}

void Mesh::UploadBuffers(float32* vertices, UINT vertexCount, uint32* indices, UINT indexCount, UINT floatStride)
{
    m_vertexCount = vertexCount;
    m_indexCount = indexCount;

    UINT vertexBufferSize = vertexCount * sizeof(float32);
    UINT indexBufferSize = indexCount * sizeof(uint32);

    m_pVertexBufferGPU = m_pRender->GetDeviceResources()->CreateDefaultBuffer(
        m_pRender->GetDeviceResources()->GetDevice(),
        m_pRender->GetDeviceResources()->GetCommandList(),
        vertices,
        vertexBufferSize,
        m_pVertexBufferUploader,
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
    );

    m_pIndexBufferGPU = m_pRender->GetDeviceResources()->CreateDefaultBuffer(
        m_pRender->GetDeviceResources()->GetDevice(),
        m_pRender->GetDeviceResources()->GetCommandList(),
        indices,
        indexBufferSize,
        m_pIndexBufferUploader,
        D3D12_RESOURCE_STATE_INDEX_BUFFER
    );

    D3D12_VERTEX_BUFFER_VIEW vertexView = {};
    vertexView.BufferLocation = m_pVertexBufferGPU->GetGPUVirtualAddress();
    vertexView.StrideInBytes = sizeof(float32) * floatStride;
    vertexView.SizeInBytes = vertexBufferSize;
    m_pVertexBuffer = new D3D12_VERTEX_BUFFER_VIEW(vertexView);

    D3D12_INDEX_BUFFER_VIEW indexView = {};
    indexView.BufferLocation = m_pIndexBufferGPU->GetGPUVirtualAddress();
    indexView.Format = DXGI_FORMAT_R32_UINT;
    indexView.SizeInBytes = indexBufferSize;
    m_pIndexBuffer = new D3D12_INDEX_BUFFER_VIEW(indexView);
}



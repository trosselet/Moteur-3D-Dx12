#include "pch.h"
#include "Mesh.h"

#include "DeviceResources.h"
#include "Geometry.h"

Mesh::Mesh(Geometry* pGeometry, Render::DeviceResources* pRender) :
	m_pGeometry(pGeometry), m_pRender(pRender)
{
	UploadGeometry();
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
		delete m_pIndexBufferUploader;
		m_pIndexBufferUploader = nullptr;
	}
	
	if (m_pVertexBufferUploader)
	{
		delete m_pVertexBufferUploader;
		m_pVertexBufferUploader = nullptr;
	}
}

void Mesh::UploadGeometry()
{
	if (!m_pGeometry) return;

	std::vector<float32> geometryData;

	bool hasColor = !m_pGeometry->colors.empty();
	bool hasUV = !m_pGeometry->UVs.empty();
	bool hasNormal = !m_pGeometry->normals.empty();

	int32 size = 3;

	geometryData.reserve(m_pGeometry->positions.size() * size);

	for (int i = 0; i < m_pGeometry->positions.size(); i++)
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

	UploadBuffers(geometryData.data(), static_cast<UINT>(geometryData.size()), m_pGeometry->indexes.data(), static_cast<UINT>(m_pGeometry->indexes.size()));

	m_pVertexBuffer->BufferLocation = m_pVertexBufferUploader->GetResource()->GetGPUVirtualAddress();
	m_pVertexBuffer->StrideInBytes = sizeof(float32);
	m_pVertexBuffer->SizeInBytes = static_cast<UINT>(geometryData.size() * sizeof(float32));

	m_pIndexBuffer->BufferLocation = m_pIndexBufferUploader->GetResource()->GetGPUVirtualAddress();
	m_pIndexBuffer->SizeInBytes = static_cast<UINT>(m_pGeometry->indexes.size() * sizeof(UINT));
	m_pIndexBuffer->Format = DXGI_FORMAT_R32_UINT;

	m_vertexCount = static_cast<UINT>(geometryData.size());
	m_indexCount = static_cast<UINT>(m_pGeometry->indexes.size());
}

void Mesh::UploadBuffers(float32* vertices, UINT vertexCount, uint32* indices, UINT indexCount)
{
	m_vertexCount = vertexCount;
	m_indexCount = indexCount;

	m_pVertexBufferUploader = new Render::UploadBuffer<float32>(m_pRender->GetDevice(), m_vertexCount, false);

	for (UINT i = 0; i < m_vertexCount; ++i)
	{
		m_pVertexBufferUploader->CopyData(i, vertices[i]);
	}


	m_pIndexBufferUploader = new Render::UploadBuffer<uint32>(m_pRender->GetDevice(), m_indexCount, false);

	for (UINT i = 0; i < m_indexCount; ++i)
	{
		m_pIndexBufferUploader->CopyData(i, indices[i]);
	}
}

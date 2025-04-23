#include "pch.h"
#include "TShape.h"

Render::Shape::Shape() : m_vbv(), m_ibv(), m_pIndexBuffer(nullptr), m_pVertexBuffer(nullptr), m_isInitialize(false)
{
}

Render::Shape::~Shape()
{
}

void Render::Shape::Draw(ID3D12Device* pDevice, ID3D12GraphicsCommandList* commandList)
{
	if (!m_pDevice)
		m_pDevice = pDevice;

	if (!m_isInitialize)
	{
		BuildBuffers();

		m_vbv = {};
		m_vbv.BufferLocation = m_pVertexBuffer->GetResource()->GetGPUVirtualAddress();
		m_vbv.StrideInBytes = sizeof(Vertex);
		m_vbv.SizeInBytes = m_vertexCount;

		m_ibv = {};
		m_ibv.BufferLocation = m_pIndexBuffer->GetResource()->GetGPUVirtualAddress();
		m_ibv.SizeInBytes = m_indexCount * sizeof(UINT);
		m_ibv.Format = DXGI_FORMAT_R32_UINT;
	}

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &m_vbv);
	commandList->IASetIndexBuffer(&m_ibv);
	commandList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
}

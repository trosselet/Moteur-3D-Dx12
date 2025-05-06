#include "pch.h"
#include "TShape.h"
#include "GraphicEngine.h"
#include "Geometry.h"

Render::Shape::Shape() : m_vbv(), m_ibv(), m_pIndexBuffer(nullptr), m_pVertexBuffer(nullptr), m_isInitialize(false), 
m_indexCount(0), m_vertexCount(0), m_pDevice(nullptr), m_pConstantBuffer(nullptr), m_color(Color::White), m_pTextureHeap(nullptr)
{
	m_pAssetManager = AssetManager::Get();
}

Render::Shape::~Shape()
{

	if (m_pVertexBuffer)
	{
		delete m_pVertexBuffer;
		m_pVertexBuffer = nullptr;
	}

	if (m_pIndexBuffer)
	{
		delete m_pIndexBuffer;
		m_pIndexBuffer = nullptr;
	}

	if (m_pConstantBuffer)
	{
		delete m_pConstantBuffer;
		m_pConstantBuffer = nullptr;
	}


	m_pDevice = nullptr;
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
		m_vbv.SizeInBytes = m_vertexCount * sizeof(Vertex);

		m_ibv = {};
		m_ibv.BufferLocation = m_pIndexBuffer->GetResource()->GetGPUVirtualAddress();
		m_ibv.SizeInBytes = m_indexCount * sizeof(UINT);
		m_ibv.Format = DXGI_FORMAT_R32_UINT;

		m_pConstantBuffer = new UploadBuffer<ObjectData>(pDevice, 1, true);

		m_isInitialize = true;
	}

	if (!m_transform.mIsUpdated)
	{
		m_transform.UpdateMatrix();
	}

	ObjectData objConstants = {};
	DirectX::XMStoreFloat4x4(&objConstants.world, DirectX::XMMatrixTranspose(m_transform.GetMatrix()));
	m_pConstantBuffer->CopyData(0, objConstants);

	commandList->SetGraphicsRootConstantBufferView(1, m_pConstantBuffer->GetResource()->GetGPUVirtualAddress());

	ID3D12DescriptorHeap* heaps[] = { m_pTextureHeap };
	commandList->SetDescriptorHeaps(_countof(heaps), heaps);
	commandList->SetGraphicsRootDescriptorTable(2, m_pTextureHeap->GetGPUDescriptorHandleForHeapStart());


	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &m_vbv);
	commandList->IASetIndexBuffer(&m_ibv);
	commandList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
}

void Render::Shape::UploadBuffers(Vertex* vertices, UINT vertexCount, UINT* indices, UINT indexCount)
{
	m_vertexCount = vertexCount;
	m_indexCount = indexCount;

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
}

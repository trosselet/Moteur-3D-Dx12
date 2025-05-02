#include "pch.h"
#include "Render.h"

#include "Window.h"
#include "Mesh.h"
#include "Material.h"
#include "DeviceResources.h"
#include "PipelineStateObjectManager.h"


Render::Render(Window* const pWindow, uint32 const renderWidth, uint32 const renderHeight) :
	m_pWindow(pWindow), 
	m_renderWidth(renderWidth),
	m_renderHeight(renderHeight),
	m_pCbCurrentViewProjInstance(nullptr)
	
{
	m_pDeviceResources = new DeviceResources();
	m_pPSOManager = new PipelineStateObjectManager();

	m_pDeviceResources->Initialize(m_pWindow->GetHWND(), m_renderWidth, m_renderHeight);
	m_pPSOManager->Initialize(m_pDeviceResources->GetDevice());
	m_pPSOManager->CreatePipelineState("../Gameplay/shader/DefaultShader.hlsl", L"../Gameplay/shader/DefaultShader.hlsl");

	m_pDeviceResources->Resize(renderWidth, renderHeight);

	m_pCbCurrentViewProjInstance = new UploadBuffer<CameraCB>(m_pDeviceResources->GetDevice(), 1, 1);

}

Render::~Render()
{
}

DeviceResources* Render::GetDeviceResources()
{
	return m_pDeviceResources;
}

bool Render::Clear()
{
	if (!m_pDeviceResources->ResetCommandList())
		return false;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pDeviceResources->GetCurrentRenderTarget();
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	m_pDeviceResources->GetCommandList()->ResourceBarrier(1, &barrier);

	D3D12_VIEWPORT viewport = m_pDeviceResources->GetViewport();
	D3D12_RECT rect = m_pDeviceResources->GetRect();
	m_pDeviceResources->GetCommandList()->RSSetViewports(1, &viewport);
	m_pDeviceResources->GetCommandList()->RSSetScissorRects(1, &rect);

	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pDeviceResources->GetCurrentRTV();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pDeviceResources->GetCurrentDSV();

	m_pDeviceResources->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	/*PRINT_CONSOLE_OUTPUT("[RENDER]: Clear render target !!!!!!!\n")*/
	m_pDeviceResources->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_pDeviceResources->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 1, &rect);

	ID3D12DescriptorHeap* descHeap = m_pDeviceResources->GetCbvSrvUavDescriptorHeap();
	m_pDeviceResources->GetCommandList()->SetDescriptorHeaps(1, &descHeap);

	return true;
}

bool Render::DrawObject(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objworldMatrix)
{
	assert(pMaterial != nullptr);
	assert(pMesh != nullptr);

	PipelineStateObjectManager::PipelineStateConfig* pShader = pMaterial->GetShader();
	assert(pShader != nullptr);

	pMaterial->UpdateWorldConstantBuffer(DirectX::XMLoadFloat4x4(&objworldMatrix));

	m_pDeviceResources->GetCommandList()->SetPipelineState(pShader->pipelineState);
	m_pDeviceResources->GetCommandList()->SetGraphicsRootSignature(pShader->rootSignature);

	m_pDeviceResources->GetCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D12_VERTEX_BUFFER_VIEW pVbv = pMesh->GetVertexBuffer();
	m_pDeviceResources->GetCommandList()->IASetVertexBuffers(0, 1, &pVbv);

	D3D12_INDEX_BUFFER_VIEW pIbv = pMesh->GetIndexBuffer();
	m_pDeviceResources->GetCommandList()->IASetIndexBuffer(&pIbv);

	pMaterial->UpdateTexture(m_pPSOManager->GetShaderPosition());

	m_pDeviceResources->GetCommandList()->SetGraphicsRootConstantBufferView(0, m_pCbCurrentViewProjInstance->GetResource()->GetGPUVirtualAddress());
	m_pDeviceResources->GetCommandList()->SetGraphicsRootConstantBufferView(1, pMaterial->GetUploadBuffer()->GetResource()->GetGPUVirtualAddress());

	// TODO ADD NORMAL
	
	m_pDeviceResources->GetCommandList()->DrawIndexedInstanced(pMesh->GetIndexCount(), 1, 0, 0, 0);

	return true;
}

bool Render::Display()
{
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_pDeviceResources->GetCurrentRenderTarget();
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	m_pDeviceResources->GetCommandList()->ResourceBarrier(1, &barrier);

	m_pDeviceResources->GetCommandList()->Close();
	m_pDeviceResources->ExecuteTheCommandList();

	m_pDeviceResources->Present(false);

	m_pDeviceResources->WaitForSynchronisation();

	return true;
}

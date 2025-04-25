#include "pch.h"
#include "GraphicEgine.h"
#include "DeviceResources.h"
#include "TDrawable.h"
#include "PipelineStateObjectManager.h"

namespace Render
{
	GraphicEgine::GraphicEgine() : m_pDeviceResources(nullptr), m_pPsoManager(nullptr), m_rect(), m_viewport()
	{
	}

	GraphicEgine::~GraphicEgine()
	{
		if (m_pPsoManager)
		{
			delete m_pPsoManager;
		}

		if (m_pDeviceResources)
		{
			delete m_pDeviceResources;
		}

		ReportLiveD3D12Objects();

	}

	void GraphicEgine::Initialize(HWND hwnd, UINT width, UINT height)
	{
		m_pDeviceResources = new DeviceResources();
		m_pDeviceResources->Initialize(hwnd, width, height);

		m_pPsoManager = new PipelineStateObjectManager();

		m_pPsoManager->Initialize(m_pDeviceResources->GetDevice());

		m_pPsoManager->CreatePipelineState("../Game/shader/DefaultShader.hlsl", L"../Game/shader/DefaultShader.hlsl");

		m_isInitialize = true;

	}

	void GraphicEgine::BeginFrame(Color clearColor)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_pDeviceResources->GetCurrentRenderTarget();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		m_pDeviceResources->GetCommandList()->ResourceBarrier(1, &barrier);

		m_pDeviceResources->ClearCurrentRenderTarget(clearColor);

		const float color[4] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pDeviceResources->GetCurrentRTV();
		m_pDeviceResources->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
		m_pDeviceResources->GetCommandList()->ClearRenderTargetView(rtvHandle, color, 0, nullptr);

		m_pDeviceResources->GetCommandList()->RSSetViewports(1, &m_viewport);
		m_pDeviceResources->GetCommandList()->RSSetScissorRects(1, &m_rect);

	}

	void GraphicEgine::RenderFrame(Core::IDrawable& drawable, const char* shaderPath)
	{

		PipelineStateConfig* psoConfig = m_pPsoManager->Get(shaderPath);

		if (psoConfig != nullptr)
		{
			m_pDeviceResources->GetCommandList()->SetPipelineState(psoConfig->pipelineState);
			m_pDeviceResources->GetCommandList()->SetGraphicsRootSignature(psoConfig->rootSignature);

			drawable.Draw(m_pDeviceResources->GetDevice(), m_pDeviceResources->GetCommandList());
		}
		else
		{
			PRINT_CONSOLE_OUTPUT("[RENDER]: Error PSO not found")
		}
	}

	void GraphicEgine::EndFrame()
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

		m_pDeviceResources->ResetCommandList();
	}

	void GraphicEgine::ResizeWindow(UINT width, UINT height)
	{
		m_pDeviceResources->Resize(width, height);
		SetViewport(width, height);
	}

	void GraphicEgine::SetViewport(UINT width, UINT height)
	{
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width = width;
		m_viewport.Height = height;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_rect.left = 0;
		m_rect.right = m_viewport.Width;
		m_rect.top = 0;
		m_rect.bottom = m_viewport.Height;
	}

	void GraphicEgine::ReportLiveD3D12Objects()
	{
		IDXGIDebug1* debug = nullptr;

		if (DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)) == S_OK)
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
			debug->Release();
			debug = nullptr;
		}

	}
}

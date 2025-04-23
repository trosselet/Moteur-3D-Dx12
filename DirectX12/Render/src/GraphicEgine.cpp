#include "pch.h"
#include "GraphicEgine.h"
#include "DeviceResources.h"
#include "TDrawable.h"

namespace Render
{
	GraphicEgine::GraphicEgine() : m_pDeviceResources(nullptr)
	{
	}

	GraphicEgine::~GraphicEgine()
	{
		if (m_pDeviceResources)
		{
			delete m_pDeviceResources;
		}
	}

	void GraphicEgine::Initialize(HWND hwnd, UINT width, UINT height)
	{
		m_pDeviceResources = new DeviceResources();
		m_pDeviceResources->Initialize(hwnd, width, height);

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

	}

	void GraphicEgine::RenderFrame(Core::IDrawable* drawable)
	{
		drawable->Draw(m_pDeviceResources->GetCommandList());
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

		m_pDeviceResources->Present(true);

		m_pDeviceResources->WaitForSynchronisation();

		m_pDeviceResources->ResetCommandList();
	}

	void GraphicEgine::ResizeWindow(UINT width, UINT height)
	{
		m_pDeviceResources->Resize(width, height);
	}
}

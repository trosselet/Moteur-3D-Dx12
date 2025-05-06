#ifndef DEVICE_RESOURCES__H
#define DEVICE_RESOURCES__H

#include "Define.h"
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

class DeviceResources
{
public:
	static const UINT FrameCount = 2;

	DeviceResources();
	~DeviceResources();

	void Initialize(HWND hwnd, UINT width, UINT height);
	void Resize(UINT width, UINT height);
	void Present(bool vsync);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDSV() const;
	ID3D12Resource* GetCurrentRenderTarget();
	void ExecuteTheCommandList();

	ID3D12Fence* GetD3D12Fence() { return m_pFence.Get(); };
	UINT64 GetD3D12CurrentFenceValue() { return m_currentFenceValue; };

	bool ResetCommandList();

	ID3D12Device* GetDevice() const { return m_pDevice.Get(); }
	ID3D12CommandAllocator* GetAllocator() const { return m_pCommandAllocator.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return m_pCommandList.Get(); }
	ID3D12CommandQueue* GetCommandQueue() const { return m_pCommandQueue.Get(); }
	ID3D12DescriptorHeap* GetCbvSrvUavDescriptorHeap() const { return m_pCbvSrvUavDescriptorHeap.Get(); }
	D3D12_VIEWPORT GetViewport() const { return m_screenViewport; }
	D3D12_RECT GetRect() const { return m_scissorRect; }

	void WaitForSynchronisation();

	void DebugSetName();
	void FlushQueue(UINT bufferCount);

	void UpdateViewport();

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer,
		D3D12_RESOURCE_STATES finalState
	);

	UINT AllocateSRVHeapIndex() { return m_srvHeapIndex++; }

private:
	void ReleaseResources();
	void CreateDevice();
	void CreateCommandQueue();
	void CreateCommandList();
	void CreateSwapChain(HWND hwnd, UINT width, UINT height);
	void CreateRenderTargets();
	void CreateDepthStencilResources(UINT width, UINT height);
	void CreateCbvSrvUavDescriptorHeap();

private:
	Microsoft::WRL::ComPtr<IDXGIFactory2> m_pFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter> m_pAdapter;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_pSwapChain;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_pRenderTargets[FrameCount];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pRtvDescriptorHeap;

	UINT m_bufferCount = FrameCount;
	UINT m_currentSwapChainBuffer = 0;
	UINT m_rtvHeapIncrement = 0;

	uint32 m_srvHeapIndex = 0;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pDsvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pDepthStencil;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pCbvSrvUavDescriptorHeap;

	Microsoft::WRL::ComPtr<ID3D12Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_pCommandQueue;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_pCommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	Microsoft::WRL::ComPtr<ID3D12Fence> m_pFence;
	UINT64 m_currentFenceValue = 0;
	HANDLE m_fenceEvent = nullptr;

	uint16 m_renderWidth = 1920;
	uint16 m_renderHeight = 1080;

	D3D12_VIEWPORT m_screenViewport{};
	D3D12_RECT m_scissorRect{};
};

#endif // !DEVICE_RESOURCES__H

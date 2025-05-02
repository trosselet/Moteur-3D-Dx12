#ifndef DEVICE_RESOURCES__H
#define DEVICE_RESOURCES__H

#include "Define.h"

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

	ID3D12Fence* GetD3D12Fence() { return m_pFence; };
	UINT64 GetD3D12CurrentFenceValue() { return m_currentFenceValue; };

	bool ResetCommandList();

	ID3D12Device* GetDevice() const { return m_pDevice; }
	ID3D12CommandAllocator* GetAllocator() const { return m_pCommandAllocator; }
	ID3D12GraphicsCommandList* GetCommandList() const { return m_pCommandList; }
	ID3D12CommandQueue* GetCommandQueue() const { return m_pCommandQueue; };
	ID3D12DescriptorHeap* GetCbvSrvUavDescriptorHeap() const { return m_pCbvSrvUavDescriptorHeap; };
	D3D12_VIEWPORT GetViewport() const { return m_screenViewport; };
	D3D12_RECT GetRect() const { return m_scissorRect; };

	void WaitForSynchronisation();

	void DebugSetName();
	void FlushQueue(UINT bufferCount);

	void UpdateViewport();

	ID3D12Resource* CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize, ID3D12Resource*& uploadBuffer, D3D12_RESOURCE_STATES finalState);

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

	IDXGIFactory2* m_pFactory;
	IDXGIAdapter* m_pAdapter;
	IDXGISwapChain3* m_pSwapChain;

	ID3D12Resource* m_pRenderTargets[FrameCount];
	ID3D12DescriptorHeap* m_pRtvDescriptorHeap;

	UINT m_bufferCount;
	UINT m_currentSwapChainBuffer;
	UINT m_rtvHeapIncrement;

	ID3D12DescriptorHeap* m_pDsvDescriptorHeap;
	ID3D12Resource* m_pDepthStencil;

	ID3D12DescriptorHeap* m_pCbvSrvUavDescriptorHeap;

	ID3D12Device* m_pDevice;
	ID3D12CommandQueue* m_pCommandQueue;

	ID3D12CommandAllocator* m_pCommandAllocator;
	ID3D12GraphicsCommandList* m_pCommandList;

	ID3D12Fence* m_pFence;
	UINT64 m_currentFenceValue;
	HANDLE m_fenceEvent;

	uint16 m_renderWidth = 1920;
	uint16 m_renderHeight = 1080;

	D3D12_VIEWPORT m_screenViewport{};
	D3D12_RECT m_scissorRect{};
};


#endif // !DEVICE_RESOURCES__H
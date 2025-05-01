#ifndef DEVICE_RESOURCES__H
#define DEVICE_RESOURCES__H

namespace Render
{
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

		void ResetCommandList();

		ID3D12Device* GetDevice() const { return m_pDevice; }
		ID3D12CommandAllocator* GetAllocator() const { return m_pCommandAllocator; }
		ID3D12GraphicsCommandList* GetCommandList() const { return m_pCommandList; }
		ID3D12CommandQueue* GetCommandQueue() const { return m_pCommandQueue; };

		void WaitForSynchronisation();

		void DebugSetName();
		void FlushQueue(UINT bufferCount);

	private:
		void ReleaseResources();
		void CreateDevice();
		void CreateCommandQueue();
		void CreateCommandList();
		void CreateSwapChain(HWND hwnd, UINT width, UINT height);
		void CreateRenderTargets();
		void CreateDepthStencilResources(UINT width, UINT height);


	private:

		IDXGIFactory2* m_pFactory;
		IDXGIAdapter* m_pAdapter;
		IDXGISwapChain1* m_pSwapChain;

		ID3D12Resource* m_pRenderTargets[FrameCount];
		ID3D12DescriptorHeap* m_pRtvDescriptorHeap;

		UINT m_bufferCount;
		UINT m_currentSwapChainBuffer;
		UINT m_rtvHeapIncrement;

		ID3D12DescriptorHeap* m_pDsvDescriptorHeap;
		ID3D12Resource* m_pDepthStencil;


		ID3D12Device* m_pDevice;
		ID3D12CommandQueue* m_pCommandQueue;

		ID3D12CommandAllocator* m_pCommandAllocator;
		ID3D12GraphicsCommandList* m_pCommandList;

		ID3D12Fence* m_pFence;
		UINT64 m_currentFenceValue;
		HANDLE m_fenceEvent;
		
	};
}


#endif // !DEVICE_RESOURCES__H
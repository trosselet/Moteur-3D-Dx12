#ifndef DEVICE_RESOURCES__H
#define DEVICE_RESOURCES__H



namespace Render
{
	class FramesResources;

	class DeviceResources
	{
	public:
		static const UINT FrameCount = 2;

		DeviceResources();
		~DeviceResources();

		void Initialize(HWND hwnd, UINT width, UINT height);
		void Resize(UINT width, UINT height);
		void WaitForGPU();
		void Present(bool vsync);
		void Release();

		inline ID3D12Device* GetDevice() const { return m_pDevice; };
		inline ID3D12CommandQueue* GetCommandQueue() const { return m_pCommandQueue; };
		inline FramesResources* GetCurrentFrameResources() { return m_frameResources[m_frameIndex]; };
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;
		inline UINT GetCurrentFrameIndex() const { return m_frameIndex; };

		void Upload(ID3D12Resource* pDest, void* pData, const UINT size, ID3D12CommandList* pCommandList, ID3D12CommandQueue* pCommandQueue, D3D12_RESOURCE_STATES state);

		void ExecuteCommandList();

		void WaitForTemporaryGPUExecution(ID3D12CommandQueue* commandQueue);
		void ExecuteUploadCommandList();

	private:
		void CreateDevice();
		void CreateCommandQueue();
		void CreateSwapChain(HWND hwnd, UINT width, UINT height);
		void CreateRenderTargets();
		void CreateFrameResources();
		void MoveToNextFrame();

	private:
		ID3D12Device* m_pDevice;
		IDXGIFactory7* m_pFactory;
		IDXGISwapChain3* m_pSwapChain;
		ID3D12CommandQueue* m_pCommandQueue;

		ID3D12DescriptorHeap* m_pRenderTargetViewHeap;
		UINT m_renderTargetViewDescriptorSize;
		ID3D12Resource* m_renderTargets[FrameCount];

		FramesResources* m_frameResources[FrameCount];
		UINT m_frameIndex;

		ID3D12Fence* m_fence;
		UINT64 m_fenceValue;
		HANDLE m_fenceEvent;

		ID3D12CommandAllocator* m_uploadCommandAllocator;
		ID3D12GraphicsCommandList* m_uploadCommandList;


	};
}


#endif // !DEVICE_RESOURCES__H
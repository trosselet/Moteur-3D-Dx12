#pragma once

class DXGIManager
{
public:
	DXGIManager() = default;
	~DXGIManager();

	void InitializeFactory();
	void InitializeAdapter();
	void InitializeSwapChain(ID3D12Device* pDevice, IDXGIFactory2* pFactory, ID3D12CommandQueue* pCommandQueue, const HWND hwnd, const UINT width, const UINT height);

	D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetViewHandle();
	ID3D12Resource* GetCurrentRenderTarget();
	void Present(bool vsync);

	inline IDXGIFactory2* GetDXGIFactory() { return mFactory; };
	inline IDXGIAdapter* GetDXGIAdapter() { return mAdapter; };
	inline IDXGISwapChain1* GetDXGISwapChain() { return mSwapChain; };
	inline UINT GetBufferCount() { return mBufferCount; };

	void Release();


private:
	void CreateBuffer(ID3D12Device* pDevice);

private:
	IDXGIFactory2* mFactory;
	IDXGIAdapter* mAdapter;
	IDXGISwapChain1* mSwapChain;

	ID3D12Resource* mRenderTargets[2];
	ID3D12DescriptorHeap* mRtvDescriptorHeap;

	UINT mBufferCount;
	UINT mCurrentSwapChainBuffer;
	UINT mRtvHeapIncrement;
};
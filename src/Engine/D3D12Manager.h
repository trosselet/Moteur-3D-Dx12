#pragma once

class D3D12Manager
{

private:
	enum ShaderType
	{
		VERTEX,
		PIXEL,
	};

public:
	D3D12Manager() = default;
	~D3D12Manager();

	void D3D12DeviceInitialize(IDXGIAdapter* pAdapter);
	void D3D12CommandQueueInitialize(ID3D12Device* pDevice);
	void D3D12CommandListInitialize(ID3D12Device* pDevice);
	void D3D12ShaderInitialize(const LPCWSTR filename, const ShaderType shaderType, ID3DBlob** ppBlob);
	void D3D12RootSignatureInitialize(ID3D12Device* pDevice);
	void D3D12PipelineState3DObjectInitialize(ID3D12Device* pDevice);
	void D3D12PipelineStateUiElementInitialize(ID3D12Device* pDevice);
	void D3D12PipelineStatePostProcessInitialize(ID3D12Device* pDevice);

	void BufferInitialize(ID3D12Device* pDevice, unsigned int numberOfBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, ID3D12Resource** ppRessource);
	void DepthBufferInitialize(ID3D12Device* pDevice, const unsigned int width, const unsigned int height, ID3D12Resource** ppRessource);
	void DepthDescriptorHeapInitialize(ID3D12Device* pDevice);
	void DepthStencilViewInitialize(ID3D12Device* pDevice);

	void Upload(ID3D12Resource* pDest, void* pData, const UINT size, ID3D12CommandList* pCommandList, ID3D12CommandQueue* pCommandQueue, D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON);

	ID3D12GraphicsCommandList* GetGraphicsCommandList();
	void ExecuteTheCommandList(ID3D12CommandList* pCommandList);
	void ResetCommandList();
	void FlushQueue(UINT bufferCount);

	inline ID3D12Device* GetD3D12Device() { return mDevice; };
	inline ID3D12CommandQueue* GetD3D12CommandQueue() { return mCommandQueue; };
	inline ID3D12CommandList* GetD3D12CommandList() { return mCommandList; };
	inline ID3D12Fence* GetD3D12Fence() { return mFence; };
	inline UINT64 GetD3D12CurrentFenceValue() { return mCurrentFenceValue; };
	inline ID3D12CommandAllocator* GetD3D12CommandAllocator() { return mAllocator; };
	
	inline ID3D12RootSignature* GetD3D12RootSignature() { return mRootSignature; };
	
	inline ID3D12PipelineState* GetD3D12PipelineState3DObject() { return mPipelineState3DObject; };
	inline ID3D12PipelineState* GetD3D12PipelineStateUiElement() { return mPipelineStateUiElement; };
	inline ID3D12PipelineState* GetPostProcessPipelineState() { return mPipelineStatePostProcessing; }


	inline ID3D12Resource** GetD3D12DepthBufferPointer() { return &mDepthBuffer; };
	inline ID3D12DescriptorHeap* GetD3D12DepthDescriptorHeap() { return mDepthDescriptorHeap; };


	void Release();

private:
	ID3D12Device* mDevice;
	ID3D12CommandQueue* mCommandQueue;
	ID3D12CommandList* mCommandList;
	ID3D12Fence* mFence;
	ID3D12CommandAllocator* mAllocator;

	ID3D12RootSignature* mRootSignature;

	ID3D12PipelineState* mPipelineState3DObject;
	ID3D12PipelineState* mPipelineStateUiElement;
	ID3D12PipelineState* mPipelineStatePostProcessing;

	ID3DBlob* mBlob;
	ID3DBlob* mShader[2];
	ID3DBlob* mShaderUI[2];
	ID3DBlob* mShaderPostProcess[2];

	ID3D12Resource* mDepthBuffer;
	ID3D12DescriptorHeap* mDepthDescriptorHeap;

	UINT64 mCurrentFenceValue;

};
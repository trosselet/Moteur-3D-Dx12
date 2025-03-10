#include "pch.h"
#include "DXGIManager.h"

DXGIManager::~DXGIManager()
{
	
}


void DXGIManager::InitializeFactory()
{
#ifdef _DEBUG
	if (CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&mFactory)) != S_OK)
#else
	if (CreateDXGIFactory2(0, IID_PPV_ARGS(&mFactory)) != S_OK)
#endif
	{
		PRINT_CONSOLE_OUTPUT(" Error creating DXGIFactory, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}
}

void DXGIManager::InitializeAdapter()
{
	IDXGIFactory6* pFactory6;

	if (mFactory->QueryInterface(IID_PPV_ARGS(&pFactory6)) == S_OK)
	{
		if (pFactory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&mAdapter)) != S_OK)
		{
			PRINT_CONSOLE_OUTPUT(" Error finding the adapter, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
		}
	}
	else
	{
		_ASSERT_EXPR(false, "Not compatible with factory 6");
	}
}

void DXGIManager::InitializeSwapChain(ID3D12Device* pDevice, IDXGIFactory2* pFactory, ID3D12CommandQueue* pCommandQueue, const HWND hwnd, const UINT width, const UINT height)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = 2;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NodeMask = 0;

	if (pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&mRtvDescriptorHeap)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT(" Error creating the descriptor heap, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}
	mRtvHeapIncrement = pDevice->GetDescriptorHandleIncrementSize(heapDesc.Type);



	DXGI_SWAP_CHAIN_DESC1 desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Stereo = false;
	desc.SampleDesc = { 1, 0 };
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferCount = 2;
	desc.Scaling = DXGI_SCALING_NONE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if (pFactory->CreateSwapChainForHwnd(pCommandQueue, hwnd, &desc, nullptr, nullptr, &mSwapChain) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT(" Error creating the swap chain, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}

	mBufferCount = 2;

	CreateBuffer(pDevice);
}

D3D12_CPU_DESCRIPTOR_HANDLE DXGIManager::GetRenderTargetViewHandle()
{
	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle = mRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	cpuDescHandle.ptr += (size_t)mRtvHeapIncrement * mCurrentSwapChainBuffer;

	return cpuDescHandle;
}

ID3D12Resource* DXGIManager::GetCurrentRenderTarget()
{
	return mRenderTargets[mCurrentSwapChainBuffer];
}

void DXGIManager::Present(bool vsync)
{
	mSwapChain->Present(vsync, 0);

	mCurrentSwapChainBuffer = (mCurrentSwapChainBuffer + 1) % mBufferCount;
}

void DXGIManager::Release()
{
	if (mSwapChain) { mSwapChain->Release(); mSwapChain = nullptr; }
	if (mFactory) { mFactory->Release(); mFactory = nullptr; }
	if (mAdapter) { mAdapter->Release(); mAdapter = nullptr; }

	for (int i = 0; i < 2; i++)
	{
		if (mRenderTargets[i])
		{
			mRenderTargets[i]->Release();
			mRenderTargets[i] = nullptr;
		}
	}

	if (mRtvDescriptorHeap)
	{
		mRtvDescriptorHeap->Release();
		mRtvDescriptorHeap = nullptr;
	}
}

void DXGIManager::CreateBuffer(ID3D12Device* pDevice)
{
	if (!mSwapChain)
		return;

	for (int i = 0; i < mBufferCount; i++)
	{
		if (mRenderTargets[i])
		{
			mRenderTargets[i]->Release();
			mRenderTargets[i] = nullptr;
		}

		if (mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mRenderTargets[i])) != S_OK)
		{
			PRINT_CONSOLE_OUTPUT("API Error: Error creating the buffer for the swapchain, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		cpuHandle.ptr += (size_t)mRtvHeapIncrement * i;

		pDevice->CreateRenderTargetView(mRenderTargets[i], 0, cpuHandle);

	}
}
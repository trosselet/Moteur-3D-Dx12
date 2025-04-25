#include "pch.h"
#include "DeviceResources.h"

Render::DeviceResources::DeviceResources() : m_bufferCount(0), m_currentFenceValue(0), m_currentSwapChainBuffer(0), 
m_fenceEvent(nullptr), m_pAdapter(nullptr), m_pDevice(nullptr), m_pCommandQueue(nullptr), m_pCommandAllocator(nullptr), 
m_pCommandList(nullptr), m_pSwapChain(nullptr), m_pFactory(nullptr), m_pFence(nullptr), m_pRenderTargets(), 
m_pRtvDescriptorHeap(nullptr), m_rtvHeapIncrement(0)
{
}

Render::DeviceResources::~DeviceResources()
{
    FlushQueue(FrameCount);
    ReleaseResources();
}

void Render::DeviceResources::Initialize(HWND hwnd, UINT width, UINT height)
{
    CreateDevice();
    CreateCommandQueue();
    CreateCommandList();
    CreateSwapChain(hwnd, width, height);
    m_currentSwapChainBuffer = 0;

    m_currentFenceValue = 0;

    if (m_pDevice->CreateFence(m_currentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating the Device Resources fence, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
    }
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

}

void Render::DeviceResources::Present(bool vsync)
{
    m_pSwapChain->Present(vsync, 0);

    m_currentSwapChainBuffer = (m_currentSwapChainBuffer + 1) % FrameCount;
}

D3D12_CPU_DESCRIPTOR_HANDLE Render::DeviceResources::GetCurrentRTV() const
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    cpuDescHandle.ptr += (size_t)m_rtvHeapIncrement * m_currentSwapChainBuffer;

    return cpuDescHandle;
}

ID3D12Resource* Render::DeviceResources::GetCurrentRenderTarget()
{
    return m_pRenderTargets[m_currentSwapChainBuffer];
}

void Render::DeviceResources::ExecuteTheCommandList()
{

    ID3D12CommandList* const* cmdList = (ID3D12CommandList* const*)&m_pCommandList;

    m_pCommandQueue->ExecuteCommandLists(1, cmdList);
    m_currentFenceValue++;
    m_pCommandQueue->Signal(m_pFence, m_currentFenceValue);

}

void Render::DeviceResources::ClearCurrentRenderTarget(Color color)
{
    
}

void Render::DeviceResources::ResetCommandList()
{
    m_pCommandAllocator->Reset();
    GetCommandList()->Reset(m_pCommandAllocator, 0);
}

void Render::DeviceResources::Resize(UINT width, UINT height)
{
    WaitForSynchronisation();

    for (UINT i = 0; i < FrameCount; ++i)
    {
        if (m_pRenderTargets[i])
        {
            m_pRenderTargets[i]->Release();
            m_pRenderTargets[i] = nullptr;
        }
    }

    DXGI_SWAP_CHAIN_DESC desc = {};
    IDXGISwapChain* pBaseSwapChain = nullptr;
    if (m_pSwapChain->QueryInterface(IID_PPV_ARGS(&pBaseSwapChain)) == S_OK)
    {
        pBaseSwapChain->GetDesc(&desc);
        pBaseSwapChain->Release();
    }

    if (m_pSwapChain->ResizeBuffers(FrameCount, width, height, desc.BufferDesc.Format, desc.Flags) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to resize swap chain buffers.");
        return;
    }

    IDXGISwapChain3* pSwapChain3 = nullptr;
    if (m_pSwapChain->QueryInterface(IID_PPV_ARGS(&pSwapChain3)) == S_OK)
    {
        m_currentSwapChainBuffer = pSwapChain3->GetCurrentBackBufferIndex();
        pSwapChain3->Release();
    }
    else
    {
        m_currentSwapChainBuffer = 0;
    }

    CreateRenderTargets();
}



void Render::DeviceResources::WaitForSynchronisation()
{
    ID3D12Fence* fence = GetD3D12Fence();
    UINT64 fenceValue = GetD3D12CurrentFenceValue() + 1;
    GetCommandQueue()->Signal(fence, fenceValue);

    if (fence->GetCompletedValue() < fenceValue)
    {
        HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEvent)
        {
            fence->SetEventOnCompletion(fenceValue, fenceEvent);
            WaitForSingleObject(fenceEvent, INFINITE);
            CloseHandle(fenceEvent);
        }
        else
        {
            PRINT_CONSOLE_OUTPUT("[RENDER]: Error failed to create Fence Event!" << std::endl);
        }
    }
}

void Render::DeviceResources::ReleaseResources()
{
    WaitForSynchronisation();

    if (m_fenceEvent)
    {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }

    if (m_pFence)
    {
        m_pFence->Release();
        m_pFence = nullptr;
    }

    if (m_pCommandList)
    {
        m_pCommandList->Release();
        m_pCommandList = nullptr;
    }

    if (m_pCommandAllocator)
    {
        m_pCommandAllocator->Release();
        m_pCommandAllocator = nullptr;
    }

    if (m_pCommandQueue)
    {
        m_pCommandQueue->Release();
        m_pCommandQueue = nullptr;
    }

    for (UINT i = 0; i < FrameCount; ++i)
    {
        if (m_pRenderTargets[i])
        {
            m_pRenderTargets[i]->Release();
            m_pRenderTargets[i] = nullptr;
        }
    }

    if (m_pRtvDescriptorHeap)
    {
        m_pRtvDescriptorHeap->Release();
        m_pRtvDescriptorHeap = nullptr;
    }

    if (m_pSwapChain)
    {
        m_pSwapChain->Release();
        m_pSwapChain = nullptr;
    }

    if (m_pAdapter)
    {
        m_pAdapter->Release();
        m_pAdapter = nullptr;
    }

    if (m_pFactory)
    {
        m_pFactory->Release();
        m_pFactory = nullptr;
    }

    if (m_pDevice)
    {
        m_pDevice->Release();
        m_pDevice = nullptr;
    }
}

void Render::DeviceResources::CreateDevice()
{
#ifdef _DEBUG

    ID3D12Debug* pDebugController = nullptr;
    if (D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController)) == S_OK)
    {
        pDebugController->EnableDebugLayer();
        //pDebugController->SetEnableGPUBasedValidation(TRUE);
        pDebugController->Release();
    }
    else
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating debug interface, At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
    }

    if (CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_pFactory)) != S_OK)
#else
    if (CreateDXGIFactory2(0, IID_PPV_ARGS(&m_pFactory)) != S_OK)

#endif // _DEBUG
    {
        PRINT_CONSOLE_OUTPUT(" [RENDER]: Error creating DXGIFactory, At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
    }

    IDXGIFactory6* pFactory6 = nullptr;

    if (m_pFactory->QueryInterface(IID_PPV_ARGS(&pFactory6)) == S_OK)
    {
        if (pFactory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_pAdapter)) != S_OK)
        {
            PRINT_CONSOLE_OUTPUT(" [RENDER]: Error finding the adapter,  At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
        }

        pFactory6->Release();
        pFactory6 = nullptr;
    }
    else
    {
        _ASSERT_EXPR(false, "Not compatible with factory 6");
    }

    if (D3D12CreateDevice(m_pAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_pDevice)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT(" [RENDER]: Error creating the device,  At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
    }

}

void Render::DeviceResources::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    if (m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pCommandQueue)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT(" [RENDER]: Error creating the command queue,  At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
    }

    if (m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT(" [RENDER]: Error creating the fence,  At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
    }
}

void Render::DeviceResources::CreateCommandList()
{
    if (FAILED(m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator))))
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating command allocator, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
        return;
    }

    HRESULT hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, nullptr, IID_PPV_ARGS(&m_pCommandList));
    if (FAILED(hr))
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating command list, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
        return;
    }

    if (m_pCommandList)
    {
        hr = m_pCommandList->Close();
        if (FAILED(hr))
        {
            PRINT_CONSOLE_OUTPUT("[RENDER]: Error closing the command list after creation.");
        }
    }
}


void Render::DeviceResources::CreateSwapChain(HWND hwnd, UINT width, UINT height)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.NumDescriptors = 2;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask = 0;

    if (m_pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pRtvDescriptorHeap)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT(" [RENDER]: Error creating the descriptor heap,  At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
    }
    m_rtvHeapIncrement = m_pDevice->GetDescriptorHandleIncrementSize(heapDesc.Type);



    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Stereo = false;
    desc.SampleDesc = { 1, 0 };
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = FrameCount;
    desc.Scaling = DXGI_SCALING_NONE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    if (m_pFactory->CreateSwapChainForHwnd(m_pCommandQueue, hwnd, &desc, nullptr, nullptr, &m_pSwapChain) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT(" [RENDER]: Error creating the swap chain,  At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
    }

    m_bufferCount = 2;

    CreateRenderTargets();
}

void Render::DeviceResources::CreateRenderTargets()
{
    if (!m_pSwapChain)
        return;

    for (int i = 0; i < m_bufferCount; i++)
    {
        if (m_pRenderTargets[i])
        {
            m_pRenderTargets[i]->Release();
            m_pRenderTargets[i] = nullptr;
        }

        if (m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i])) != S_OK)
        {
            PRINT_CONSOLE_OUTPUT(" [RENDER]: Error creating the swap chain buffer,  At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
        }

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        cpuHandle.ptr += (size_t)m_rtvHeapIncrement * i;

        m_pDevice->CreateRenderTargetView(m_pRenderTargets[i], 0, cpuHandle);

    }
}

void Render::DeviceResources::FlushQueue(UINT bufferCount)
{
    if (!m_pCommandQueue || !m_pFence || !m_fenceEvent)
        return;

    m_currentFenceValue++;
    UINT64 fenceToWait = m_currentFenceValue;

    if (FAILED(m_pCommandQueue->Signal(m_pFence, fenceToWait)))
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to signal fence in FlushQueue.\n");
        return;
    }

    if (m_pFence->GetCompletedValue() < fenceToWait)
    {
        if (FAILED(m_pFence->SetEventOnCompletion(fenceToWait, m_fenceEvent)))
        {
            PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to set event on completion in FlushQueue.\n");
            return;
        }

        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}


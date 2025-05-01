#include "pch.h"
#include "DeviceResources.h"

DeviceResources::DeviceResources() : m_bufferCount(0), m_currentFenceValue(0), m_currentSwapChainBuffer(0), 
m_fenceEvent(nullptr), m_pAdapter(nullptr), m_pDevice(nullptr), m_pCommandQueue(nullptr), m_pCommandAllocator(nullptr), 
m_pCommandList(nullptr), m_pSwapChain(nullptr), m_pFactory(nullptr), m_pFence(nullptr), m_pRenderTargets(), 
m_pRtvDescriptorHeap(nullptr), m_rtvHeapIncrement(0)
{
}

DeviceResources::~DeviceResources()
{
    FlushQueue(FrameCount);
    ReleaseResources();
}

void DeviceResources::Initialize(HWND hwnd, UINT width, UINT height)
{
    CreateDevice();
    CreateCommandQueue();
    CreateCommandList();
    CreateSwapChain(hwnd, width, height);
    CreateCbvSrvUavDescriptorHeap();

    m_currentSwapChainBuffer = 0;

    m_currentFenceValue = 0;

    if (m_pDevice->CreateFence(m_currentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating the Device Resources fence, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
    }
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    DebugSetName();

}

void DeviceResources::Present(bool vsync)
{
    m_pSwapChain->Present(vsync, 0);

    m_currentSwapChainBuffer = (m_currentSwapChainBuffer + 1) % FrameCount;
}

D3D12_CPU_DESCRIPTOR_HANDLE DeviceResources::GetCurrentRTV() const
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    cpuDescHandle.ptr += (size_t)m_rtvHeapIncrement * m_currentSwapChainBuffer;

    return cpuDescHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DeviceResources::GetCurrentDSV() const
{
    return m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}

ID3D12Resource* DeviceResources::GetCurrentRenderTarget()
{
    return m_pRenderTargets[m_currentSwapChainBuffer];
}

void DeviceResources::ExecuteTheCommandList()
{

    ID3D12CommandList* const* cmdList = (ID3D12CommandList* const*)&m_pCommandList;

    m_pCommandQueue->ExecuteCommandLists(1, cmdList);
    m_currentFenceValue++;
    m_pCommandQueue->Signal(m_pFence, m_currentFenceValue);

}

void DeviceResources::ResetCommandList()
{
    m_pCommandAllocator->Reset();
    GetCommandList()->Reset(m_pCommandAllocator, 0);
}

void DeviceResources::Resize(UINT width, UINT height)
{
    WaitForSynchronisation();

    m_renderWidth = width;
    m_renderHeight = height;

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

    if (m_pDepthStencil)
    {
        m_pDepthStencil->Release();
        m_pDepthStencil = nullptr;
    }

    CreateDepthStencilResources(width, height);

    CreateRenderTargets();

    UpdateViewport();

}



void DeviceResources::WaitForSynchronisation()
{
    UINT64 fenceValue = ++m_currentFenceValue;
    m_pCommandQueue->Signal(m_pFence, fenceValue);

    if (m_pFence->GetCompletedValue() < fenceValue)
    {
        m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void DeviceResources::DebugSetName()
{
    if (m_pDevice)
    {
        m_pDevice->SetName(L"Device");
    }

    if (m_pSwapChain)
    {
        m_pSwapChain->SetPrivateData(WKPDID_D3DDebugObjectNameW, sizeof(L"SwapChain") - 2, L"SwapChain");
    }

    if (m_pCommandQueue)
    {
        m_pCommandQueue->SetName(L"CommandQueue");
    }

    if (m_pCommandAllocator)
    {
        m_pCommandAllocator->SetName(L"CommandAllocator");
    }

    if (m_pCommandList)
    {
        m_pCommandList->SetName(L"CommandList");
    }

    if (m_pFence)
    {
        m_pFence->SetName(L"Fence");
    }

    if (m_pRtvDescriptorHeap)
    {
        m_pRtvDescriptorHeap->SetName(L"RTVDescriptorHeap");
    }

    for (UINT i = 0; i < FrameCount; ++i)
    {
        if (m_pRenderTargets[i])
        {
            std::wstring name = L"RenderTarget[" + std::to_wstring(i) + L"]";
            m_pRenderTargets[i]->SetName(name.c_str());
        }
    }
}


void DeviceResources::ReleaseResources()
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

    if (m_pDepthStencil)
    {
        m_pDepthStencil->Release();
        m_pDepthStencil = nullptr;
    }

    if (m_pCbvSrvUavDescriptorHeap)
    {
        m_pCbvSrvUavDescriptorHeap->Release();
        m_pCbvSrvUavDescriptorHeap = nullptr;
    }

    if (m_pDsvDescriptorHeap)
    {
        m_pDsvDescriptorHeap->Release();
        m_pDsvDescriptorHeap = nullptr;
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

void DeviceResources::CreateDevice()
{
#ifdef _DEBUG

    ID3D12Debug* pDebugController = nullptr;
    if (D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController)) == S_OK)
    {
        pDebugController->EnableDebugLayer();
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

void DeviceResources::CreateCommandQueue()
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
}

void DeviceResources::CreateCommandList()
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


void DeviceResources::CreateSwapChain(HWND hwnd, UINT width, UINT height)
{
    m_renderWidth = width;
    m_renderHeight = height;

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
    CreateDepthStencilResources(width, height);
}

void DeviceResources::CreateRenderTargets()
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

void DeviceResources::CreateDepthStencilResources(UINT width, UINT height)
{
    D3D12_RESOURCE_DESC depthStencilDesc = {};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 1;
    heapProps.VisibleNodeMask = 1;

    if (m_pDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(&m_pDepthStencil)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to create DepthStencil buffer.\n");
    }

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (!m_pDsvDescriptorHeap)
    {
        if (m_pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_pDsvDescriptorHeap)) != S_OK)
        {
            PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to create DSV Heap.\n");
        }
    }

    m_pDevice->CreateDepthStencilView(m_pDepthStencil, nullptr, m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void DeviceResources::CreateCbvSrvUavDescriptorHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = 1000;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    rtvHeapDesc.NodeMask = 0;
    HRESULT res = m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pCbvSrvUavDescriptorHeap));
}

void DeviceResources::FlushQueue(UINT bufferCount)
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

void DeviceResources::UpdateViewport()
{
    m_screenViewport = { 0.0f, 0.0f, static_cast<float>(m_renderWidth), static_cast<float>(m_renderHeight), 0.0f, 1.0f };
    m_scissorRect = { 0, 0, m_renderWidth, m_renderHeight };
}


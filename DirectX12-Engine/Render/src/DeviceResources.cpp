#include "pch.h"
#include "DeviceResources.h"

DeviceResources::DeviceResources()
    : m_bufferCount(0),
    m_currentFenceValue(0),
    m_currentSwapChainBuffer(0),
    m_fenceEvent(nullptr),
    m_rtvHeapIncrement(0)
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

    m_currentSwapChainBuffer = m_pSwapChain->GetCurrentBackBufferIndex();
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
    return m_pRenderTargets[m_currentSwapChainBuffer].Get();
}

void DeviceResources::ExecuteTheCommandList()
{
    ID3D12CommandList* const cmdLists[] = { m_pCommandList.Get() };
    m_pCommandQueue->ExecuteCommandLists(1, cmdLists);
    m_currentFenceValue++;
    m_pCommandQueue->Signal(m_pFence.Get(), m_currentFenceValue);
}

bool DeviceResources::ResetCommandList()
{
    m_pCommandAllocator->Reset();
    GetCommandList()->Reset(m_pCommandAllocator.Get(), 0);

    return true;
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
            m_pRenderTargets[i].Reset();
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
        m_pDepthStencil.Reset();
    }

    CreateDepthStencilResources(width, height);

    CreateRenderTargets();

    UpdateViewport();

}



void DeviceResources::WaitForSynchronisation()
{
    UINT64 fenceValue = ++m_currentFenceValue;
    m_pCommandQueue->Signal(m_pFence.Get(), fenceValue);

    if (m_pFence->GetCompletedValue() < fenceValue)
    {
        m_pFence->SetEventOnCompletion(fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void DeviceResources::DebugSetName()
{
    if (m_pDevice)
        m_pDevice->SetName(L"Device");

    if (m_pAdapter)
        m_pAdapter->SetPrivateData(WKPDID_D3DDebugObjectNameW, sizeof(L"Adapter") - 2, L"Adapter");

    if (m_pFactory)
        m_pFactory->SetPrivateData(WKPDID_D3DDebugObjectNameW, sizeof(L"Factory") - 2, L"Factory");

    if (m_pSwapChain)
        m_pSwapChain->SetPrivateData(WKPDID_D3DDebugObjectNameW, sizeof(L"SwapChain") - 2, L"SwapChain");

    if (m_pCommandQueue)
        m_pCommandQueue->SetName(L"CommandQueue");

    if (m_pCommandAllocator)
        m_pCommandAllocator->SetName(L"CommandAllocator");

    if (m_pCommandList)
        m_pCommandList->SetName(L"CommandList");

    if (m_pFence)
        m_pFence->SetName(L"Fence");

    if (m_pRtvDescriptorHeap)
        m_pRtvDescriptorHeap->SetName(L"RTVDescriptorHeap");

    if (m_pDsvDescriptorHeap)
        m_pDsvDescriptorHeap->SetName(L"DSVDescriptorHeap");

    if (m_pCbvSrvUavDescriptorHeap)
        m_pCbvSrvUavDescriptorHeap->SetName(L"CBVSRVUAVDescriptorHeap");

    if (m_pDepthStencil)
        m_pDepthStencil->SetName(L"DepthStencil");

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

    m_pRtvDescriptorHeap.Reset();
    m_pDsvDescriptorHeap.Reset();
    m_pCbvSrvUavDescriptorHeap.Reset();

    for (UINT i = 0; i < FrameCount; ++i)
    {
        m_pRenderTargets[i].Reset();
    }

    m_pCommandList.Reset();
    m_pCommandAllocator.Reset();
    m_pCommandQueue.Reset();
    m_pFence.Reset();
    m_pDepthStencil.Reset();
    m_pSwapChain.Reset();
    m_pDevice.Reset();
    m_pAdapter.Reset();
    m_pFactory.Reset();
}

void DeviceResources::CreateDevice()
{
#ifdef _DEBUG
    Microsoft::WRL::ComPtr<ID3D12Debug> pDebugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController))))
    {
        pDebugController->EnableDebugLayer();
    }
    else
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating debug interface, At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
    }

    if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_pFactory))))
#else
    if (FAILED(CreateDXGIFactory2(0, IID_PPV_ARGS(&m_pFactory))))
#endif
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating DXGIFactory, At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
        return;
    }

    Microsoft::WRL::ComPtr<IDXGIFactory6> pFactory6;
    if (SUCCEEDED(m_pFactory->QueryInterface(IID_PPV_ARGS(&pFactory6))))
    {
        if (FAILED(pFactory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_pAdapter))))
        {
            PRINT_CONSOLE_OUTPUT("[RENDER]: Error finding the adapter,  At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
        }
    }
    else
    {
        _ASSERT_EXPR(false, L"Not compatible with factory 6");
    }

    if (FAILED(D3D12CreateDevice(m_pAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_pDevice))))
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating the device,  At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
    }
}

void DeviceResources::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    HRESULT hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pCommandQueue));
    if (FAILED(hr))
    {
        PRINT_CONSOLE_OUTPUT(" [RENDER]: Error creating the command queue,  At file: " << __FILE__ << ", At line : " << __LINE__ << "\n");
    }
}


void DeviceResources::CreateCommandList()
{
    HRESULT hr = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator));
    if (FAILED(hr))
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating command allocator, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
        return;
    }

    hr = m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_pCommandList));
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
    heapDesc.NumDescriptors = FrameCount;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    heapDesc.NodeMask = 0;

    HRESULT hr = m_pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pRtvDescriptorHeap));
    if (FAILED(hr))
    {
        PRINT_CONSOLE_OUTPUT(" [RENDER]: Error creating the descriptor heap, At file: " << __FILE__ << ", At line: " << __LINE__ << "\n");
        return;
    }

    m_rtvHeapIncrement = m_pDevice->GetDescriptorHandleIncrementSize(heapDesc.Type);

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.Stereo = FALSE;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = FrameCount;
    desc.Scaling = DXGI_SCALING_NONE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> pSwapChain1 = nullptr;
    hr = m_pFactory->CreateSwapChainForHwnd(m_pCommandQueue.Get(), hwnd, &desc, nullptr, nullptr, &pSwapChain1);
    if (FAILED(hr))
    {
        PRINT_CONSOLE_OUTPUT(" [RENDER]: Error creating the swap chain, At file: " << __FILE__ << ", At line: " << __LINE__ << "\n");
        return;
    }

    hr = pSwapChain1->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_pSwapChain);
    if (FAILED(hr))
    {
        PRINT_CONSOLE_OUTPUT(" [RENDER]: Error casting to IDXGISwapChain3, At file: " << __FILE__ << ", At line: " << __LINE__ << "\n");
        return;
    }

    m_bufferCount = FrameCount;

    CreateRenderTargets();
    CreateDepthStencilResources(width, height);
}


void DeviceResources::CreateRenderTargets()
{
    if (!m_pSwapChain)
        return;

    for (UINT i = 0; i < FrameCount; ++i)
    {
        if (m_pRenderTargets[i])
        {
            m_pRenderTargets[i]->Release();
            m_pRenderTargets[i] = nullptr;
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < FrameCount; ++i)
    {
        if (FAILED(m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i]))))
        {
            PRINT_CONSOLE_OUTPUT(" [RENDER]: Failed to get swap chain buffer " << i << ", At file: " << __FILE__ << ", At line: " << __LINE__ << "\n");
            continue;
        }

        m_pDevice->CreateRenderTargetView(m_pRenderTargets[i].Get(), nullptr, rtvHandle);

        rtvHandle.ptr += m_rtvHeapIncrement;
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

    m_pDevice->CreateDepthStencilView(m_pDepthStencil.Get(), nullptr, m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
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

    if (FAILED(m_pCommandQueue->Signal(m_pFence.Get(), fenceToWait)))
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

Microsoft::WRL::ComPtr<ID3D12Resource> DeviceResources::CreateDefaultBuffer(
    ID3D12Device* device,
    ID3D12GraphicsCommandList* cmdList,
    const void* initData,
    UINT64 byteSize,
    Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer,
    D3D12_RESOURCE_STATES finalState)
{
    using Microsoft::WRL::ComPtr;

    ComPtr<ID3D12Resource> defaultBuffer;

    D3D12_HEAP_PROPERTIES defaultHeapProps = {};
    defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    defaultHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    defaultHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    defaultHeapProps.CreationNodeMask = 1;
    defaultHeapProps.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = byteSize;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(defaultBuffer.ReleaseAndGetAddressOf())
    );
    if (FAILED(hr)) return nullptr;

    D3D12_HEAP_PROPERTIES uploadHeapProps = {};
    uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    uploadHeapProps.CreationNodeMask = 1;
    uploadHeapProps.VisibleNodeMask = 1;

    hr = device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadBuffer.ReleaseAndGetAddressOf())
    );
    if (FAILED(hr)) return nullptr;

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = defaultBuffer.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    cmdList->ResourceBarrier(1, &barrier);

    void* mappedData = nullptr;
    D3D12_RANGE readRange = {};
    hr = uploadBuffer->Map(0, &readRange, &mappedData);
    if (FAILED(hr)) return nullptr;

    memcpy(mappedData, initData, byteSize);
    uploadBuffer->Unmap(0, nullptr);

    cmdList->CopyBufferRegion(defaultBuffer.Get(), 0, uploadBuffer.Get(), 0, byteSize);

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = finalState;
    cmdList->ResourceBarrier(1, &barrier);

    return defaultBuffer;
}

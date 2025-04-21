#include "pch.h"
#include "DeviceResources.h"

#include "FramesResources.h"

Render::DeviceResources::DeviceResources() : 
    m_pDevice(nullptr), m_pSwapChain(nullptr), m_pCommandQueue(nullptr), m_pRenderTargetViewHeap(nullptr),
    m_frameIndex(0), m_frameResources(), m_renderTargets(), m_renderTargetViewDescriptorSize(0), m_fence(nullptr),
    m_fenceValue(0), m_fenceEvent(nullptr), m_pFactory(nullptr)
{
}

Render::DeviceResources::~DeviceResources()
{
}

void Render::DeviceResources::Initialize(HWND hwnd, UINT width, UINT height)
{
    CreateDevice();
    CreateCommandQueue();
    CreateSwapChain(hwnd, width, height);
    CreateRenderTargets();
    CreateFrameResources();

    m_fenceValue = 1;
    if (m_pDevice->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating the Device Resources fence, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
    }
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_uploadCommandAllocator));
    m_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_uploadCommandAllocator, nullptr, IID_PPV_ARGS(&m_uploadCommandList));
    m_uploadCommandList->Close();

}

void Render::DeviceResources::Resize(UINT width, UINT height)
{
    WaitForGPU();

    for (UINT i = 0; i < FrameCount; i++)
    {
        m_renderTargets[i]->Release();
        m_renderTargets[i] = nullptr;
    }

    DXGI_SWAP_CHAIN_DESC desc = {};
    m_pSwapChain->GetDesc(&desc);
    m_pSwapChain->ResizeBuffers(FrameCount, width, height, desc.BufferDesc.Format, desc.Flags);

    m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
    CreateRenderTargets();

    for (UINT i = 0; i < FrameCount; ++i)
    {
        delete m_frameResources[i];
        m_frameResources[i] = new FramesResources();
        m_frameResources[i]->Initialize(m_pDevice);
    }

}

void Render::DeviceResources::WaitForGPU()
{
    m_pCommandQueue->Signal(m_fence, m_fenceValue);
    m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
    WaitForSingleObject(m_fenceEvent, INFINITE);
    m_fenceValue++;
}

void Render::DeviceResources::Present(bool vsync)
{
    m_pSwapChain->Present(vsync, 0);
    MoveToNextFrame();
}

void Render::DeviceResources::Release()
{
    WaitForGPU();

    if (m_fenceEvent) CloseHandle(m_fenceEvent);
    if (m_fence) m_fence->Release();

    for (UINT i = 0; i < FrameCount; ++i) {
        if (m_renderTargets[i]) 
        {
            m_renderTargets[i]->Release();
        }

        if (m_frameResources[i]) 
        {
            delete m_frameResources[i];
        }
    }

    if (m_uploadCommandList) m_uploadCommandList->Release();
    if (m_uploadCommandAllocator) m_uploadCommandAllocator->Release();
    if (m_pRenderTargetViewHeap)    m_pRenderTargetViewHeap->Release();
    if (m_pCommandQueue)            m_pCommandQueue->Release();
    if (m_pSwapChain)               m_pSwapChain->Release();
    if (m_pDevice)                  m_pDevice->Release();
}


void Render::DeviceResources::CreateDevice()
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
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating debug interface, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
    }

#endif // _DEBUG

    if (CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_pFactory)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating DXGI Factory, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
    }

    IDXGIAdapter1* pAdapter = nullptr;
    if (m_pFactory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&pAdapter)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating DXGI Adapter, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
    }

    if (D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating DXGI Device, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
    }

    pAdapter->Release();

}

void Render::DeviceResources::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    if (m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pCommandQueue)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating Command Queue, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
    }
}

void Render::DeviceResources::CreateSwapChain(HWND hwnd, UINT width, UINT height)
{
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.BufferCount = FrameCount;
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count = 1;

    IDXGISwapChain1* swapChain1 = nullptr;
    m_pFactory->CreateSwapChainForHwnd(m_pCommandQueue, hwnd, &desc, nullptr, nullptr, &swapChain1);
    swapChain1->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
    swapChain1->Release();

    m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void Render::DeviceResources::CreateRenderTargets()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (m_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_pRenderTargetViewHeap)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating descriptor heap, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
    }

    m_renderTargetViewDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();

    for (UINT i = 0; i < FrameCount; i++)
    {
        m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
        m_pDevice->CreateRenderTargetView(m_renderTargets[i], nullptr, rtvHandle);
        rtvHandle.ptr += m_renderTargetViewDescriptorSize;
    }

}

void Render::DeviceResources::MoveToNextFrame()
{
    const UINT currentFenceValue = m_fenceValue;
    m_pCommandQueue->Signal(m_fence, currentFenceValue);
    m_frameResources[m_frameIndex]->fenceValue = currentFenceValue;
    m_fenceValue++;

    m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

    if (m_fence->GetCompletedValue() < m_frameResources[m_frameIndex]->fenceValue)
    {
        m_fence->SetEventOnCompletion(m_frameResources[m_frameIndex]->fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void Render::DeviceResources::CreateFrameResources()
{
    for (UINT i = 0; i < FrameCount; i++)
    {
        m_frameResources[i] = new FramesResources();
        m_frameResources[i]->Initialize(m_pDevice);
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE Render::DeviceResources::GetCurrentRTV() const
{
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += m_frameIndex * m_renderTargetViewDescriptorSize;
    return handle;
}

void Render::DeviceResources::Upload(ID3D12Resource* pDest, void* pData, const UINT size, ID3D12CommandList* pCommandList, ID3D12CommandQueue* pCommandQueue, D3D12_RESOURCE_STATES state)
{
    if (!pDest || !pData || !pCommandList)
    {
        PRINT_CONSOLE_OUTPUT("Error: Invalid Upload parameters!" << std::endl);
        return;
    }

    ID3D12Resource* uploadBuffer = nullptr;

    D3D12_HEAP_PROPERTIES heapProp = {};
    heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProp.CreationNodeMask = 0;
    heapProp.VisibleNodeMask = 0;

    D3D12_RESOURCE_DESC resDesc = {};
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Alignment = 0;
    resDesc.Width = size;
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.MipLevels = 1;
    resDesc.Format = DXGI_FORMAT_UNKNOWN;
    resDesc.SampleDesc = { 1, 0 };
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    if ((m_pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer))) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating the buffer, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
    }

    void* memory = nullptr;
    HRESULT hr = uploadBuffer->Map(0, nullptr, &memory);
    if (hr != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error Failed to map Upload Buffer!" << std::endl);
        uploadBuffer->Release();
        return;
    }

    memcpy(memory, pData, size);
    uploadBuffer->Unmap(0, nullptr);

    if (!pDest)
        return;

    D3D12_RESOURCE_BARRIER barrier = {};
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = pDest;
    barrier.Transition.Subresource = 0;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

    m_uploadCommandList->ResourceBarrier(1, &barrier);
    m_uploadCommandList->CopyBufferRegion(pDest, 0, uploadBuffer, 0, size);
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = state;
    m_uploadCommandList->ResourceBarrier(1, &barrier);

    m_uploadCommandList->Close();
    ExecuteUploadCommandList();

    uploadBuffer->Release();

    m_frameResources[m_frameIndex]->GetCommandList()->ResourceBarrier(1, &barrier);
    m_frameResources[m_frameIndex]->GetCommandList()->Close();

    ExecuteCommandList();

    ID3D12Fence* fence = nullptr;
    UINT64 fenceValue = 1;
    hr = m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    if (FAILED(hr))
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error Failed to create Fence!" << std::endl);
        uploadBuffer->Release();
        return;
    }

    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error Failed to create Fence Event!" << std::endl);
        fence->Release();
        uploadBuffer->Release();
        return;
    }

    pCommandQueue->Signal(fence, fenceValue);
    if (fence->GetCompletedValue() < fenceValue)
    {
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    CloseHandle(fenceEvent);
    fence->Release();
    uploadBuffer->Release();

    m_frameResources[m_frameIndex]->Reset();
}

void Render::DeviceResources::ExecuteCommandList()
{
    m_pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)m_frameResources[m_frameIndex]->GetCommandList());
    m_fenceValue++;
    m_pCommandQueue->Signal(m_fence, m_fenceValue);
}

void Render::DeviceResources::WaitForTemporaryGPUExecution(ID3D12CommandQueue* commandQueue)
{
    ID3D12Fence* tempFence = nullptr;
    UINT64 fenceValue = 1;
    if (FAILED(m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&tempFence))))
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to create temporary fence." << std::endl);
        return;
    }

    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!fenceEvent)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to create temporary fence event." << std::endl);
        tempFence->Release();
        return;
    }

    commandQueue->Signal(tempFence, fenceValue);
    if (tempFence->GetCompletedValue() < fenceValue)
    {
        tempFence->SetEventOnCompletion(fenceValue, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    CloseHandle(fenceEvent);
    tempFence->Release();
}

void Render::DeviceResources::ExecuteUploadCommandList()
{
    ID3D12CommandList* lists[] = { m_uploadCommandList };
    m_pCommandQueue->ExecuteCommandLists(1, lists);
    WaitForTemporaryGPUExecution(m_pCommandQueue);
    m_uploadCommandAllocator->Reset();
    m_uploadCommandList->Reset(m_uploadCommandAllocator, nullptr);
}

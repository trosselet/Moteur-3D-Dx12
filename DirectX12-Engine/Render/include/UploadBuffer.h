#ifndef UPLOAD_BUFFER__H
#define UPLOAD_BUFFER__H

//D12 header files
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

//PreCompiled lib files for d12:
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")

#include "DebugMacro.h"

#include "Define.h"

struct ConstantBuffer
{
};

struct CameraCB : ConstantBuffer
{
    DirectX::XMFLOAT4X4 viewMatrix;
    DirectX::XMFLOAT4X4 projectionMatrix;
};

struct ObjectData : ConstantBuffer
{
    DirectX::XMFLOAT4X4 world;
};

struct MaterialProperties : ConstantBuffer
{
    DirectX::XMFLOAT4 ambientLightColor;
    DirectX::XMFLOAT4 ambientLight;
    DirectX::XMFLOAT4 diffuseLight;
    DirectX::XMFLOAT4 specularLight;

    float32 shininess;
    float32 padding[3];

};

struct LightCB : ConstantBuffer
{
    DirectX::XMFLOAT3 color;
    int32 type;

    DirectX::XMFLOAT3 Direction;
    float32 intensity;

    DirectX::XMFLOAT3 Position;
    float32 spotLightAngle;

    float32 padding[4];

};

template<typename T>
class UploadBuffer
{
public:

    inline UploadBuffer(ID3D12Device* pDevice, UINT elementCount, bool isConstantBuffer)
        : m_pUploadBuffer(nullptr), m_pMappedData(nullptr), m_elementCount(elementCount)
    {
        m_elementByteSize = sizeof(T);
        if (isConstantBuffer)
        {
            m_elementByteSize = (m_elementByteSize + 255) & ~255;
        }

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = m_elementByteSize * elementCount;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        if (pDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_pUploadBuffer)) != S_OK)
        {
            PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to create upload buffer.");
        }

        if (m_pUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pMappedData)) != S_OK)
        {
            PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to map upload buffer.");
        }

    }

    inline ~UploadBuffer()
    {
        if (m_pUploadBuffer)
        {
            m_pUploadBuffer->Unmap(0, nullptr);
            m_pUploadBuffer->Release();
        }
        m_pMappedData = nullptr;
    }

    inline void CopyData(UINT elementIndex, const T& data)
    {
        memcpy(m_pMappedData + elementIndex * m_elementByteSize, &data, sizeof(T));
    }

    inline ID3D12Resource* GetResource() const { return m_pUploadBuffer; }
    inline UINT GetElementByteSize() const { return m_elementByteSize; }


private:
    ID3D12Resource* m_pUploadBuffer;
    BYTE* m_pMappedData;
    UINT m_elementByteSize;
    UINT m_elementCount;
};


#endif // !UPLOAD_BUFFER__H

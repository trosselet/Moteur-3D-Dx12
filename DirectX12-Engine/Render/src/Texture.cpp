#include "pch.h"
#include "Texture.h"
#include "DDSTextureLoader.h"
#include "DeviceResources.h"
#include "GraphicEngine.h"

Texture::Texture(int8 const* path, GraphicEngine* pGraphic)
{
    std::string filePath = "../Gameplay/texture/" + std::string(reinterpret_cast<const char*>(path));
    std::wstring wFilePath(filePath.begin(), filePath.end());

    HRESULT hr = DirectX::CreateDDSTextureFromFile12(
        pGraphic->GetRender()->GetDeviceResources()->GetDevice(),
        pGraphic->GetRender()->GetDeviceResources()->GetCommandList(),
        wFilePath.c_str(),
        &m_pTexture,
        &m_pTextureUploadHeap,
        m_width,
        m_height
    );

    if (FAILED(hr))
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to load texture. HRESULT: " << hr << "\n");
        return;
    }

    UINT descriptorIndex = pGraphic->GetRender()->GetDeviceResources()->AllocateSRVHeapIndex();

    UINT size = pGraphic->GetRender()->GetDeviceResources()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = pGraphic->GetRender()->GetDeviceResources()->GetCbvSrvUavDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
    descriptorHandle.ptr += static_cast<SIZE_T>(descriptorIndex) * size;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = m_pTexture->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = m_pTexture->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = pGraphic->GetRender()->GetDeviceResources()->GetCbvSrvUavDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
    gpuHandle.ptr += static_cast<SIZE_T>(descriptorIndex) * size;

    pGraphic->GetRender()->GetDeviceResources()->GetDevice()->CreateShaderResourceView(
        m_pTexture.Get(), &srvDesc, descriptorHandle);

    m_textureAddress = gpuHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture::GetTextureAddress()
{
    return m_textureAddress;
}

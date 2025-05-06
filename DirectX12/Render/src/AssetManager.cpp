#include "pch.h"
#include "AssetManager.h"

#include "DDSTextureLoader.h"

Render::AssetManager::AssetManager() : m_pDeviceResources(nullptr), m_pTextureUploadHeap(nullptr)
{
}

Render::AssetManager::~AssetManager()
{
	for (auto& tex : m_textures)
	{
		if (tex.second)
		{
			tex.second->Release();
			tex.second = nullptr;
		}
	}

	for (auto& tex : m_loadedTextures)
	{
		if (tex.second)
		{
			tex.second->Release();
			tex.second = nullptr;
		}
	}

	m_textures.clear();
	m_loadedTextures.clear();

    if (m_pTextureUploadHeap)
    {
        m_pTextureUploadHeap->Release();
        m_pTextureUploadHeap = nullptr;
    }
}

void Render::AssetManager::Initialize(DeviceResources* pDeviceResources)
{
	m_pDeviceResources = pDeviceResources;
}

Render::AssetManager* Render::AssetManager::Get()
{
    static AssetManager instance;
    return &instance;
}

ID3D12DescriptorHeap* Render::AssetManager::GetTexture(LPCWSTR filepath)
{
	auto it = m_textures.find(filepath);
	if (it == m_textures.end())
	{
		return LoadTexture(filepath);
	}
	return it->second;
}

ID3D12DescriptorHeap* Render::AssetManager::LoadTexture(LPCWSTR filepath)
{
    ID3D12Resource* texture = nullptr;

    if (m_pTextureUploadHeap)
    {
        m_pTextureUploadHeap->Release();
        m_pTextureUploadHeap = nullptr;
    }

    ID3D12DescriptorHeap* srvHeap = nullptr;

    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = 1;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    if (m_pDeviceResources->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error Failed to create SRV Descriptor Heap. \n");
        return nullptr;
    }

    std::wstring texturePath = filepath;

    HRESULT hr = DirectX::CreateDDSTextureFromFile12(m_pDeviceResources->GetDevice(),
        m_pDeviceResources->GetCommandList(),
        m_pDeviceResources->GetCommandQueue(),
        m_pDeviceResources->GetAllocator(),
        texturePath.c_str(),
        &texture,
        &m_pTextureUploadHeap,
        0,
        nullptr);

    if (hr != S_OK || !texture)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Failed to load texture. HRESULT: " << hr << ", mTexture : " << texture << "\n");
        return nullptr;
    }

    m_loadedTextures[filepath] = texture;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = (texture)->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = (texture)->GetDesc().MipLevels;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
    m_pDeviceResources->GetDevice()->CreateShaderResourceView(texture, &srvDesc, srvHandle);

    m_textures[filepath] = srvHeap;

    return srvHeap;
}

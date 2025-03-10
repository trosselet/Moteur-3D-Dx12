#include "pch.h"
#include "AssetManager.h"


AssetManager::~AssetManager()
{
	for (auto& tex : mTextures)
	{
		if (tex.second)
		{
			tex.second->Release();
		}
	}

	mTextures.clear();
}


void AssetManager::Initialize(D3D12Manager* pManager)
{
    mpManager = pManager;
}

ID3D12DescriptorHeap* AssetManager::GetTexture(LPCWSTR filepath)
{
    auto it = mTextures.find(filepath);
    if (it == mTextures.end())
    {
        return LoadTexture(filepath);
    }
    return it->second;
}

ID3D12DescriptorHeap* AssetManager::LoadTexture(LPCWSTR filepath)
{

	ID3D12Resource* texture = nullptr;
	ID3D12Resource* textureUploadHeap = nullptr;
	ID3D12DescriptorHeap* srvHeap = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	if (FAILED(mpManager->GetD3D12Device()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap))))
	{
		PRINT_CONSOLE_OUTPUT("Error: Failed to create SRV Descriptor Heap." << std::endl);
	}

	std::wstring texturePath = filepath;
	HRESULT hr = DirectX::CreateDDSTextureFromFile12(mpManager->GetD3D12Device(), mpManager->GetGraphicsCommandList(), mpManager->GetD3D12CommandQueue(), mpManager->GetD3D12CommandAllocator(), texturePath.c_str(), &texture, &textureUploadHeap, 0, nullptr);

	if (FAILED(hr) || !texture)
	{
		PRINT_CONSOLE_OUTPUT("Error: Failed to load texture. HRESULT: " << hr << ", mTexture : " << texture << std::endl);
		return nullptr;
	}


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = (texture)->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = (texture)->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	mpManager->GetD3D12Device()->CreateShaderResourceView(texture, &srvDesc, srvHandle);

	mTextures[filepath] = srvHeap;

	return srvHeap;
}
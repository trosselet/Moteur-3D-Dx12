#pragma once

#include <unordered_map>
#include <d3d12.h>
#include "d3dx12.h"
#include "DDSTextureLoader.h"
#include "D3D12Manager.h"

class AssetManager
{

public:
	AssetManager() = default;
	~AssetManager();
	void Initialize(D3D12Manager* pManager);

	ID3D12DescriptorHeap* GetTexture(LPCWSTR filepath);

private:
	ID3D12DescriptorHeap* LoadTexture(LPCWSTR filepath);


private:
	D3D12Manager* mpManager;
	std::unordered_map<std::wstring, ID3D12DescriptorHeap*> mTextures;
};


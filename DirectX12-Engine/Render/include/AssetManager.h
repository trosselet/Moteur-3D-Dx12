#pragma once

#include "DeviceResources.h"
#include <d3d12.h>
#include <map>
#include <string>

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();

    void Initialize(DeviceResources* pDeviceResources);

    static AssetManager* Get();

    ID3D12DescriptorHeap* GetTexture(LPCWSTR filepath);

private:
    ID3D12DescriptorHeap* LoadTexture(LPCWSTR filepath);

    DeviceResources* m_pDeviceResources;

    std::map<std::wstring, ID3D12DescriptorHeap*> m_textures;

    std::map<std::wstring, ID3D12Resource*> m_loadedTextures;

    ID3D12Resource* m_pTextureUploadHeap;
};

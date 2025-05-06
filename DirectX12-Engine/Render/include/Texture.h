#ifndef TEXTURE__H
#define TEXTURE__H

#include <d3d12.h>
#include <wrl/client.h>
#include "Define.h"

class GraphicEngine;

class Texture
{
public:
    Texture(int8 const* path, GraphicEngine* pGraphic);
    ~Texture() = default;

    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureAddress();

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_pTexture;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_pTextureUploadHeap;

    D3D12_DESCRIPTOR_HEAP_DESC m_srvHeapDesc{};
    D3D12_GPU_DESCRIPTOR_HANDLE m_textureAddress{};

    int m_width = 0;
    int m_height = 0;
};

#endif // !TEXTURE__H

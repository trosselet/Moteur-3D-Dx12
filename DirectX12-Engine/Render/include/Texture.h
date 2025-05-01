#ifndef TEXTURE__H
#define TEXTURE__H

#include <d3d12.h>
#include "Define.h"

class GraphicEngine;

class Texture
{
public:
	Texture(int8 const* path, GraphicEngine* pGraphic);
	~Texture() = default;

	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureAddress();

private:

	ID3D12Resource* m_pTexture = nullptr;
	ID3D12Resource* m_pTextureUploadHeap = nullptr;

	D3D12_DESCRIPTOR_HEAP_DESC m_srvHeapDesc;
	D3D12_GPU_DESCRIPTOR_HANDLE m_textureAddress;

	int m_width;
	int m_height;
};


#endif // !TEXTURE__H

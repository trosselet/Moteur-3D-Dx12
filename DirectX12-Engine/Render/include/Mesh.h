#ifndef MESH__H
#define MESH__H

#include "UploadBuffer.h"

#include "Define.h"

#include <wrl.h>

struct Geometry;

class Render;

class Mesh
{
public:
    Mesh(Geometry* pGeometry, Render* pRender);
    ~Mesh();

    void UpdateGeometry(); //Updates geometry in upload buffers
    D3D12_INDEX_BUFFER_VIEW GetIndexBuffer();
    D3D12_VERTEX_BUFFER_VIEW GetVertexBuffer();
    UINT GetIndexCount();
    void ReleaseUploadBuffers();

private:
    void UploadGeometry();
    void UploadBuffers(float32* vertices, UINT vertexCount, uint32* indices, UINT indexCount, UINT floatStride);

private:
    Render* m_pRender = nullptr;
    Geometry* m_pGeometry = nullptr;

    UINT m_vertexCount;
    UINT m_indexCount;

    Microsoft::WRL::ComPtr<ID3D12Resource> m_pVertexBufferUploader;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_pIndexBufferUploader;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_pVertexBufferGPU;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_pIndexBufferGPU;

    D3D12_VERTEX_BUFFER_VIEW m_vertexBuffer = {};
    D3D12_INDEX_BUFFER_VIEW m_indexBuffer = {};
};


#endif // !MESH__H
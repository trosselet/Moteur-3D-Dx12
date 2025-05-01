#ifndef MESH__H
#define MESH__H

#include "UploadBuffer.h"

#include "Define.h"

struct Geometry;

class Render;

class Mesh
{
public:
    Mesh(Geometry* pGeometry, Render* pRender);
    ~Mesh() = default;

    void UpdateGeometry(); //Updates geometry in upload buffers
    D3D12_INDEX_BUFFER_VIEW GetIndexBuffer();
    D3D12_VERTEX_BUFFER_VIEW GetVertexBuffer();
    UINT GetIndexCount();
    void ReleaseUploadBuffers();

private:
    void UploadGeometry();
    void UploadBuffers(float32* vertices, UINT vertexCount, uint32* indices, UINT indexCount);

private:
    Render* m_pRender = nullptr;
    Geometry* m_pGeometry = nullptr;

    UINT m_vertexCount;
    UINT m_indexCount;

    UploadBuffer<float32>* m_pVertexBufferUploader;
    UploadBuffer<uint32>* m_pIndexBufferUploader;

    D3D12_VERTEX_BUFFER_VIEW* m_pVertexBuffer;
    D3D12_INDEX_BUFFER_VIEW* m_pIndexBuffer;
};


#endif // !MESH__H
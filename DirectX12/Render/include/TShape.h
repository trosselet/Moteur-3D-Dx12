#ifndef SHAPE__H
#define SHAPE__H

#include <DirectXMath.h>
#include "UploadBuffer.h"

#include "../../Core/include/TDrawable.h"
#include "../../Core/include/TTransform.h"

namespace Render
{
	class Shape : public Core::IDrawable, public Core::Transform
	{
	public:

		Shape();
		virtual ~Shape();

		
		void Draw(ID3D12Device* pDevice, ID3D12GraphicsCommandList* commandList) override;

		virtual void BuildBuffers() = 0;
		

	protected:
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT4 color;
		};

		void UploadBuffers(Vertex* vertices, UINT vertexCount, UINT* indices, UINT indexCount);


	protected:

		UploadBuffer<Vertex>* m_pVertexBuffer;
		UploadBuffer<UINT>* m_pIndexBuffer;

		D3D12_VERTEX_BUFFER_VIEW m_vbv;
		D3D12_INDEX_BUFFER_VIEW m_ibv;

		ID3D12Device* m_pDevice;

		UINT m_vertexCount;
		UINT m_indexCount;

		bool m_isInitialize;

	};
}


#endif // !SHAPE__H
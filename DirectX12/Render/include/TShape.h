#ifndef SHAPE__H
#define SHAPE__H

#include <DirectXMath.h>
#include "TDrawable.h"
#include "TTransform.h"
#include "UploadBuffer.h"

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
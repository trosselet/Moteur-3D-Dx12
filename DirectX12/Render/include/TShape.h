#ifndef SHAPE__H
#define SHAPE__H

#include <DirectXMath.h>
#include "UploadBuffer.h"
#include "AssetManager.h"

#include "../../Core/include/TDrawable.h"
#include "../../Core/include/TTransformable.h"
#include "../../Core/include/TColor.h"

struct Geometry;

namespace Render
{
	class GraphicEngine;

	class Shape : public Core::IDrawable, public Core::Transformable
	{
		struct ObjectData 
		{
			DirectX::XMFLOAT4X4 world;
		};

	public:

		Shape();
		virtual ~Shape();


	private:
		void UploadGeometry();

		void Draw(ID3D12Device* pDevice, ID3D12GraphicsCommandList* commandList) override;

		

	protected:
		virtual void BuildBuffers() = 0;

		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 texCorrd;
		};

		void UploadBuffers(Vertex* vertices, UINT vertexCount, UINT* indices, UINT indexCount);


	protected:

		UploadBuffer<Vertex>* m_pVertexBuffer;
		UploadBuffer<UINT>* m_pIndexBuffer;

		D3D12_VERTEX_BUFFER_VIEW m_vbv;
		D3D12_INDEX_BUFFER_VIEW m_ibv;

		UploadBuffer<ObjectData>* m_pConstantBuffer;

		ID3D12Device* m_pDevice;

		UINT m_vertexCount;
		UINT m_indexCount;

		Color m_color;

		bool m_isInitialize;

		ID3D12DescriptorHeap* m_pTextureHeap;
		AssetManager* m_pAssetManager;


	private:
		friend class GraphicEngine;
	};
}


#endif // !SHAPE__H
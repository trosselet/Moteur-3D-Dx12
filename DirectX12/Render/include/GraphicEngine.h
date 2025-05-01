#ifndef GRAPHIC_ENIGNE__H
#define GRAPHIC_ENIGNE__H

#include <d3d12.h>
#include <DirectXMath.h>

#include "PrimitiveFactory.h"

#include "../../Core/include/TTransform.h"
#include "../../Core/include/TColor.h"

namespace Core
{
	class IDrawable;
}

struct GlobalInformation
{
	DirectX::XMFLOAT4X4 ViewProj;
};

struct ObjectData
{
	DirectX::XMFLOAT4X4 world;
};

struct Geometry;
class Mesh;


namespace Render
{
	enum PrimitiveType : uint8;
	class DeviceResources;
	class PipelineStateObjectManager;
	class Shape;

	template<typename T>
	class UploadBuffer;

	class GraphicEngine
	{
	public:
		GraphicEngine();
		~GraphicEngine();

		

		void Initialize(HWND hwnd, UINT width, UINT height, TRANSFORM* camera);
		void Update();
		void BeginFrame(Color clearColor = Color{0.0f, 0.0f, 0.0f, 1.0f});
		void RenderFrame(Shape& shape, const char* shaderPath = "../Game/shader/DefaultShader.hlsl");
		void RenderFrame(Mesh* pMesh, const char* shaderPath = "../Game/shader/DefaultShader.hlsl");
		void EndFrame();

		void ResizeWindow(UINT width, UINT height);
		bool IsInitialize() const { return m_isInitialize; };

		void SetViewport(UINT width, UINT height);

		Geometry* CreatePrimitiveGeometry(PrimitiveType primitiveType, Color color = Color::White);
		Mesh* CreateMesh(Geometry* pGeometry);


	private:
		DeviceResources* m_pDeviceResources;
		PipelineStateObjectManager* m_pPsoManager;
		bool m_isInitialize = false;

		D3D12_VIEWPORT m_viewport;
		D3D12_RECT m_rect;

		UploadBuffer<GlobalInformation>* mGlobalConstantBuffer;

		TRANSFORM* m_pCam;
		DirectX::XMFLOAT4X4 m_view;
		DirectX::XMFLOAT4X4 m_proj;


	};
}
#endif // !GRAPHIC_ENIGNE__H
#ifndef GRAPHIC_ENIGNE__H
#define GRAPHIC_ENIGNE__H

#include <d3d12.h>
#include <DirectXMath.h>

#include "../../Core/include/TTransform.h"

namespace Core
{
	class IDrawable;
}

struct GlobalInformation
{
	DirectX::XMFLOAT4X4 ViewProj;
};

namespace Render
{
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
		void BeginFrame(Color clearColor);
		void RenderFrame(Shape& shape, const char* shaderPath = "../Game/shader/DefaultShader.hlsl");
		void EndFrame();

		void ResizeWindow(UINT width, UINT height);
		bool IsInitialize() const { return m_isInitialize; };

		void SetViewport(UINT width, UINT height);

		void ReportLiveD3D12Objects();

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
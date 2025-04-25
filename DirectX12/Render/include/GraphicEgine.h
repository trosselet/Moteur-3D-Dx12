#ifndef GRAPHIC_ENIGNE__H
#define GRAPHIC_ENIGNE__H

#include <d3d12.h>

namespace Core
{
	class IDrawable;
}

namespace Render
{
	class DeviceResources;
	class PipelineStateObjectManager;

	class GraphicEgine
	{
	public:
		GraphicEgine();
		~GraphicEgine();

		void Initialize(HWND hwnd, UINT width, UINT height);
		void BeginFrame(Color clearColor);
		void RenderFrame(Core::IDrawable& drawable, const char* shaderPath = "../Game/shader/DefaultShader.hlsl");
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
	};
}
#endif // !GRAPHIC_ENIGNE__H
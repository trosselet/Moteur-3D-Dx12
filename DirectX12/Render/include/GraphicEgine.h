#ifndef GRAPHIC_ENIGNE__H
#define GRAPHIC_ENIGNE__H



namespace Render
{
	class DeviceResources;

	class GraphicEgine
	{
	public:
		GraphicEgine();
		~GraphicEgine();

		void Initialize(HWND hwnd, UINT width, UINT height);
		void BeginFrame(Color clearColor);
		void RenderFrame();
		void EndFrame();

		void ResizeWindow(UINT width, UINT height);
		bool IsInitialize() const { return m_isInitialize; };

	private:
		DeviceResources* m_pDeviceResources;
		bool m_isInitialize = false;
	};
}
#endif // !GRAPHIC_ENIGNE__H
#pragma once


namespace Render
{
	class GraphicEgine
	{
	public:
		GraphicEgine();
		~GraphicEgine();

		void Initialize();
		void BeginFrame();
		void RenderFrame();
		void EndFrame();

	private:

	};
}
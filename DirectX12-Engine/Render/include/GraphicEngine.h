#ifndef GRAPHIC_ENIGNE__H
#define GRAPHIC_ENIGNE__H

#include <d3d12.h>
#include <DirectXMath.h>
#include "Transform.h"
#include "Color.h"
#include "Render.h"
#include "PipelineStateObjectManager.h"


class Window;
struct Geometry;
class Mesh;
class Texture;
class Material;
struct LightCB;
enum PrimitiveType : uint8;

class GraphicEngine
{
public:
	GraphicEngine(Window* pWindow);
	~GraphicEngine();

	//void Update();
	void BeginFrame();
	void RenderFrame(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objectWorldMatrix);
	void EndFrame();

	bool IsInit() { return m_isInit; };

	void ResizeWindow(UINT width, UINT height);

	Geometry* CreatePrimitiveGeometry(PrimitiveType primitiveType, Color color = Color::White);
	Mesh* CreateMesh(Geometry* pGeometry);
	Texture* CreateTexture(char const* filePath);
	Material* CreateMaterial(PipelineStateObjectManager::PipelineStateConfig* pShader);

	void AddLight(LightCB* pLight);

	void UpdateCameraAt(Vector3f const& position, Vector3f const& target, Vector3f const& up, float32 viewWidth, float32 viewHeight, float32 fov, float32 cNear, float32 cFar, DirectX::XMMATRIX& projectionMatrix, DirectX::XMMATRIX& viewMatrix);
	void UpdateCameraTo(Vector3f const& position, Vector3f const& target, Vector3f const& up, float32 viewWidth, float32 viewHeight, float32 fov, float32 cNear, float32 cFar, DirectX::XMMATRIX& projectionMatrix, DirectX::XMMATRIX& viewMatrix);

	inline Render* GetRender() { return &m_render; };

private:
	Render m_render;
	bool m_isInit = false;
};


#endif // !GRAPHIC_ENIGNE__H
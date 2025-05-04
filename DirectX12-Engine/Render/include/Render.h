#ifndef RENDER__H
#define RENDER__H

#include <DirectXMath.h>
#include "Define.h"
#include "Vector.hpp"
#include "UploadBuffer.h"
#include "Color.h"

class Window;
class Mesh;
class Material;
class DeviceResources;
class PipelineStateObjectManager;
class GraphicEngine;

class Render
{
	friend class Window;
	friend class GraphicEngine;

	Render(Window* pWindow, uint32 renderWidth = 1920, uint32 renderHeight = 1080);
	~Render();


public:
	DeviceResources* GetDeviceResources();
	inline PipelineStateObjectManager* GetPSOManager() { return m_pPSOManager; };

	void CopyLightsData();

	bool Clear();
	bool DrawObject(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objworldMatrix);
	bool Display();

private:
	Window* m_pWindow = nullptr;
	uint16 m_renderWidth = 1920;
	uint16 m_renderHeight = 1080;

	DeviceResources* m_pDeviceResources;
	PipelineStateObjectManager* m_pPSOManager;

	UploadBuffer<CameraCB>* m_pCbCurrentViewProjInstance;
	UploadBuffer<LightCB>* m_pCbCurrentLightInstance;

	std::vector<LightCB*> m_lightSources;

};


#endif // !RENDER__H
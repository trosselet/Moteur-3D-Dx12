#ifndef MATERIAL__H
#define MATERIAL__H

#include "UploadBuffer.h"

#include "Define.h"
#include "Vector.hpp"

#include "PipelineStateObjectManager.h"

class Texture;
class Render;

class Material
{
public:
	Material(PipelineStateObjectManager::PipelineStateConfig* pShader, Render* pRender);
	~Material() = default;

	void SetTexture(Texture* pTexture);
	bool UpdateTexture(int16 position);
	
	void SetMaterialProperties(Vector<float32, 4> lightColor, Vector<float32, 4> ambient, Vector<float32, 4> diffuse, Vector<float32, 4> specular, float32 shininess, bool ingoreLighting);

	Texture* GetTexture();
	PipelineStateObjectManager::PipelineStateConfig* GetShader();

	UploadBuffer<ObjectData>* GetUploadBuffer();
	UploadBuffer<MaterialProperties>* GetLightUploadBuffer();

	void UpdateWorldConstantBuffer(DirectX::XMMATRIX const& matrix);

private:
	Texture* m_pTexture;
	PipelineStateObjectManager::PipelineStateConfig* m_pShader;

	Render* m_pRender = nullptr;

	UploadBuffer<ObjectData> m_uploadBuffer;
	UploadBuffer<MaterialProperties> m_lightUploadBuffer;

	MaterialProperties m_materialProperties;

};

#endif // !MATERIAL__H

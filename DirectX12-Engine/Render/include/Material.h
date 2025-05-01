#ifndef MATERIAL__H
#define MATERIAL__H

#include "UploadBuffer.h"

#include "Define.h"

class Texture;
class Render;
class PipelineStateObjectManager;

class Material
{
public:
	Material(PipelineStateObjectManager* pShader, Render* pRender);
	~Material() = default;

	void SetTexture(Texture* pTexture);
	bool UpdateTexture(int16 position);
	
	Texture* GetTexture();
	PipelineStateObjectManager* GetShader();

	UploadBuffer<ObjectData>* GetUploadBuffer();

	void UpdateWorldConstantBuffer(DirectX::XMMATRIX const& matrix);

private:
	Texture* m_pTexture;
	PipelineStateObjectManager* m_pShader;

	Render* m_pRender = nullptr;

	UploadBuffer<ObjectData> m_uploadBuffer;

};

#endif // !MATERIAL__H

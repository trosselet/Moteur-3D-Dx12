#include "pch.h"
#include "Material.h"
#include "DeviceResources.h"
#include "Texture.h"
#include "Render.h"
#include "PipelineStateObjectManager.h"

Material::Material(PipelineStateObjectManager::PipelineStateConfig* pShader, Render* pRender) :
    m_pShader(pShader),
    m_pRender(pRender),
    m_uploadBuffer(pRender->GetDeviceResources()->GetDevice(), 1, 1),
    m_pTexture(nullptr),
    m_lightUploadBuffer(pRender->GetDeviceResources()->GetDevice(), 1, 1)
{
    m_materialProperties.ambientLightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_materialProperties.ambientLight = DirectX::XMFLOAT4(0.55f, 0.55f, 0.55f, 1.0f);
    m_materialProperties.diffuseLight = DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
    m_materialProperties.specularLight = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_materialProperties.shininess = 5.0f;
    m_materialProperties.ignoreLighting = false;

    m_lightUploadBuffer.CopyData(0, m_materialProperties);
}

Material::~Material()
{
    if (m_pTexture)
    {
        delete m_pTexture;
        m_pTexture = nullptr;
    }
}

void Material::SetTexture(Texture* pTexture)
{
    m_pTexture = pTexture;
}

bool Material::UpdateTexture(int16 position)
{
    if (m_pTexture != nullptr)
    {
        m_pRender->GetDeviceResources()->GetCommandList()->SetGraphicsRootDescriptorTable(position, m_pTexture->GetTextureAddress());
    }
    return true;
}

void Material::SetMaterialProperties(Vector<float32, 4> lightColor, Vector<float32, 4> ambient, Vector<float32, 4> diffuse, Vector<float32, 4> specular, float32 shininess, bool ingoreLighting)
{
    m_materialProperties.ambientLightColor = DirectX::XMFLOAT4(lightColor.values[0], lightColor.values[1], lightColor.values[2], lightColor.values[3]);
    m_materialProperties.ambientLight = DirectX::XMFLOAT4(ambient.values[0], ambient.values[1], ambient.values[2], ambient.values[3]);
    m_materialProperties.diffuseLight = DirectX::XMFLOAT4(diffuse.values[0], diffuse.values[1], diffuse.values[2], diffuse.values[3]);
    m_materialProperties.specularLight = DirectX::XMFLOAT4(specular.values[0], specular.values[1], specular.values[2], specular.values[3]);
    m_materialProperties.shininess = shininess;
    m_materialProperties.ignoreLighting = ingoreLighting;

    m_lightUploadBuffer.CopyData(0, m_materialProperties);
}

Texture* Material::GetTexture()
{
    return m_pTexture;
}

PipelineStateObjectManager::PipelineStateConfig* Material::GetShader()
{
    return m_pShader;
}

UploadBuffer<ObjectData>* Material::GetUploadBuffer()
{
    return &m_uploadBuffer;
}

UploadBuffer<MaterialProperties>* Material::GetLightUploadBuffer()
{
    return &m_lightUploadBuffer;
}

void Material::UpdateWorldConstantBuffer(DirectX::XMMATRIX const& matrix)
{
    ObjectData dataCb = {};
    DirectX::XMStoreFloat4x4(&dataCb.world, DirectX::XMMatrixTranspose(matrix));
    m_uploadBuffer.CopyData(0, dataCb);
}

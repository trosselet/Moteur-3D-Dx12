#include "pch.h"
#include "Material.h"
#include "DeviceResources.h"
#include "Texture.h"
#include "Render.h"
#include "PipelineStateObjectManager.h"

Material::Material(PipelineStateObjectManager* pShader, Render* pRender) :
    m_pShader(pShader),
    m_pRender(pRender),
    m_uploadBuffer(pRender->GetDeviceResources()->GetDevice(), 1, 1),
    m_pTexture(nullptr)
{
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

Texture* Material::GetTexture()
{
    return m_pTexture;
}

PipelineStateObjectManager* Material::GetShader()
{
    return m_pShader;
}

UploadBuffer<ObjectData>* Material::GetUploadBuffer()
{
    return &m_uploadBuffer;
}

void Material::UpdateWorldConstantBuffer(DirectX::XMMATRIX const& matrix)
{
    ObjectData dataCb = {};
    DirectX::XMStoreFloat4x4(&dataCb.world, DirectX::XMMatrixTranspose(matrix));
    m_uploadBuffer.CopyData(0, dataCb);
}

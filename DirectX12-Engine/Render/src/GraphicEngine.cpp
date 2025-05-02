#include "pch.h"
#include "GraphicEngine.h"
#include "DeviceResources.h"
#include "Window.h"
#include "PrimitiveFactory.h"
#include "UploadBuffer.h"
#include "Geometry.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

GraphicEngine::GraphicEngine(Window* pWindow) :
	m_render(pWindow, pWindow->GetWidth(), pWindow->GetHeight())
{
	PrimitiveFactory::InitializePrimitiveGeometry();
	m_isInit = true;
}

GraphicEngine::~GraphicEngine()
{

}

void GraphicEngine::BeginFrame()
{
	m_render.Clear();
}

void GraphicEngine::RenderFrame(Mesh* pMesh, Material* pMaterial, DirectX::XMFLOAT4X4 const& objectWorldMatrix)
{
	m_render.DrawObject(pMesh, pMaterial, objectWorldMatrix);
}

void GraphicEngine::EndFrame()
{
	m_render.Display();
}

void GraphicEngine::ResizeWindow(UINT width, UINT height)
{
	m_render.GetDeviceResources()->Resize(width, height);
}

Geometry* GraphicEngine::CreatePrimitiveGeometry(PrimitiveType primitiveType, Color color)
{
	Geometry const* const pGeo = PrimitiveFactory::GetPrimitiveGeometry(primitiveType);
	Geometry* pResult = new Geometry();
	*pResult = *pGeo;

	for (int i = 0; i < pResult->indexes.size(); i++)
	{
		pResult->colors.push_back(DirectX::XMFLOAT4(color.r, color.g, color.b, color.a));
	}

	return pResult;
}

Mesh* GraphicEngine::CreateMesh(Geometry* pGeometry)
{
	m_render.GetDeviceResources()->ResetCommandList();
	Mesh* mesh = new Mesh(pGeometry, &m_render);
	m_render.GetDeviceResources()->GetCommandList()->Close();
	m_render.GetDeviceResources()->ExecuteTheCommandList();
	m_render.GetDeviceResources()->FlushQueue(2);
	mesh->ReleaseUploadBuffers();
	return mesh;
}

Texture* GraphicEngine::CreateTexture(char const* filePath)
{
	m_render.GetDeviceResources()->ResetCommandList();
	Texture* pTexture = new Texture(filePath, this);
	m_render.GetDeviceResources()->GetCommandList()->Close();
	m_render.GetDeviceResources()->ExecuteTheCommandList();
	m_render.GetDeviceResources()->FlushQueue(2);
	return pTexture;
}

Material* GraphicEngine::CreateMaterial(PipelineStateObjectManager::PipelineStateConfig* pShader)
{
	Material* pMat = new Material(pShader, &m_render);
	return pMat;
}

void GraphicEngine::UpdateCameraAt(Vector3f const& position, Vector3f const& target, Vector3f const& up, float32 viewWidth, float32 viewHeight, float32 fov, float32 cNear, float32 cFar, DirectX::XMMATRIX& projectionMatrix, DirectX::XMMATRIX& viewMatrix)
{
	DirectX::XMFLOAT3 d12Position = DirectX::XMFLOAT3(position.x, position.y, position.z);
	DirectX::XMFLOAT3 d12Target = DirectX::XMFLOAT3(target.x, target.y, target.z);
	DirectX::XMFLOAT3 d12Up = DirectX::XMFLOAT3(up.x, up.y, up.z);

	DirectX::XMVECTOR camPos = XMLoadFloat3(&d12Position);
	DirectX::XMVECTOR camTarget = XMLoadFloat3(&d12Target);
	DirectX::XMVECTOR camUp = XMLoadFloat3(&d12Up);

	DirectX::XMMATRIX tempProj = DirectX::XMMatrixPerspectiveFovLH(fov, viewWidth / viewHeight, cNear, cFar);
	DirectX::XMMATRIX tempView = DirectX::XMMatrixLookAtLH(camPos, camTarget, camUp);

	projectionMatrix = DirectX::XMMATRIX(DirectX::XMMatrixTranspose(tempProj));
	viewMatrix = DirectX::XMMATRIX(DirectX::XMMatrixTranspose(tempView));

	CameraCB camera = {};
	DirectX::XMStoreFloat4x4(&camera.projectionMatrix, projectionMatrix);
	DirectX::XMStoreFloat4x4(&camera.viewMatrix, projectionMatrix);

	m_render.m_pCbCurrentViewProjInstance->CopyData(0, camera);
}

void GraphicEngine::UpdateCameraTo(Vector3f const& position, Vector3f const& target, Vector3f const& up, float32 viewWidth, float32 viewHeight, float32 fov, float32 cNear, float32 cFar, DirectX::XMMATRIX& projectionMatrix, DirectX::XMMATRIX& viewMatrix)
{
	DirectX::XMFLOAT3 d12Position = DirectX::XMFLOAT3(position.x, position.y, position.z);
	DirectX::XMFLOAT3 d12Target = DirectX::XMFLOAT3(target.x, target.y, target.z);
	DirectX::XMFLOAT3 d12Up = DirectX::XMFLOAT3(up.x, up.y, up.z);

	DirectX::XMVECTOR camPos = XMLoadFloat3(&d12Position);
	DirectX::XMVECTOR camTarget = XMLoadFloat3(&d12Target);
	DirectX::XMVECTOR camUp = XMLoadFloat3(&d12Up);

	DirectX::XMMATRIX tempProj = DirectX::XMMatrixPerspectiveFovLH(fov, viewWidth / viewHeight, cNear, cFar);
	DirectX::XMMATRIX tempView = DirectX::XMMatrixLookAtLH(camPos, camTarget, camUp);

	projectionMatrix = DirectX::XMMATRIX(DirectX::XMMatrixTranspose(tempProj));
	viewMatrix = DirectX::XMMATRIX(DirectX::XMMatrixTranspose(tempView));

	CameraCB camera = {};
	DirectX::XMStoreFloat4x4(&camera.projectionMatrix, projectionMatrix);
	DirectX::XMStoreFloat4x4(&camera.viewMatrix, projectionMatrix);

	m_render.m_pCbCurrentViewProjInstance->CopyData(0, camera);
}

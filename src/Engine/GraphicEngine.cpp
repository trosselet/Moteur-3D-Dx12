#include "pch.h"
#include "GraphicEngine.h"

#include <map>
#include <vector>
#include <fstream>

#include "Component.h"

#include "D12Debug.h"

#include <d3dcompiler.h>
#pragma comment(lib, "D3DCompiler.lib")



std::wstring MatrixToString(const DirectX::XMMATRIX& matrix)
{
	std::wostringstream os;
	for (int i = 0; i < 4; ++i)
	{
		os << L"[ ";
		for (int j = 0; j < 4; ++j)
		{
			os << matrix.r[i].m128_f32[j] << L" ";
		}
		os << L"]\n";
	}
	return os.str();
}



GraphicEngine::~GraphicEngine()
{
	mpD3D12Manager->FlushQueue(mpDxgiManager->GetBufferCount());

	mpDxgiManager->Release();
	mpD3D12Manager->Release();

	delete mpDxgiManager;
	delete mpD3D12Manager;
	delete mpAssetManager;

	for (Mesh* mesh : mObjects)
	{
		if (mesh)
		{
			if (mesh->constantBuffer) mesh->constantBuffer->Release();
			if (mesh->vertexBuffer) mesh->vertexBuffer->Release();
			if (mesh->indexBuffer) mesh->indexBuffer->Release();
			if (mesh->srvHeap) mesh->srvHeap->Release();
			mesh->parent = nullptr;

			delete mesh;
		}
	}
	mObjects.clear();
}

void GraphicEngine::Initialize(HWND hwnd, int width, int height)
{
	mWidth = width;
	mHeight = height;

	//Initialize D3D12 Debug
	Engine::D12Debug::Get().Enable();


	mpDxgiManager = new DXGIManager();
	mpD3D12Manager = new D3D12Manager();
	mpAssetManager = new AssetManager();

	//Initialze DXGI Factory2
	mpDxgiManager->InitializeFactory();
	//Initialize DXGI Adapter
	mpDxgiManager->InitializeAdapter();

	//Get DXGI Adapter Description to print out the usage Graphic Card
	DXGI_ADAPTER_DESC desc;
	mpDxgiManager->GetDXGIAdapter()->GetDesc(&desc);
	PRINT_CONSOLE_OUTPUT("Selected adapter : " << desc.Description << std::endl);

	//Initialize D3D12 Device
	mpD3D12Manager->D3D12DeviceInitialize(mpDxgiManager->GetDXGIAdapter());

	//Initialize D3D12 Command Queue
	mpD3D12Manager->D3D12CommandQueueInitialize(mpD3D12Manager->GetD3D12Device());
	//Initialize D3D12 Command List
	mpD3D12Manager->D3D12CommandListInitialize(mpD3D12Manager->GetD3D12Device());

	//Initialze DXGI SwapChain + Create SwapChain Buffer
	mpDxgiManager->InitializeSwapChain(mpD3D12Manager->GetD3D12Device(), mpDxgiManager->GetDXGIFactory(), mpD3D12Manager->GetD3D12CommandQueue(), hwnd, mWidth, mHeight);

	//Initialize the PipelineState
	mpD3D12Manager->D3D12PipelineState3DObjectInitialize(mpD3D12Manager->GetD3D12Device());
	mpD3D12Manager->D3D12PipelineStateUiElementInitialize(mpD3D12Manager->GetD3D12Device());
	mpD3D12Manager->D3D12PipelineStatePostProcessInitialize(mpD3D12Manager->GetD3D12Device());

	//Initialize the Depth buffer for depth rendering
	mpD3D12Manager->DepthBufferInitialize(mpD3D12Manager->GetD3D12Device(), width, height, mpD3D12Manager->GetD3D12DepthBufferPointer());

	//Initialize the Depth buffer descriptor heap for depth rendering
	mpD3D12Manager->DepthDescriptorHeapInitialize(mpD3D12Manager->GetD3D12Device());

	//Create a Depth stencil view for depth rendering
	mpD3D12Manager->DepthStencilViewInitialize(mpD3D12Manager->GetD3D12Device());

	//Define viewport size
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mViewport.Width = mWidth;
	mViewport.Height = mHeight;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	//Define scisor rect size
	mRect.left = 0;
	mRect.right = mViewport.Width;
	mRect.top = 0;
	mRect.bottom = mViewport.Height;


	//Create + Initialize a camera
	mECSManager = mECSManager->Get();
	mCamera = mECSManager->Create();
	InputControlerComponent* inputControlerComponent = new InputControlerComponent();
	TransformComponent* transformComponent = new TransformComponent();
	CameraComponent* cameraComponent = new CameraComponent();

	inputControlerComponent->Initialize();
	transformComponent->Initialize();
	cameraComponent->Initialize();

	mECSManager->addComponent(inputControlerComponent, mCamera);
	mECSManager->addComponent(transformComponent, mCamera);
	mECSManager->addComponent(cameraComponent, mCamera);

	cameraComponent->SetProjection(45.0f);

	mpAssetManager->Initialize(mpD3D12Manager);

	//World light
	mLightPos = DirectX::XMFLOAT4(0.0f, 10.0f, 0.0f, 0.0f);
	
}

void GraphicEngine::UpdateDraw()
{


	if (GetAsyncKeyState(0x4B) & 0x8000) mLightPos.x -= 0.02f;
	if (GetAsyncKeyState(0x4D) & 0x8000) mLightPos.x += 0.02f;
	if (GetAsyncKeyState(0x4F) & 0x8000) mLightPos.y += 0.02f;
	if (GetAsyncKeyState(0x4C) & 0x8000) mLightPos.y -= 0.02f;
	if (GetAsyncKeyState(0x49) & 0x8000) mLightPos.z += 0.02f;
	if (GetAsyncKeyState(0x50) & 0x8000) mLightPos.z -= 0.02f;

	UpdateWorldMatrix();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = mpDxgiManager->GetCurrentRenderTarget();
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	mpD3D12Manager->GetGraphicsCommandList()->ResourceBarrier(1, &barrier);


	const float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mpDxgiManager->GetRenderTargetViewHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = mpD3D12Manager->GetD3D12DepthDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

	mpD3D12Manager->GetGraphicsCommandList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	mpD3D12Manager->GetGraphicsCommandList()->ClearRenderTargetView(rtvHandle, color, 0, 0);
	mpD3D12Manager->GetGraphicsCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, 0);

	mpD3D12Manager->GetGraphicsCommandList()->RSSetViewports(1, &mViewport);
	mpD3D12Manager->GetGraphicsCommandList()->RSSetScissorRects(1, &mRect);

	DrawScene();

	RenderUIElements();
	
	ApplyPostProcessing();

	UpdateMeshToDelete();


	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mpD3D12Manager->GetGraphicsCommandList()->ResourceBarrier(1, &barrier);

	mpD3D12Manager->GetGraphicsCommandList()->Close();
	mpD3D12Manager->ExecuteTheCommandList(mpD3D12Manager->GetD3D12CommandList());

	mpDxgiManager->Present(true);

	while (mpD3D12Manager->GetD3D12Fence()->GetCompletedValue() < mpD3D12Manager->GetD3D12CurrentFenceValue());
	{
		_mm_pause();
	}

	mpD3D12Manager->ResetCommandList();
}

void GraphicEngine::DrawScene()
{
	mpD3D12Manager->GetGraphicsCommandList()->SetGraphicsRootSignature(mpD3D12Manager->GetD3D12RootSignature());
	mpD3D12Manager->GetGraphicsCommandList()->SetPipelineState(mpD3D12Manager->GetD3D12PipelineState3DObject());
	mpD3D12Manager->GetGraphicsCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	Frustum frustum = GetFrustrum(GetCamera()->GetViewMatrix() * GetCamera()->GetProjectionMatrix());

	for (const auto& object : mObjects)
	{

		if (!object->boundingBox.IsInFrustum(frustum))
		{
			continue;
		}

		DirectX::XMFLOAT4X4 matrix;
		DirectX::XMStoreFloat4x4(&matrix, object->localMatrix);

		object->boundingBox.center = { matrix._41, matrix._42, matrix._43 };

		Constant cc = {};

		cc.m_world = object->worldMatrix;
		cc.m_view = GetCamera()->GetViewMatrix();
		cc.m_proj = GetCamera()->GetProjectionMatrix();
		cc.mLightDirection = mLightPos;
		cc.mCameraPosition = GetCameraTransform()->vPosition;

		void* pConstantData;
		object->constantBuffer->Map(0, 0, &pConstantData);
		memcpy(pConstantData, &cc, sizeof(Constant));
		object->constantBuffer->Unmap(0, 0);

		mpD3D12Manager->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(0, object->constantBuffer->GetGPUVirtualAddress());

		ID3D12DescriptorHeap* heaps[] = { object->srvHeap };
		mpD3D12Manager->GetGraphicsCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
		mpD3D12Manager->GetGraphicsCommandList()->SetGraphicsRootDescriptorTable(1, object->srvHeap->GetGPUDescriptorHandleForHeapStart());


		mpD3D12Manager->GetGraphicsCommandList()->IASetVertexBuffers(0, 1, &object->vertexBufferView);
		mpD3D12Manager->GetGraphicsCommandList()->IASetIndexBuffer(&object->indexBufferView);
		mpD3D12Manager->GetGraphicsCommandList()->DrawIndexedInstanced(static_cast<UINT>(object->indexBufferView.SizeInBytes / sizeof(unsigned int)), 1, 0, 0, 0);

	}
}

void GraphicEngine::RenderUIElements()
{
	ID3D12GraphicsCommandList* cmdList = mpD3D12Manager->GetGraphicsCommandList();

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->SetPipelineState(mpD3D12Manager->GetD3D12PipelineStateUiElement());

	for (const auto& ui : mActiveUIElements)
	{
		if (!ui->srvHeap || !ui->vertexBuffer || !ui->indexBuffer) continue;

		Constant cc = {};

		cc.m_world = DirectX::XMMatrixScaling(ui->size.x, ui->size.y, 1.0f) * DirectX::XMMatrixTranslation(ui->position.x, ui->position.y, 0.0f);
		cc.m_view = DirectX::XMMatrixIdentity();
		cc.m_proj = DirectX::XMMatrixOrthographicLH(mWidth, mHeight, 0.1f, 100.0f);


		void* pConstantData;
		ui->constantBuffer->Map(0, 0, &pConstantData);
		memcpy(pConstantData, &cc, sizeof(Constant));
		ui->constantBuffer->Unmap(0, 0);


		mpD3D12Manager->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(0, ui->constantBuffer->GetGPUVirtualAddress());

		ID3D12DescriptorHeap* heaps[] = { ui->srvHeap };
		cmdList->SetDescriptorHeaps(1, heaps);
		cmdList->SetGraphicsRootDescriptorTable(1, ui->srvHeap->GetGPUDescriptorHandleForHeapStart());

		cmdList->IASetVertexBuffers(0, 1, &ui->vertexBufferView);
		cmdList->IASetIndexBuffer(&ui->indexBufferView);
		cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}


void GraphicEngine::CreateUI(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 sz, LPCWSTR tex, unsigned int id)
{
	ID3D12DescriptorHeap* texture = mpAssetManager->GetTexture(tex);

	UIElement* ui = new UIElement(pos, sz, texture);
	ui->id = id;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	Vertex vertices[] =
	{
		{{ -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f }},
		{{  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }},
		{{ -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f }},
		{{  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f }},
	};

	uint32_t indices[] = { 0, 1, 2, 2, 1, 3 };

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(),
		sizeof(vertices), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, &ui->vertexBuffer);

	mpD3D12Manager->Upload(ui->vertexBuffer, vertices, sizeof(vertices),
		mpD3D12Manager->GetD3D12CommandList(), mpD3D12Manager->GetD3D12CommandQueue(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);


	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(),
		sizeof(indices), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, &ui->indexBuffer);

	mpD3D12Manager->Upload(ui->indexBuffer, indices, sizeof(indices),
		mpD3D12Manager->GetD3D12CommandList(), mpD3D12Manager->GetD3D12CommandQueue(), D3D12_RESOURCE_STATE_INDEX_BUFFER);


	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(Constant),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ,
		&ui->constantBuffer);

	ui->vertexBufferView.BufferLocation = ui->vertexBuffer->GetGPUVirtualAddress();
	ui->vertexBufferView.StrideInBytes = sizeof(Vertex);
	ui->vertexBufferView.SizeInBytes = sizeof(vertices);

	ui->indexBufferView.BufferLocation = ui->indexBuffer->GetGPUVirtualAddress();
	ui->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	ui->indexBufferView.SizeInBytes = sizeof(indices);

	float x = (ui->position.x / mWidth) * 2.0f - 1.0f;
	float y = 1.0f - (ui->position.y / mHeight) * 2.0f;

	ui->transform = DirectX::XMMatrixTranslation(x, y, 0.0f);


	mUIElements.push_back(ui);
}

void GraphicEngine::CreatePostProcess(float brightness, float contrast, float saturation, float damageIntensity, unsigned int id)
{

	PostProcessQuad* ppQuad = new PostProcessQuad();
	ppQuad->ppp.brightness = brightness;
	ppQuad->ppp.contrast = contrast;
	ppQuad->ppp.saturation = saturation;
	ppQuad->ppp.damageIntensity = damageIntensity;
	ppQuad->id = id;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	Vertex vertices[] =
	{
		{ { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } },
		{ {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } },
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
		{ {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } }
	};

	uint32_t indices[] = { 0, 1, 2, 2, 1, 3 };

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(),
		sizeof(vertices), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, &ppQuad->vertexBuffer);

	mpD3D12Manager->Upload(ppQuad->vertexBuffer, vertices, sizeof(vertices),
		mpD3D12Manager->GetD3D12CommandList(), mpD3D12Manager->GetD3D12CommandQueue(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(),
		sizeof(indices), D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON, &ppQuad->indexBuffer);

	mpD3D12Manager->Upload(ppQuad->indexBuffer, indices, sizeof(indices),
		mpD3D12Manager->GetD3D12CommandList(), mpD3D12Manager->GetD3D12CommandQueue(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

	ppQuad->vertexBufferView.BufferLocation = ppQuad->vertexBuffer->GetGPUVirtualAddress();
	ppQuad->vertexBufferView.StrideInBytes = sizeof(Vertex);
	ppQuad->vertexBufferView.SizeInBytes = sizeof(vertices);

	ppQuad->indexBufferView.BufferLocation = ppQuad->indexBuffer->GetGPUVirtualAddress();
	ppQuad->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	ppQuad->indexBufferView.SizeInBytes = sizeof(indices);

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(PostProcessParams),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ,
		&ppQuad->constantBuffer);

	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = 1;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	if (FAILED(mpD3D12Manager->GetD3D12Device()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&ppQuad->srvHeap))))
	{
		PRINT_CONSOLE_OUTPUT("Error : creating the post process srvHeap!");
		return;
	}

	UpdatePostProcessSRV(ppQuad);


	mPPQuad.push_back(ppQuad);
}

void GraphicEngine::SetActiveUI(unsigned int id)
{
	for (UIElement* ui : mUIElements)
	{
		if (ui->id == id)
		{
			if (std::find(mActiveUIElements.begin(), mActiveUIElements.end(), ui) == mActiveUIElements.end())
			{
				mActiveUIElements.push_back(ui);
			}
			return;
		}
	}
}

void GraphicEngine::SetActivePostProcessing(unsigned int id)
{
	for (PostProcessQuad* ppQ : mPPQuad)
	{
		if (ppQ->id == id)
		{
			if (std::find(mActivePostProcesses.begin(), mActivePostProcesses.end(), ppQ) == mActivePostProcesses.end())
			{
				mActivePostProcesses.push_back(ppQ);
			}
			return;
		}
	}
}

void GraphicEngine::RemoveActiveUI(unsigned int id)
{
	for (auto it = mActiveUIElements.begin(); it != mActiveUIElements.end(); ++it)
	{
		if ((*it)->id == id)
		{
			mActiveUIElements.erase(it);
			return;
		}
	}
}

void GraphicEngine::RemoveActivePostProcessing(unsigned int id)
{
	for (auto it = mActivePostProcesses.begin(); it != mActivePostProcesses.end(); ++it)
	{
		if ((*it)->id == id)
		{
			mActivePostProcesses.erase(it);
			return;
		}
	}
}

void GraphicEngine::ModifyPostProcess(unsigned int id, float brightness, float contrast, float saturation, float damageIntensity)
{
	for (PostProcessQuad* ppQ : mPPQuad)
	{
		if (ppQ->id == id)
		{
			ppQ->ppp.brightness = brightness;
			ppQ->ppp.contrast = contrast;
			ppQ->ppp.saturation = saturation;
			ppQ->ppp.damageIntensity = damageIntensity;

			for (PostProcessQuad* activePPQ : mActivePostProcesses)
			{
				if (activePPQ->id == id)
				{
					activePPQ->ppp.brightness = brightness;
					activePPQ->ppp.contrast = contrast;
					activePPQ->ppp.saturation = saturation;
					activePPQ->ppp.damageIntensity = damageIntensity;
				}
			}
			return;
		}
	}
}


void GraphicEngine::ApplyPostProcessing()
{
	ID3D12GraphicsCommandList* cmdList = mpD3D12Manager->GetGraphicsCommandList();

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = mpDxgiManager->GetRenderTargetViewHandle();
	cmdList->OMSetRenderTargets(1, &cpuHandle, false, nullptr);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->SetPipelineState(mpD3D12Manager->GetPostProcessPipelineState());




	for (const auto& postProcessQuad : mActivePostProcesses)
	{
		UpdatePostProcessSRV(postProcessQuad);

		if (!postProcessQuad->vertexBuffer || !postProcessQuad->indexBuffer) continue;

		PostProcessParams ppp;
		ppp.brightness = postProcessQuad->ppp.brightness;
		ppp.contrast = postProcessQuad->ppp.contrast;
		ppp.saturation = postProcessQuad->ppp.saturation;
		ppp.damageIntensity = postProcessQuad->ppp.damageIntensity;

		void* pConstantData;
		postProcessQuad->constantBuffer->Map(0, 0, &pConstantData);
		memcpy(pConstantData, &ppp, sizeof(PostProcessParams));
		postProcessQuad->constantBuffer->Unmap(0, 0);

		ID3D12DescriptorHeap* heaps[] = { postProcessQuad->srvHeap };
		cmdList->SetDescriptorHeaps(1, heaps);
		cmdList->SetGraphicsRootDescriptorTable(1, postProcessQuad->srvHeap->GetGPUDescriptorHandleForHeapStart());

		mpD3D12Manager->GetGraphicsCommandList()->SetGraphicsRootConstantBufferView(0, postProcessQuad->constantBuffer->GetGPUVirtualAddress());

		cmdList->IASetVertexBuffers(0, 1, &postProcessQuad->vertexBufferView);
		cmdList->IASetIndexBuffer(&postProcessQuad->indexBufferView);
		cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	}
}

void GraphicEngine::UpdatePostProcessSRV(PostProcessQuad* ppq)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	mpD3D12Manager->GetD3D12Device()->CreateShaderResourceView(mpDxgiManager->GetCurrentRenderTarget(), &srvDesc, ppq->srvHeap->GetCPUDescriptorHandleForHeapStart());
}

void GraphicEngine::UpdateMeshToDelete()
{
	for (Mesh* mesh : mMeshToDelete)
	{
		if (mesh->constantBuffer) mesh->constantBuffer->Release();
		if (mesh->vertexBuffer) mesh->vertexBuffer->Release();
		if (mesh->indexBuffer) mesh->indexBuffer->Release();

		delete mesh;
	}
	mMeshToDelete.clear();
}


GraphicEngine* GraphicEngine::Get()
{
	static GraphicEngine mInstance;

	return &mInstance;
}

GraphicEngine::Frustum GraphicEngine::GetFrustrum(const DirectX::XMMATRIX& viewProj)
{
	Frustum frustum;
	DirectX::XMFLOAT4X4 matrix;
	DirectX::XMStoreFloat4x4(&matrix, viewProj);

	frustum.planes[0] = { matrix._14 + matrix._11, matrix._24 + matrix._21, matrix._34 + matrix._31, matrix._44 + matrix._41 };
	
	frustum.planes[1] = { matrix._14 - matrix._11, matrix._24 - matrix._21, matrix._34 - matrix._31, matrix._44 - matrix._41 };
	
	frustum.planes[2] = { matrix._14 + matrix._12, matrix._24 + matrix._22, matrix._34 + matrix._32, matrix._44 + matrix._42 };
	
	frustum.planes[3] = { matrix._14 - matrix._12, matrix._24 - matrix._22, matrix._34 - matrix._32, matrix._44 - matrix._42 };
	
	frustum.planes[4] = { matrix._13, matrix._23, matrix._33, matrix._43 };
	
	frustum.planes[5] = { matrix._14 - matrix._13, matrix._24 - matrix._23, matrix._34 - matrix._33, matrix._44 - matrix._43 };

	for (int i = 0; i < 6; i++)
	{
		float length = sqrtf(frustum.planes[i].x * frustum.planes[i].x +
			frustum.planes[i].y * frustum.planes[i].y +
			frustum.planes[i].z * frustum.planes[i].z);
		frustum.planes[i].x /= length;
		frustum.planes[i].y /= length;
		frustum.planes[i].z /= length;
		frustum.planes[i].w /= length;
	}

	return frustum;
}

void GraphicEngine::UpdateWorldMatrix()
{
	for (auto& objects : mObjects)
	{
		if (objects->parent)
		{
			if (!std::isfinite(objects->parent->worldMatrix.r[0].m128_f32[0]))
			{
				objects->worldMatrix = objects->localMatrix;
			}
			else
			{
				objects->worldMatrix = objects->localMatrix * objects->parent->worldMatrix;
			}
		}
		else
		{
			objects->worldMatrix = objects->localMatrix;
		}
	}
}

DirectX::XMFLOAT3 GraphicEngine::ComputeNormal(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2, DirectX::XMFLOAT3 p3)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&p1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&p2);
	DirectX::XMVECTOR v3 = DirectX::XMLoadFloat3(&p3);

	DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(v2, v1);
	DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(v3, v1);

	DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(edge1, edge2));

	DirectX::XMFLOAT3 normalFloat3;
	DirectX::XMStoreFloat3(&normalFloat3, normal);
	return normalFloat3;
}

void GraphicEngine::CreateCube(float width, float height, float depth, float x, float y, float z, LPCWSTR texturePath, Mesh* parent)
{
	Mesh* newCube = new Mesh();
	newCube->width = width;
	newCube->height = height;
	newCube->depth = depth;
	newCube->x = x;
	newCube->y = y;
	newCube->z = z;
	newCube->parent = parent;

	newCube->boundingBox.center = { x, y, z };
	newCube->boundingBox.scaling = { width / 2, height / 2, depth / 2 };

	std::vector<Vertex> mCubeVertices;

	if (!isCubeInit)
	{
		mCubeVertices =
		{
			// LEFT (-X)
			{{-width / 2, -height / 2, -depth / 2}, {1.0f, 1.0f}, {-1.0f,  0.0f,  0.0f}},
			{{-width / 2, -height / 2,  depth / 2}, {0.0f, 1.0f}, {-1.0f,  0.0f,  0.0f}},
			{{-width / 2,  height / 2, -depth / 2}, {1.0f, 0.0f}, {-1.0f,  0.0f,  0.0f}},
			{{-width / 2,  height / 2,  depth / 2}, {0.0f, 0.0f}, {-1.0f,  0.0f,  0.0f}},

			// RIGHT (+X)
			{{ width / 2, -height / 2, -depth / 2}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
			{{ width / 2, -height / 2,  depth / 2}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
			{{ width / 2,  height / 2, -depth / 2}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{ width / 2,  height / 2,  depth / 2}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},

			// BOTTOM (-Y)
			{{-width / 2, -height / 2, -depth / 2}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},
			{{ width / 2, -height / 2,  depth / 2}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{-width / 2, -height / 2,  depth / 2}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}},
			{{ width / 2, -height / 2, -depth / 2}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}},

			// TOP (+Y)
			{{ -width / 2,  height / 2, -depth / 2}, {0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
			{{ width / 2,  height / 2,  depth / 2},  {1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{ width / 2, height / 2, -depth / 2},   {1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
			{{ -width / 2,  height / 2,  depth / 2}, {0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

			// FRONT (-Z)
			{{-width / 2, height / 2, -depth / 2},   {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},
			{{ width / 2, -height / 2, -depth / 2},  {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
			{{-width / 2, -height / 2, -depth / 2},  {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}},
			{{ width / 2, height / 2, -depth / 2},   {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}},

			// BACK (+Z)
			{{ -width / 2, -height / 2,  depth / 2}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
			{{ width / 2,  height / 2,  depth / 2},  {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{ -width / 2,  height / 2,  depth / 2}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{ width / 2, -height / 2,  depth / 2},  {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
		};

		cubeIndices =
		{
			0, 1, 2,
			1, 3, 2,

			4, 6, 5,
			5, 6, 7,

			8, 9, 10,
			8, 11, 9,

			12, 13, 14,
			12, 15, 13,

			16, 17, 18,
			16, 19, 17,

			20, 21, 22,
			20, 23, 21

		};

		isCubeInit = true;
	}



	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(Vertex) * mCubeVertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON,
		&newCube->vertexBuffer);
	if (!newCube->vertexBuffer) return;
	newCube->vertexBuffer->SetName(L"Cube Vertex Buffer");


	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(unsigned int) * cubeIndices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON,
		&newCube->indexBuffer);
	if (!newCube->indexBuffer) return;
	newCube->indexBuffer->SetName(L"Cube Index Buffer");

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(Constant),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ,
		&newCube->constantBuffer);
	if (!newCube->constantBuffer) return;
	newCube->constantBuffer->SetName(L"Cube Constant Buffer");

	mpD3D12Manager->Upload(newCube->vertexBuffer, mCubeVertices.data(), sizeof(Vertex) * mCubeVertices.size(),
		mpD3D12Manager->GetD3D12CommandList(), mpD3D12Manager->GetD3D12CommandQueue(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	mpD3D12Manager->Upload(newCube->indexBuffer, cubeIndices.data(), sizeof(unsigned int) * cubeIndices.size(),
		mpD3D12Manager->GetD3D12CommandList(), mpD3D12Manager->GetD3D12CommandQueue(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

	newCube->vertexBufferView.BufferLocation = newCube->vertexBuffer->GetGPUVirtualAddress();
	newCube->vertexBufferView.StrideInBytes = sizeof(Vertex);
	newCube->vertexBufferView.SizeInBytes = sizeof(Vertex) * mCubeVertices.size();

	newCube->indexBufferView.BufferLocation = newCube->indexBuffer->GetGPUVirtualAddress();
	newCube->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	newCube->indexBufferView.SizeInBytes = sizeof(unsigned int) * cubeIndices.size();

	newCube->localMatrix = DirectX::XMMatrixTranslation(x, y, z);
	newCube->worldMatrix = (parent) ? newCube->localMatrix * parent->worldMatrix : newCube->localMatrix;

	newCube->srvHeap = mpAssetManager->GetTexture(texturePath);

	mObjects.push_back(newCube);
}

GraphicEngine::Mesh* GraphicEngine::CreateSphere(float radius, int sliceCount, int stackCount, float x, float y, float z, LPCWSTR texturePath, bool isInversed, bool isMissile, Mesh* parent)
{
	Mesh* newSphere = new Mesh();
	newSphere->x = x;
	newSphere->y = y;
	newSphere->z = z;
	newSphere->parent = parent;

	newSphere->boundingBox.center = { x, y, z };
	newSphere->boundingBox.scaling = { radius / 2, radius / 2, radius / 2 };

	int totalVertices = 2 + (stackCount - 1) * (sliceCount + 1);
	int totalIndices = 6 * (stackCount - 2) * sliceCount + 6 * sliceCount;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	vertices.reserve(totalVertices);
	indices.reserve(totalIndices);

	vertices.push_back({ {0.0f, radius, 0.0f}, {0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} });

	for (int stack = 1; stack <= stackCount - 1; stack++)
	{
		float phi = DirectX::XM_PI * stack / stackCount;
		for (int slice = 0; slice <= sliceCount; slice++)
		{
			float theta = 2.0f * DirectX::XM_PI * slice / sliceCount;

			float px = radius * sinf(phi) * cosf(theta);
			float py = radius * cosf(phi);
			float pz = radius * sinf(phi) * sinf(theta);

			float u = (float)slice / sliceCount;
			float v = (float)stack / stackCount;

			vertices.push_back({ {px, py, pz}, {u, v}, {px, py, pz} });
		}
	}

	vertices.push_back({ {0.0f, -radius, 0.0f}, {0.5f, 1.0f}, {0.0f, -1.0f, 0.0f} });

	for (int slice = 0; slice < sliceCount; slice++)
	{
		indices.push_back(0);
		indices.push_back((slice + 1) % sliceCount + 1);
		indices.push_back(slice + 1);
	}

	int baseIndex = 1;
	int ringVertexCount = sliceCount + 1;

	for (int stack = 0; stack < stackCount - 2; stack++)
	{
		for (int slice = 0; slice < sliceCount; slice++)
		{
			indices.push_back(baseIndex + stack * ringVertexCount + slice);
			indices.push_back(baseIndex + stack * ringVertexCount + slice + 1);
			indices.push_back(baseIndex + (stack + 1) * ringVertexCount + slice);

			indices.push_back(baseIndex + (stack + 1) * ringVertexCount + slice);
			indices.push_back(baseIndex + stack * ringVertexCount + slice + 1);
			indices.push_back(baseIndex + (stack + 1) * ringVertexCount + slice + 1);
		}
	}

	int southPoleIndex = (int)vertices.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;

	for (int slice = 0; slice < sliceCount; slice++)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + slice);
		indices.push_back(baseIndex + (slice + 1) % sliceCount);
	}

	for (int i = 0; i < indices.size(); i += 3)
	{
		DirectX::XMFLOAT3 normal = ComputeNormal(
			vertices[indices[i]].pos,
			vertices[indices[i + 1]].pos,
			vertices[indices[i + 2]].pos);

		vertices[indices[i]].normal = normal;
		vertices[indices[i + 1]].normal = normal;
		vertices[indices[i + 2]].normal = normal;
	}

	if (isInversed)
	{
		for (size_t i = 0; i < indices.size(); i += 3) {
			std::swap(indices[i + 1], indices[i + 2]);
		}
	}

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(Vertex) * vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON,
		&newSphere->vertexBuffer);
	if (!newSphere->vertexBuffer) return nullptr;
	newSphere->vertexBuffer->SetName(L"Sphere Vertex Buffer");

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(unsigned int) * indices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON,
		&newSphere->indexBuffer);
	if (!newSphere->indexBuffer) return nullptr;
	newSphere->indexBuffer->SetName(L"Sphere Index Buffer");

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(Constant),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ,
		&newSphere->constantBuffer);
	if (!newSphere->constantBuffer) return nullptr;
	newSphere->constantBuffer->SetName(L"Sphere Constant Buffer");

	mpD3D12Manager->Upload(newSphere->vertexBuffer, vertices.data(), sizeof(Vertex) * vertices.size(),
		mpD3D12Manager->GetD3D12CommandList(), mpD3D12Manager->GetD3D12CommandQueue(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

	mpD3D12Manager->Upload(newSphere->indexBuffer, indices.data(), sizeof(unsigned int) * indices.size(),
		mpD3D12Manager->GetD3D12CommandList(), mpD3D12Manager->GetD3D12CommandQueue(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

	newSphere->vertexBufferView.BufferLocation = newSphere->vertexBuffer->GetGPUVirtualAddress();
	newSphere->vertexBufferView.StrideInBytes = sizeof(Vertex);
	newSphere->vertexBufferView.SizeInBytes = sizeof(Vertex) * vertices.size();

	newSphere->indexBufferView.BufferLocation = newSphere->indexBuffer->GetGPUVirtualAddress();
	newSphere->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	newSphere->indexBufferView.SizeInBytes = sizeof(unsigned int) * indices.size();

	newSphere->localMatrix = DirectX::XMMatrixTranslation(x, y, z);
	newSphere->worldMatrix = (parent) ? newSphere->localMatrix * parent->worldMatrix : newSphere->localMatrix;

	newSphere->srvHeap = mpAssetManager->GetTexture(texturePath);

	mObjects.push_back(newSphere);
	

	return newSphere;
}


void GraphicEngine::CreateMeshFromFile(LPCWSTR filename, float width, float height, float depth, float x, float y, float z, LPCWSTR texturePath, Mesh* parent)
{
	Mesh* newMesh = new Mesh();
	newMesh->x = x;
	newMesh->y = y;
	newMesh->z = z;
	newMesh->parent = parent;


	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::ifstream file(filename);
	if (!file.is_open()) {
		PRINT_CONSOLE_OUTPUT("Failed to open file: " << filename << std::endl);
		return;
	}

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> uvs;
	std::string line;

	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string prefix;
		ss >> prefix;

		if (prefix == "v")
		{
			float vx, vy, vz;
			ss >> vx >> vy >> vz;
			positions.push_back({ vx * width, vy * height, vz * depth });
		}
		else if (prefix == "vn")
		{
			float nx, ny, nz;
			ss >> nx >> ny >> nz;
			normals.push_back({ nx, ny, nz });
		}
		else if (prefix == "vt")
		{
			float u, v;
			ss >> u >> v;
			uvs.push_back({ u, 1.0f - v });
		}
		else if (prefix == "f")
		{
			unsigned int v[3], vt[3], vn[3];
			char slash;
			for (int i = 0; i < 3; i++) {
				ss >> v[i] >> slash >> vt[i] >> slash >> vn[i];
				indices.push_back(v[i] - 1);
				indices.push_back(vt[i] - 1);
				indices.push_back(vn[i] - 1);
			}
		}
	}
	file.close();

	std::map<std::tuple<int, int, int>, int> uniqueVertices;
	std::vector<Vertex> optimizedVertices;
	std::vector<unsigned int> optimizedIndices;

	for (size_t i = 0; i < indices.size(); i += 3) {
		int vIdx = indices[i];
		int vtIdx = indices[i + 1];
		int nIdx = indices[i + 2];

		DirectX::XMFLOAT3 normal = (nIdx < normals.size()) ? normals[nIdx] : DirectX::XMFLOAT3(0, 0, 0);
		DirectX::XMFLOAT2 uv = (vtIdx < uvs.size()) ? uvs[vtIdx] : DirectX::XMFLOAT2(0, 0);

		auto key = std::make_tuple(vIdx, vtIdx, nIdx);
		if (uniqueVertices.find(key) == uniqueVertices.end()) {
			uniqueVertices[key] = optimizedVertices.size();
			optimizedVertices.push_back({ positions[vIdx], uv, normal });
		}
		optimizedIndices.push_back(uniqueVertices[key]);
	}

	DirectX::XMFLOAT3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
	DirectX::XMFLOAT3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (const auto& pos : positions) {
		if (pos.x < min.x) min.x = pos.x;
		if (pos.y < min.y) min.y = pos.y;
		if (pos.z < min.z) min.z = pos.z;

		if (pos.x > max.x) max.x = pos.x;
		if (pos.y > max.y) max.y = pos.y;
		if (pos.z > max.z) max.z = pos.z;
	}

	newMesh->boundingBox.center.x = (min.x + max.x) / 2.0f;
	newMesh->boundingBox.center.y = (min.y + max.y) / 2.0f;
	newMesh->boundingBox.center.z = (min.z + max.z) / 2.0f;

	newMesh->boundingBox.scaling.x = (max.x - min.x) / 2.0f;
	newMesh->boundingBox.scaling.y = (max.y - min.y) / 2.0f;
	newMesh->boundingBox.scaling.z = (max.z - min.z) / 2.0f;

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(Vertex) * optimizedVertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON,
		&newMesh->vertexBuffer);

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(unsigned int) * optimizedIndices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COMMON,
		&newMesh->indexBuffer);

	mpD3D12Manager->BufferInitialize(mpD3D12Manager->GetD3D12Device(), sizeof(Constant),
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ,
		&newMesh->constantBuffer);

	mpD3D12Manager->Upload(newMesh->vertexBuffer, optimizedVertices.data(), sizeof(Vertex) * optimizedVertices.size(), mpD3D12Manager->GetD3D12CommandList(), mpD3D12Manager->GetD3D12CommandQueue(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	mpD3D12Manager->Upload(newMesh->indexBuffer, optimizedIndices.data(), sizeof(unsigned int) * optimizedIndices.size(), mpD3D12Manager->GetD3D12CommandList(), mpD3D12Manager->GetD3D12CommandQueue(), D3D12_RESOURCE_STATE_INDEX_BUFFER);

	newMesh->vertexBufferView.BufferLocation = newMesh->vertexBuffer->GetGPUVirtualAddress();
	newMesh->vertexBufferView.StrideInBytes = sizeof(Vertex);
	newMesh->vertexBufferView.SizeInBytes = sizeof(Vertex) * optimizedVertices.size();

	newMesh->indexBufferView.BufferLocation = newMesh->indexBuffer->GetGPUVirtualAddress();
	newMesh->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	newMesh->indexBufferView.SizeInBytes = sizeof(unsigned int) * optimizedIndices.size();

	newMesh->localMatrix = DirectX::XMMatrixTranslation(x, y, z);
	newMesh->worldMatrix = (parent) ? newMesh->localMatrix * parent->worldMatrix : newMesh->localMatrix;

	newMesh->srvHeap = mpAssetManager->GetTexture(texturePath);

	mObjects.push_back(newMesh);
}

void GraphicEngine::Delete(Mesh* mesh)
{

	mMeshToDelete.push_back(mesh);

	auto it = std::find(mObjects.begin(), mObjects.end(), mesh);
	if (it != mObjects.end())
	{
		mObjects.erase(it);
	}
}



bool GraphicEngine::BoundingBox::Contains(const BoundingBox& box)
{
	DirectX::XMVECTOR CenterA = DirectX::XMLoadFloat3(&center);
	DirectX::XMVECTOR ExtentsA = DirectX::XMLoadFloat3(&scaling);

	DirectX::XMVECTOR CenterB = DirectX::XMLoadFloat3(&box.center);
	DirectX::XMVECTOR ExtentsB = DirectX::XMLoadFloat3(&box.scaling);

	DirectX::XMVECTOR MinA = DirectX::XMVectorSubtract(CenterA, ExtentsA);
	DirectX::XMVECTOR MaxA = DirectX::XMVectorAdd(CenterA, ExtentsA);

	DirectX::XMVECTOR MinB = DirectX::XMVectorSubtract(CenterB, ExtentsB);
	DirectX::XMVECTOR MaxB = DirectX::XMVectorAdd(CenterB, ExtentsB);

	DirectX::XMVECTOR Disjoint = DirectX::XMVectorOrInt(DirectX::XMVectorGreater(MinA, MaxB), DirectX::XMVectorGreater(MinB, MaxA));

	DirectX::XMVECTOR C = DirectX::XMVectorSwizzle<DirectX::XM_SWIZZLE_X, DirectX::XM_SWIZZLE_Y, DirectX::XM_SWIZZLE_Z, DirectX::XM_SWIZZLE_X>(Disjoint);

	if (DirectX::XMComparisonAnyTrue(DirectX::XMVector4EqualIntR(C, DirectX::XMVectorTrueInt())))
	{
		return false;
	}



	DirectX::XMVECTOR Inside = DirectX::XMVectorAndInt(DirectX::XMVectorLessOrEqual(MinA, MinB), DirectX::XMVectorLessOrEqual(MaxB, MaxA));

	C = DirectX::XMVectorSwizzle<DirectX::XM_SWIZZLE_X, DirectX::XM_SWIZZLE_Y, DirectX::XM_SWIZZLE_Z, DirectX::XM_SWIZZLE_X>(Inside);

	if (DirectX::XMComparisonAllTrue(DirectX::XMVector4EqualIntR(C, DirectX::XMVectorTrueInt())))
	{
		return true;
	}

	return false;
}

bool GraphicEngine::BoundingBox::Intersects(const BoundingBox& box)
{
	DirectX::XMVECTOR CenterA = DirectX::XMLoadFloat3(&center);
	DirectX::XMVECTOR ExtentsA = DirectX::XMLoadFloat3(&scaling);

	DirectX::XMVECTOR CenterB = DirectX::XMLoadFloat3(&box.center);
	DirectX::XMVECTOR ExtentsB = DirectX::XMLoadFloat3(&box.scaling);

	DirectX::XMVECTOR MinA = DirectX::XMVectorSubtract(CenterA, ExtentsA);
	DirectX::XMVECTOR MaxA = DirectX::XMVectorAdd(CenterA, ExtentsA);

	DirectX::XMVECTOR MinB = DirectX::XMVectorSubtract(CenterB, ExtentsB);
	DirectX::XMVECTOR MaxB = DirectX::XMVectorAdd(CenterB, ExtentsB);

	DirectX::XMVECTOR Disjoint = DirectX::XMVectorOrInt(DirectX::XMVectorGreater(MinA, MaxB), DirectX::XMVectorGreater(MinB, MaxA));

	DirectX::XMVECTOR C = DirectX::XMVectorSwizzle<DirectX::XM_SWIZZLE_X, DirectX::XM_SWIZZLE_Y, DirectX::XM_SWIZZLE_Z, DirectX::XM_SWIZZLE_X>(Disjoint);

	if (DirectX::XMComparisonAnyTrue(DirectX::XMVector4EqualIntR(C, DirectX::XMVectorTrueInt())))
	{
		return false;
	}

	return true;
}

bool GraphicEngine::BoundingBox::IsInFrustum(const Frustum& frustum)
{
	for (int i = 0; i < 6; i++)
	{
		float distance = frustum.planes[i].x * center.x +
			frustum.planes[i].y * center.y +
			frustum.planes[i].z * center.z +
			frustum.planes[i].w;
		if (distance < -sqrtf(scaling.x * scaling.x + scaling.y * scaling.y + scaling.z * scaling.z))
		{
			return false;
		}
	}
	return true;
}

GraphicEngine::UIElement::UIElement(DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 sz, ID3D12DescriptorHeap* tex) : position(pos), size(sz), srvHeap(tex), id(0)
{
	transform = DirectX::XMMatrixTranslation(pos.x, pos.y, 0.0f);
}

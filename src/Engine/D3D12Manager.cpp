#include "pch.h"
#include "D3D12Manager.h"


D3D12Manager::~D3D12Manager()
{
	
}


void D3D12Manager::D3D12DeviceInitialize(IDXGIAdapter* pAdapter)
{
	if (D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mDevice)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT(" Error creating the device, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}
}

void D3D12Manager::D3D12CommandQueueInitialize(ID3D12Device* pDevice)
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	if (pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&mCommandQueue)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT(" Error creating the command queue, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}

	if (pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT(" Error creating the fence, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}
}

void D3D12Manager::D3D12CommandListInitialize(ID3D12Device* pDevice)
{
	if (pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mAllocator)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT(" Error creating the command allocator, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}

	if (pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mAllocator, nullptr, IID_PPV_ARGS(&mCommandList)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT(" Error creating the command list, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}
}

void D3D12Manager::D3D12ShaderInitialize(const LPCWSTR filename, const ShaderType shaderType, ID3DBlob** ppBlob)
{
	UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ALL_RESOURCES_BOUND;

	const char* targetType = "";
	const char* entryPoint = "";

	switch (shaderType)
	{
	case ShaderType::VERTEX:
		targetType = "vs_5_1";
		entryPoint = "vsmain";
		break;

	case ShaderType::PIXEL:
		targetType = "ps_5_1";
		entryPoint = "psmain";
		break;

	default:

		PRINT_CONSOLE_OUTPUT("Error unspported shader target type, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
		return;
	}

	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(filename, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, targetType, flags, 0, ppBlob, &errorBlob);

	if (hr != S_OK)
	{
		//If error code = -2147024893 error path
		//Can be checked with programmer calculator in DEC row to translate in HEX
		PRINT_CONSOLE_OUTPUT("Shader loading error was: " << hr << ", At file :" << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}

	if (errorBlob)
	{
		PRINT_CONSOLE_OUTPUT("Shader compliation error: " << (const char*)errorBlob->GetBufferPointer() << ", At file : " << __FILE__ << std::endl);
		errorBlob->Release();
		errorBlob = nullptr;
		return;
	}



	PRINT_CONSOLE_OUTPUT("Load the shader : " << filename << std::endl);
}

void D3D12Manager::D3D12RootSignatureInitialize(ID3D12Device* pDevice)
{
	D3D12_ROOT_PARAMETER rootParam[2];
	rootParam[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParam[0].Descriptor.RegisterSpace = 0;
	rootParam[0].Descriptor.ShaderRegister = 0; // b0
	rootParam[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParam[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParam[1].DescriptorTable.NumDescriptorRanges = 1;
	D3D12_DESCRIPTOR_RANGE srvRange = {};
	srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange.NumDescriptors = 1;
	srvRange.BaseShaderRegister = 0; // t0
	srvRange.RegisterSpace = 0;
	srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	rootParam[1].DescriptorTable.pDescriptorRanges = &srvRange;
	rootParam[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = 0;  // s0
	samplerDesc.RegisterSpace = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;




	D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc = {};
	desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;
	desc.Desc_1_0.NumParameters = 2;
	desc.Desc_1_0.pParameters = rootParam;
	desc.Desc_1_0.NumStaticSamplers = 1;
	desc.Desc_1_0.pStaticSamplers = &samplerDesc;
	desc.Desc_1_0.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* pSerializedBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	if (D3D12SerializeVersionedRootSignature(&desc, &pSerializedBlob, &pErrorBlob) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT("Rootsignature serialization error: " << (const char*)pErrorBlob->GetBufferPointer() << std::endl);
	}

	if (pDevice->CreateRootSignature(0, pSerializedBlob->GetBufferPointer(), pSerializedBlob->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT("Error creating the root signature" << std::endl);
	}
}

void D3D12Manager::D3D12PipelineState3DObjectInitialize(ID3D12Device* pDevice)
{
	D3D12RootSignatureInitialize(pDevice);
	//D3D12ShaderInitialize(L"D:/Code/CPP/tools-Mateo/SolutionGenerator/x64/Release/lyo-t2-amiga-p7-07/src/Application/VS.hlsl", ShaderType::VERTEX, &mShader[0]);
	//D3D12ShaderInitialize(L"D:/Code/CPP/tools-Mateo/SolutionGenerator/x64/Release/lyo-t2-amiga-p7-07/src/Application/PS.hlsl", ShaderType::PIXEL, &mShader[1] );

	D3D12ShaderInitialize(L"../../src/Application/VS.hlsl", ShaderType::VERTEX, &mShader[0]);
	D3D12ShaderInitialize(L"../../src/Application/PS.hlsl", ShaderType::PIXEL, &mShader[1]);


	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
	pipelineDesc.pRootSignature = mRootSignature;
	pipelineDesc.VS.pShaderBytecode = mShader[0]->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = mShader[0]->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = mShader[1]->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = mShader[1]->GetBufferSize();

	pipelineDesc.BlendState.AlphaToCoverageEnable = false;
	pipelineDesc.BlendState.IndependentBlendEnable = false;
	pipelineDesc.BlendState.RenderTarget[0].BlendEnable = false;
	pipelineDesc.BlendState.RenderTarget[0].LogicOpEnable = false;
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	pipelineDesc.SampleMask = 0xFFFFFFFF;
	pipelineDesc.SampleDesc = { 1, 0 };

	pipelineDesc.RasterizerState = {};
	//Can be change to wireframe
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	pipelineDesc.RasterizerState.FrontCounterClockwise = false;
	pipelineDesc.RasterizerState.DepthClipEnable = true;
	pipelineDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineDesc.DepthStencilState.DepthEnable = true;
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputLayout;
	inputLayoutDesc.NumElements = _countof(inputLayout);

	pipelineDesc.InputLayout = inputLayoutDesc;
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	pipelineDesc.NodeMask = 0;
	pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;


	if (pDevice->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&mPipelineState3DObject)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT("Error creating the graphics pipeline state" << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}
}

void D3D12Manager::D3D12PipelineStateUiElementInitialize(ID3D12Device* pDevice)
{
	D3D12RootSignatureInitialize(pDevice);

	D3D12ShaderInitialize(L"../../src/Application/UI.hlsl", ShaderType::VERTEX, &mShaderUI[0]);
	D3D12ShaderInitialize(L"../../src/Application/UI.hlsl", ShaderType::PIXEL, &mShaderUI[1]);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
	pipelineDesc.pRootSignature = mRootSignature;
	pipelineDesc.VS.pShaderBytecode = mShaderUI[0]->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = mShaderUI[0]->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = mShaderUI[1]->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = mShaderUI[1]->GetBufferSize();

	pipelineDesc.BlendState.AlphaToCoverageEnable = false;
	pipelineDesc.BlendState.IndependentBlendEnable = false;
	pipelineDesc.BlendState.RenderTarget[0].BlendEnable = true;
	pipelineDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	pipelineDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	pipelineDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	pipelineDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	pipelineDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	pipelineDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	pipelineDesc.SampleMask = 0xFFFFFFFF;
	pipelineDesc.SampleDesc = { 1, 0 };

	pipelineDesc.RasterizerState = {};
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineDesc.RasterizerState.FrontCounterClockwise = false;
	pipelineDesc.RasterizerState.DepthClipEnable = true;
	pipelineDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	pipelineDesc.DepthStencilState.DepthEnable = false;
	pipelineDesc.DepthStencilState.StencilEnable = false;
	pipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;


	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputLayout;
	inputLayoutDesc.NumElements = _countof(inputLayout);

	pipelineDesc.InputLayout = inputLayoutDesc;
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
	pipelineDesc.NodeMask = 0;
	pipelineDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	if (pDevice->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&mPipelineStateUiElement)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT("Error creating the UI graphics pipeline state" << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}
}

void D3D12Manager::D3D12PipelineStatePostProcessInitialize(ID3D12Device* pDevice)
{
	D3D12RootSignatureInitialize(pDevice);

	// Charger les shaders de post-processing
	D3D12ShaderInitialize(L"../../src/Application/PostProcess.hlsl", ShaderType::VERTEX, &mShaderPostProcess[0]);
	D3D12ShaderInitialize(L"../../src/Application/PostProcess.hlsl", ShaderType::PIXEL, &mShaderPostProcess[1]);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc = {};
	pipelineDesc.pRootSignature = mRootSignature;
	pipelineDesc.VS.pShaderBytecode = mShaderPostProcess[0]->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = mShaderPostProcess[0]->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = mShaderPostProcess[1]->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = mShaderPostProcess[1]->GetBufferSize();

	pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	pipelineDesc.SampleMask = 0xFFFFFFFF;
	pipelineDesc.SampleDesc = { 1, 0 };
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineDesc.RasterizerState.DepthClipEnable = true;
	pipelineDesc.DepthStencilState.DepthEnable = false;
	pipelineDesc.DepthStencilState.StencilEnable = false;
	pipelineDesc.DSVFormat = DXGI_FORMAT_UNKNOWN;

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineDesc.NumRenderTargets = 1;
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;

	if (pDevice->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&mPipelineStatePostProcessing)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT("Erreur : Impossible de créer la pipeline de post-processing !");
	}
}

void D3D12Manager::BufferInitialize(ID3D12Device* pDevice, unsigned int numberOfBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState, ID3D12Resource** ppRessource)
{
	if (!ppRessource)
	{
		return;
	}

	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = heapType;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = numberOfBytes;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	if (FAILED(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, initialState, nullptr, IID_PPV_ARGS(ppRessource))))
	{
		PRINT_CONSOLE_OUTPUT(" Error creating the buffer, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}
}

void D3D12Manager::DepthBufferInitialize(ID3D12Device* pDevice, const unsigned int width, const unsigned int height, ID3D12Resource** ppRessource)
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Alignment = 0;
	resDesc.Width = width;
	resDesc.Height = height;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 0;
	resDesc.Format = DXGI_FORMAT_D32_FLOAT;
	resDesc.SampleDesc = { 1, 0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE value = {};
	value.Format = DXGI_FORMAT_D32_FLOAT;
	value.DepthStencil.Depth = 1.0f;
	value.DepthStencil.Stencil = 0.0f;

	if (FAILED(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &value, IID_PPV_ARGS(ppRessource))))
	{
		PRINT_CONSOLE_OUTPUT(" Error creating the depth buffer, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}
}

void D3D12Manager::DepthDescriptorHeapInitialize(ID3D12Device* pDevice)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desc.NumDescriptors = 1;
	desc.NodeMask = 0;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mDepthDescriptorHeap)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT(" Error creating the depth descriptor heap, At file: " << __FILE__ << ", At line : " << __LINE__ << std::endl);
	}
}

void D3D12Manager::DepthStencilViewInitialize(ID3D12Device* pDevice)
{
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	pDevice->CreateDepthStencilView(mDepthBuffer, &dsvDesc, mDepthDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

void D3D12Manager::Upload(ID3D12Resource* pDest, void* pData, const UINT size, ID3D12CommandList* pCommandList, ID3D12CommandQueue* pCommandQueue, D3D12_RESOURCE_STATES state)
{
	if (!pDest || !pData || !pCommandList)
	{
		PRINT_CONSOLE_OUTPUT("Error: Invalid Upload parameters!" << std::endl);
		return;
	}

	ID3D12Resource* uploadBuffer = nullptr;

	BufferInitialize(mDevice, size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, &uploadBuffer);

	void* memory = nullptr;
	HRESULT hr = uploadBuffer->Map(0, nullptr, &memory);
	if (FAILED(hr))
	{
		PRINT_CONSOLE_OUTPUT("Error: Failed to map Upload Buffer!" << std::endl);
		uploadBuffer->Release();
		return;
	}

	memcpy(memory, pData, size);
	uploadBuffer->Unmap(0, nullptr);

	if (!pDest)
		return;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = pDest;
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;

	GetGraphicsCommandList()->ResourceBarrier(1, &barrier);

	GetGraphicsCommandList()->CopyBufferRegion(pDest, 0, uploadBuffer, 0, size);

	barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = pDest;
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = (state == D3D12_RESOURCE_STATE_COMMON ? D3D12_RESOURCE_STATE_COMMON : state);

	GetGraphicsCommandList()->ResourceBarrier(1, &barrier);

	GetGraphicsCommandList()->Close();
	ExecuteTheCommandList(pCommandList);

	ID3D12Fence* fence = nullptr;
	UINT64 fenceValue = 1;
	hr = mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	if (FAILED(hr))
	{
		PRINT_CONSOLE_OUTPUT("Error: Failed to create Fence!" << std::endl);
		uploadBuffer->Release();
		return;
	}

	HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!fenceEvent)
	{
		PRINT_CONSOLE_OUTPUT("Error: Failed to create Fence Event!" << std::endl);
		fence->Release();
		uploadBuffer->Release();
		return;
	}

	pCommandQueue->Signal(fence, fenceValue);
	if (fence->GetCompletedValue() < fenceValue)
	{
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}

	CloseHandle(fenceEvent);
	fence->Release();
	uploadBuffer->Release();

	ResetCommandList();
}

ID3D12GraphicsCommandList* D3D12Manager::GetGraphicsCommandList()
{
	if (!mCommandList) {
		PRINT_CONSOLE_OUTPUT("Error: Command List is null!");
		return nullptr;
	}

	return (ID3D12GraphicsCommandList*)mCommandList;
}

void D3D12Manager::ExecuteTheCommandList(ID3D12CommandList* pCommandList)
{
	mCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&pCommandList);
	mCurrentFenceValue++;
	mCommandQueue->Signal(mFence, mCurrentFenceValue);
}

void D3D12Manager::ResetCommandList()
{
	mAllocator->Reset();
	GetGraphicsCommandList()->Reset(mAllocator, 0);
}

void D3D12Manager::FlushQueue(UINT bufferCount)
{
	if (mCommandQueue)
	{
		for (int i = 0; i < bufferCount; i++)
		{
			mCommandQueue->Signal(mFence, mCurrentFenceValue + i);
		}
	}
}

void D3D12Manager::Release()
{
	if (mDevice) { mDevice->Release(); mDevice = nullptr; }
	if (mCommandQueue) { mCommandQueue->Release(); mCommandQueue = nullptr; }
	if (mCommandList) { mCommandList->Release(); mCommandList = nullptr; }
	if (mFence) { mFence->Release(); mFence = nullptr; }
	if (mAllocator) { mAllocator->Release(); mAllocator = nullptr; }
	if (mRootSignature) { mRootSignature->Release(); mRootSignature = nullptr; }
	if (mPipelineState3DObject) { mPipelineState3DObject->Release(); mPipelineState3DObject = nullptr; }
	if (mPipelineStateUiElement) { mPipelineStateUiElement->Release(); mPipelineStateUiElement = nullptr; }

	if (mBlob) { mBlob->Release(); mBlob = nullptr; }
	if (mShader[VERTEX]) { mShader[VERTEX]->Release(); mShader[VERTEX] = nullptr; }
	if (mShader[PIXEL]) { mShader[PIXEL]->Release(); mShader[PIXEL] = nullptr; }
	if (mShaderUI[VERTEX]) { mShaderUI[VERTEX]->Release(); mShaderUI[VERTEX] = nullptr; }
	if (mShaderUI[PIXEL]) { mShaderUI[PIXEL]->Release(); mShaderUI[PIXEL] = nullptr; }

	if (mDepthBuffer) { mDepthBuffer->Release(); mDepthBuffer = nullptr; }
	if (mDepthDescriptorHeap) { mDepthDescriptorHeap->Release(); mDepthDescriptorHeap = nullptr; }
}

#include "pch.h"
#include "PipelineStateObjectManager.h"


PipelineStateObjectManager::PipelineStateObjectManager() : m_pDevice(nullptr), m_pRootSignature(nullptr)
{
}

PipelineStateObjectManager::~PipelineStateObjectManager()
{
    for (auto& pso : m_pipelineStates)
    {
        delete pso.second;
    }
    m_pipelineStates.clear();
}

void PipelineStateObjectManager::Initialize(ID3D12Device* device)
{
    m_pDevice = device;
}

void PipelineStateObjectManager::CreatePipelineState(const char* name, const std::wstring& shaderPath)
{
    if (!m_pDevice)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER] Error: PSO device was not initialized ! \n");
        return;
    }

    ComPtr<ID3DBlob> vsBlob = CompileShader(shaderPath, "vs_5_0");
    ComPtr<ID3DBlob> psBlob = CompileShader(shaderPath, "ps_5_0");

    CreateRootSignature(name);

    std::string shaderPathStr(shaderPath.begin(), shaderPath.end());
    auto inputLayoutElements = ParseVertexInStruct(shaderPathStr);

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
    auto inputLayoutDesc = CreateInputLayoutDescFromElements(inputLayoutElements, inputElementDescs);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = inputLayoutDesc;
    psoDesc.pRootSignature = m_pRootSignature.Get();
    psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
    
    
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencilDesc.StencilEnable = FALSE;

    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
    psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    psoDesc.RasterizerState.DepthClipEnable = TRUE;
    psoDesc.RasterizerState.MultisampleEnable = FALSE;
    psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
    psoDesc.RasterizerState.ForcedSampleCount = 0;
    psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
    psoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
    psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
    psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
    psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    psoDesc.DepthStencilState = depthStencilDesc;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    ComPtr<ID3D12PipelineState> pso;
    if (FAILED(m_pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso))))
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error failed to create pso ! \n");
        return;
    }

    PipelineStateObjectManager::PipelineStateConfig* config = new PipelineStateConfig();
    config->shaderPath = shaderPath.c_str();
    config->pipelineState = pso;
    config->rootSignature = m_pRootSignature;

    m_pipelineStates[name] = config;
}

int PipelineStateObjectManager::GetShaderPosition()
{
    return shaderPos;
}

PipelineStateObjectManager::PipelineStateConfig* PipelineStateObjectManager::Get(const char* name)
{
    auto it = m_pipelineStates.find(name);
    if (it != m_pipelineStates.end())
    {
        return it->second;
    }
    return nullptr;
}

void PipelineStateObjectManager::CreateRootSignature(const char* name)
{
    if (m_pRootSignature)
    {
        m_pRootSignature->Release();
        m_pRootSignature = nullptr;
    }

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = 5;
    rootSignatureDesc.NumStaticSamplers = 1;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    D3D12_ROOT_PARAMETER rootParameters[5] = {};

    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    rootParameters[0].Descriptor.RegisterSpace = 0;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].Descriptor.ShaderRegister = 1;
    rootParameters[1].Descriptor.RegisterSpace = 0;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[2].Descriptor.ShaderRegister = 2;
    rootParameters[2].Descriptor.RegisterSpace = 0;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[3].Descriptor.ShaderRegister = 3;
    rootParameters[3].Descriptor.RegisterSpace = 0;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
    D3D12_DESCRIPTOR_RANGE srvRange = {};
    srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    srvRange.NumDescriptors = 1;
    srvRange.BaseShaderRegister = 0; // t0
    srvRange.RegisterSpace = 0;
    srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    rootParameters[4].DescriptorTable.pDescriptorRanges = &srvRange;
    rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    shaderPos = 4;

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

    D3D12_STATIC_SAMPLER_DESC samplers[] = { samplerDesc };
    rootSignatureDesc.pStaticSamplers = samplers;

    rootSignatureDesc.pParameters = rootParameters;

    ID3DBlob* signature = nullptr;
    ID3DBlob* error = nullptr;

    if (D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error failed to serialize the root signature! \n");
    }

    if (m_pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error failed to create the root signature! \n");
    }

    signature->Release();
}


ComPtr<ID3DBlob> PipelineStateObjectManager::CompileShader(const std::wstring& path, const char* target)
{
    UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ALL_RESOURCES_BOUND;

    const char* entryPoint = (!target || strcmp(target, "vs_5_0") != 0) ? "psmain" : "vsmain";

    ComPtr<ID3DBlob> compiledShader;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(path.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, flags, 0, &compiledShader, &errorBlob);

    if (FAILED(hr))
    {
        PRINT_CONSOLE_OUTPUT("Shader loading error was: " << hr << ", for shader :" << path << "\n");
        if (errorBlob)
        {
            PRINT_CONSOLE_OUTPUT("Shader compilation error: " << (const char*)errorBlob->GetBufferPointer() << ", for shader :" << path << "\n");
        }
        return nullptr;
    }

    return compiledShader;
}


PipelineStateObjectManager::T_DXGI_INFO PipelineStateObjectManager::HlslTypeToDxgiFormat(const std::string& type)
{
    if (type == "float")   return { DXGI_FORMAT_R32_FLOAT, 4};
    if (type == "float2")  return {DXGI_FORMAT_R32G32_FLOAT, 8};
    if (type == "float3")  return {DXGI_FORMAT_R32G32B32_FLOAT, 12};
    if (type == "float4")  return {DXGI_FORMAT_R32G32B32A32_FLOAT, 16};
    return { DXGI_FORMAT_UNKNOWN, 0 };
}

std::vector<PipelineStateObjectManager::InputLayoutElement> PipelineStateObjectManager::ParseVertexInStruct(const std::string& hlslFilePath)
{
    std::ifstream file(hlslFilePath);
    if (!file.is_open())
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error failed to load hlsl file: " << hlslFilePath.c_str() << "\n");
        return {};
    }

    std::vector<InputLayoutElement> elements;
    std::string line;
    bool insideStruct = false;

    std::regex fieldRegex(R"(\s*(\w+)\s+(\w+)\s*:\s*(\w+)\s*;)");

    while (std::getline(file, line))
    {
        if (line.find("struct VertexIn") != std::string::npos)
        {
            insideStruct = true;
            continue;
        }

        if (insideStruct)
        {
            if (line.find("};") != std::string::npos)
            {
                break;
            }

            std::smatch match;
            if (std::regex_match(line, match, fieldRegex))
            {
                InputLayoutElement element;
                element.type = match[1];
                element.name = match[2];
                element.semanticName = match[3];
                element.dxgi_info = HlslTypeToDxgiFormat(element.type);
                elements.push_back(element);
            }
        }
    }


    return elements;
}

D3D12_INPUT_LAYOUT_DESC PipelineStateObjectManager::CreateInputLayoutDescFromElements(const std::vector<InputLayoutElement>& elements, std::vector<D3D12_INPUT_ELEMENT_DESC>& outDescs)
{
    outDescs.clear();

    UINT offset = 0;
    for (size_t i = 0; i < elements.size(); ++i)
    {
        const auto& elem = elements[i];

        D3D12_INPUT_ELEMENT_DESC desc = {};
        desc.SemanticName = elem.semanticName.c_str();
        desc.SemanticIndex = 0;
        desc.Format = elem.dxgi_info.format;
        desc.InputSlot = 0;
        desc.AlignedByteOffset = offset;
        desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        desc.InstanceDataStepRate = 0;

        offset += elem.dxgi_info.byteAlignement;
        outDescs.push_back(desc);
    }

    D3D12_INPUT_LAYOUT_DESC inputLayout = {};
    inputLayout.pInputElementDescs = outDescs.data();
    inputLayout.NumElements = static_cast<UINT>(outDescs.size());

    return inputLayout;
}

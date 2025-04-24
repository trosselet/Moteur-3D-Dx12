#include "pch.h"
#include "PipelineStateObjectManager.h"


Render::PipelineStateObjectManager::PipelineStateObjectManager() : m_pDevice(nullptr), m_pRootSignature(nullptr)
{
}

Render::PipelineStateObjectManager::~PipelineStateObjectManager()
{
    for (auto& pso : m_pipelineStates)
    {
        delete pso.second;
    }
    m_pipelineStates.clear();

    if (m_pRootSignature)
    {
        m_pRootSignature->Release();
        m_pRootSignature = nullptr;
    }
}

void Render::PipelineStateObjectManager::Initialize(ID3D12Device* device)
{
    m_pDevice = device;
}

void Render::PipelineStateObjectManager::CreatePipelineState(const char* name, const std::wstring& shaderPath, D3D12_INPUT_LAYOUT_DESC inputLayout)
{
    if (!m_pDevice)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER] Error: PSO device was not initialized ! \n")
    }

    ID3DBlob* vsBlob = CompileShader(shaderPath, "vs_5_0");
    ID3DBlob* psBlob = CompileShader(shaderPath, "ps_5_0");

    CreateRootSignature(name);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = inputLayout;
    psoDesc.pRootSignature = m_pRootSignature;
    psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };

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

    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    ID3D12PipelineState* pso = nullptr;

    if (m_pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error failed to create pso ! \n");
    }

    PipelineStateConfig* psoConfig = new PipelineStateConfig();
    psoConfig->rootSignature = m_pRootSignature;
    psoConfig->pipelineState = pso;

    m_pipelineStates[name] = psoConfig;

    vsBlob->Release();
    psBlob->Release();

}

Render::PipelineStateConfig* Render::PipelineStateObjectManager::Get(const char* name)
{
    auto it = m_pipelineStates.find(name);
    if (it != m_pipelineStates.end())
    {
        return it->second;
    }
    return nullptr;
}

void Render::PipelineStateObjectManager::CreateRootSignature(const char* name)
{
    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
    rootSignatureDesc.NumParameters = 0;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ID3DBlob* signature = nullptr;
    ID3DBlob* error = nullptr;

    if (D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error failed to serialized the root signature! \n");
    }


    if (m_pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_pRootSignature)) != S_OK)
    {
        PRINT_CONSOLE_OUTPUT("[RENDER]: Error failed to create the root signature! \n");
    }

    signature->Release();
}

ID3DBlob* Render::PipelineStateObjectManager::CompileShader(const std::wstring& path, const char* target)
{
    UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS | D3DCOMPILE_ALL_RESOURCES_BOUND;

    const char* entryPoint = "";

    if (!target)
        return nullptr;

    if (target == "vs_5_0")
    {
        entryPoint = "vsmain";
    }
    else
    {
        entryPoint = "psmain";
    }
    ID3DBlob* compiledShader = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompileFromFile(path.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, flags, 0, &compiledShader, &errorBlob);
    
    if (hr != S_OK)
    {
        //If error code = -2147024893 error path
        //Can be checked with programmer calculator in DEC row to translate in HEX
        PRINT_CONSOLE_OUTPUT("Shader loading error was: " << hr << ", for shader :" << path << "\n");
    }

    if (errorBlob)
    {
        PRINT_CONSOLE_OUTPUT("Shader compliation error: " << (const char*)errorBlob->GetBufferPointer() << ", for shader :" << path << "\n");
        errorBlob->Release();
        errorBlob = nullptr;
        return nullptr;
    }

    return compiledShader;

}

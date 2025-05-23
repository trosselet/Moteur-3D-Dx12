#ifndef PIPELINE_STATE_OBJECT_MANAGER__H
#define PIPELINE_STATE_OBJECT_MANAGER__H

#include <unordered_map>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>

#include <d3d12.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;


class PipelineStateObjectManager
{
public:
    struct PipelineStateConfig
    {
        LPCWSTR shaderPath;
        ComPtr<ID3D12RootSignature> rootSignature;
        ComPtr<ID3D12PipelineState> pipelineState;
    };

    struct T_DXGI_INFO
    {
        DXGI_FORMAT format;
        UINT byteAlignement;
    };

    struct InputLayoutElement
    {
        std::string type;
        std::string name;
        std::string semanticName;
        T_DXGI_INFO dxgi_info = { DXGI_FORMAT_UNKNOWN, 0 };
    };

public:
    PipelineStateObjectManager();
    ~PipelineStateObjectManager();

    void Initialize(ID3D12Device* device);

    void CreatePipelineState(const char* name, const std::wstring& shaderPath);

    int GetShaderPosition();

    PipelineStateConfig* Get(const char* name);

private:
    void CreateRootSignature(const char* name);
    ComPtr<ID3DBlob> CompileShader(const std::wstring& path, const char* target);

    T_DXGI_INFO HlslTypeToDxgiFormat(const std::string& type);
    std::vector<InputLayoutElement> ParseVertexInStruct(const std::string& hlslFilePath);
    D3D12_INPUT_LAYOUT_DESC CreateInputLayoutDescFromElements(const std::vector<InputLayoutElement>& elements, std::vector<D3D12_INPUT_ELEMENT_DESC>& outDescs);

private:
    ComPtr<ID3D12Device> m_pDevice;
    ComPtr<ID3D12RootSignature> m_pRootSignature;
    std::unordered_map<std::string, PipelineStateConfig*> m_pipelineStates;
    int shaderPos = 0;
};

#endif // !PIPELINE_STATE_OBJECT_MANAGER__H
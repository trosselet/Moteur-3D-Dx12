#ifndef PIPELINE_STATE_OBJECT_MANAGER__H
#define PIPELINE_STATE_OBJECT_MANAGER__H

#include <unordered_map>

namespace Render
{
	struct PipelineStateConfig
	{
		ID3D12RootSignature* rootSignature;
		ID3D12PipelineState* pipelineState;
	};

	class PipelineStateObjectManager
	{
    public:
        PipelineStateObjectManager();
        ~PipelineStateObjectManager();

        void Initialize(ID3D12Device* device);

        void CreatePipelineState(const char* name,
            const std::wstring& shaderPath,
            D3D12_INPUT_LAYOUT_DESC inputLayout);

        PipelineStateConfig* Get(const char* name);

    private:
        void CreateRootSignature(const char* name);
        ID3DBlob* CompileShader(const std::wstring& path, const char* target);

    private:
        ID3D12Device* m_pDevice;
        ID3D12RootSignature* m_pRootSignature;
        std::unordered_map<std::string, PipelineStateConfig*> m_pipelineStates;
	};

}

#endif // !PIPELINE_STATE_OBJECT_MANAGER__H
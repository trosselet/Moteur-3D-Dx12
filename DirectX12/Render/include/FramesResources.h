#ifndef FRAMES_RESOURCES__H
#define FRAMES_RESOURCES__H

namespace Render
{
	class FramesResources
	{
    public:
        FramesResources();
        ~FramesResources();
        void Initialize(ID3D12Device* pDevice);
        void Reset();

        ID3D12CommandAllocator* GetAllocator() const { return m_pCommandAllocator; }
        ID3D12GraphicsCommandList* GetCommandList() const { return m_pCommandList; }

        UINT64 fenceValue = 0;

    private:
        ID3D12Device* m_pDevice;
        ID3D12CommandAllocator* m_pCommandAllocator;
        ID3D12GraphicsCommandList* m_pCommandList;
	};
}


#endif // !FRAMES_RESOURCES__H
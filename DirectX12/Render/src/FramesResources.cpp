#include "pch.h"
#include "FramesResources.h"

Render::FramesResources::FramesResources() : m_pDevice(nullptr), m_pCommandAllocator(nullptr), m_pCommandList(nullptr)
{
}

Render::FramesResources::~FramesResources()
{
	if (m_pCommandList)
	{
		m_pCommandList->Release();
		m_pCommandList = nullptr;
	}

	if (m_pCommandAllocator)
	{
		m_pCommandAllocator->Release();
		m_pCommandAllocator = nullptr;
	}
}

void Render::FramesResources::Initialize(ID3D12Device* pDevice)
{
	if (pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_pCommandAllocator)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating command allocator, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
	}

	if (pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, nullptr, IID_PPV_ARGS(&m_pCommandList)) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT("[RENDER]: Error creating command list, at line: " << __LINE__ << ", At file: " << __FILE__ << "\n");
	}

	m_pCommandList->Close();
}

void Render::FramesResources::Reset()
{
	m_pCommandAllocator->Reset();
	m_pCommandList->Reset(m_pCommandAllocator, nullptr);
}

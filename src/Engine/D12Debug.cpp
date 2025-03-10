#include "pch.h"
#include "D12Debug.h"


namespace Engine
{
	D12Debug D12Debug::mInstance;


	void D12Debug::Enable()
	{
		GetInterface();

		Get()->EnableDebugLayer();
	}

	void D12Debug::GetInterface()
	{
		if (!Get())
		{
			if (D3D12GetDebugInterface(IID_PPV_ARGS(GetAddressOf())) != S_OK)
			{
				PRINT_CONSOLE_OUTPUT("Error creating the debug interface" << std::endl);
			}
		}
	}
}
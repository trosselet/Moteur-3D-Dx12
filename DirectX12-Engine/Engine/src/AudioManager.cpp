#include "AudioManager.h"
#include "DebugMacro.h"

#include <windows.h>
#include <combaseapi.h>

namespace Engine
{
	AudioManager& AudioManager::GetInstance()
	{
		static AudioManager instance;
		return instance;
	}

	bool AudioManager::Initialize()
	{
		std::call_once(m_initFlag, [this]() 
		{
			if (CoInitializeEx(NULL, COINITBASE_MULTITHREADED) != S_OK)
			{
				PRINT_CONSOLE_OUTPUT("[MUSIC] CoInitialize failed.\n");
				return;
			}

			if (FAILED(XAudio2Create(&m_pXAudio2, 0)))
			{
				PRINT_CONSOLE_OUTPUT("[MUSIC] XAudio2Create failed.\n");
				return;
			}

			if (FAILED(m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice)))
			{
				PRINT_CONSOLE_OUTPUT("[MUSIC] CreateMasteringVoice failed.\n");
				return;
			}

			DWORD channelMask = 0;
			m_pMasterVoice->GetChannelMask(&channelMask);

			BYTE x3dInstance[X3DAUDIO_HANDLE_BYTESIZE] = {};
			if (X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, x3dInstance) != S_OK)
			{
				PRINT_CONSOLE_OUTPUT("[MUSIC] X3DAudio init failed\n");
				return;
			}

			memcpy(&m_x3dHandle, x3dInstance, sizeof(X3DAUDIO_HANDLE));
			
			m_x3dInitialized = true;
			m_initialized = true;
		});

		return m_initialized;
	}

	void AudioManager::Shutdown()
	{
		if (m_pMasterVoice)
		{
			m_pMasterVoice->DestroyVoice();
			m_pMasterVoice = nullptr;
		}

		if (m_pXAudio2)
		{
			m_pXAudio2->Release();
			m_pXAudio2 = nullptr;
		}

		CoUninitialize();
		m_initialized = false;
	}
}

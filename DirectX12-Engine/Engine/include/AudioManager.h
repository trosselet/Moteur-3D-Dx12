#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <xaudio2.h>
#include <x3daudio.h>
#pragma comment(lib, "xaudio2.lib")

#include <mutex>

namespace Engine
{
	class AudioManager
	{
	public:
		static AudioManager& GetInstance();

		bool Initialize();
		void Shutdown();

		IXAudio2* GetXAudio2() const { return m_pXAudio2; }
		IXAudio2MasteringVoice* GetMasterVoice() const { return m_pMasterVoice; }

		const X3DAUDIO_HANDLE& GetX3DAudioHandle() const { return m_x3dHandle; }
		bool IsX3DAudioInitialized() const { return m_x3dInitialized; }

	private:
		AudioManager() = default;
		~AudioManager() = default;

		AudioManager(const AudioManager&) = delete;
		AudioManager& operator=(const AudioManager&) = delete;

		IXAudio2* m_pXAudio2 = nullptr;
		IXAudio2MasteringVoice* m_pMasterVoice = nullptr;

		X3DAUDIO_HANDLE m_x3dHandle = {};
		bool m_x3dInitialized = false;

		std::once_flag m_initFlag;
		bool m_initialized = false;
	};
}

#endif // AUDIOMANAGER_H

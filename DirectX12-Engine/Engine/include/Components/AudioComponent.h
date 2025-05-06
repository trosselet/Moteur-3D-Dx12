#ifndef AUDIOCOMPONENT__H
#define AUDIOCOMPONENT__H

#include "ComponentBase.h"
#include "Define.h"
#include "Vector3.hpp"

#include <xaudio2.h>
#include <x3daudio.h>
#pragma comment(lib, "xaudio2.lib")


namespace Engine
{
	struct AudioComponent : public ComponentBase<Component::Audio>
	{

	private:
		struct AudioCallback : public IXAudio2VoiceCallback
		{
			bool finished = false;

			void STDMETHODCALLTYPE OnStreamEnd() override { finished = true; }
			void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
			void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
			void STDMETHODCALLTYPE OnBufferEnd(void*) override {}
			void STDMETHODCALLTYPE OnBufferStart(void*) override {}
			void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
			void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}
		};

		struct AudioData
		{
			WAVEFORMATEX format = {};
			BYTE* pBuffer = nullptr;
			DWORD bufferSize = 0;
			IXAudio2SourceVoice* pSourceVoice = nullptr;
			AudioCallback callback;
		};

	public:

		void Play(const char* audioPath);
		
		void SetVolume(float32 volume);

		void SetStereo(bool enabled) { m_stereoEnabled = enabled; }

		void Set3DPosition(Vector3f pos);

		AudioData m_audioData = {};
		bool m_playRequested = false;
		bool m_isPlaying = false;

		IXAudio2* m_pXAudio2 = nullptr;
		IXAudio2MasteringVoice* m_pMasterVoice = nullptr;


	private:
		~AudioComponent();
		void Release();
		bool Load(const char* audioPath);
		bool m_stereoEnabled = false;
          
		X3DAUDIO_LISTENER m_listener = {};         
		X3DAUDIO_EMITTER m_emitter = {};                  
		Vector3f m_position = Vector3f{ 0, 0, 0 };          


		friend class Scene;
		friend class AudioSystem;
	};
}


#endif // !AUDIOCOMPONENT__H
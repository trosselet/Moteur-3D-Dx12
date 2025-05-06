#include "Systems/AudioSystem.h"

#include "DebugMacro.h"
#include "Components/AudioComponent.h"

#include "GameManager.h"
#include "GameObject.h"
#include "AudioManager.h"

void Engine::AudioSystem::UpdateAudio()
{
	for (AudioComponent* pAudioComponent : m_audioComponents)
	{
		if (pAudioComponent->m_playRequested && !pAudioComponent->m_isPlaying)
		{
			PlayAudio(pAudioComponent);
			pAudioComponent->m_isPlaying = true;
			pAudioComponent->m_playRequested = false;
		}


		if (pAudioComponent->m_isPlaying && pAudioComponent->m_stereoEnabled)
		{
			pAudioComponent->m_listener.Position = GameManager::GetActiveScene().GetMainCamera()->m_pTransform->GetPositionFLOAT();
			pAudioComponent->m_listener.OrientFront = GameManager::GetActiveScene().GetMainCamera()->m_pTransform->Forward();
			pAudioComponent->m_listener.OrientTop = GameManager::GetActiveScene().GetMainCamera()->m_pTransform->Up();

			pAudioComponent->m_emitter.Position = pAudioComponent->GetOwner().m_pTransform->GetPositionFLOAT();
			
			X3DAUDIO_DISTANCE_CURVE_POINT curvePoints[] =
			{
				{ 0.0f, 1.0f  },
				{ 10.0f, 0.8f },
				{ 25.0f, 0.3f },
				{ 50.0f, 0.0f },
			};

			X3DAUDIO_DISTANCE_CURVE curve = {};
			curve.pPoints = curvePoints;
			curve.PointCount = ARRAYSIZE(curvePoints);

			pAudioComponent->m_emitter.pVolumeCurve = &curve;
			pAudioComponent->m_emitter.CurveDistanceScaler = 1.0f;
			pAudioComponent->m_emitter.ChannelCount = 1;
			pAudioComponent->m_emitter.OrientFront = pAudioComponent->GetOwner().m_pTransform->Forward();
			pAudioComponent->m_emitter.OrientTop = pAudioComponent->GetOwner().m_pTransform->Up();

			float matrix[8] = {};
			float delayTimes[2] = {};
			X3DAUDIO_DSP_SETTINGS dspSettings = {};
			dspSettings.SrcChannelCount = pAudioComponent->m_audioData.format.nChannels;
			dspSettings.DstChannelCount = 2;
			dspSettings.pMatrixCoefficients = matrix;
			

			DWORD flags = X3DAUDIO_CALCULATE_MATRIX;
			if (dspSettings.SrcChannelCount == 1)
			{
				dspSettings.pDelayTimes = delayTimes;
				flags |= X3DAUDIO_CALCULATE_DELAY;
			}

			X3DAudioCalculate(
				AudioManager::GetInstance().GetX3DAudioHandle(),
				&pAudioComponent->m_listener,
				&pAudioComponent->m_emitter,
				flags,
				&dspSettings);

			pAudioComponent->m_audioData.pSourceVoice->SetOutputMatrix(
				pAudioComponent->m_pMasterVoice,
				dspSettings.SrcChannelCount,
				dspSettings.DstChannelCount,
				matrix);

			X3DAUDIO_VECTOR delta = 
			{
				pAudioComponent->m_listener.Position.x - pAudioComponent->m_emitter.Position.x,
				pAudioComponent->m_listener.Position.y - pAudioComponent->m_emitter.Position.y,
				pAudioComponent->m_listener.Position.z - pAudioComponent->m_emitter.Position.z,
			};

			float distanceSquared = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

			PRINT_CONSOLE_OUTPUT("[X3D] PosListener: ("
				<< pAudioComponent->m_listener.Position.x << ", "
				<< pAudioComponent->m_listener.Position.y << ", "
				<< pAudioComponent->m_listener.Position.z << ") | PosEmitter: ("
				<< pAudioComponent->m_emitter.Position.x << ", "
				<< pAudioComponent->m_emitter.Position.y << ", "
				<< pAudioComponent->m_emitter.Position.z << ") | Distance²: "
				<< distanceSquared << " | L=" << matrix[0] << ", R=" << matrix[1] << "\n");
		}


		if (pAudioComponent->m_isPlaying && pAudioComponent->m_audioData.callback.finished)
		{
			PRINT_CONSOLE_OUTPUT("[MUSIC] SOUND FINISHED !!!!!!!!!!!!!\n");

			pAudioComponent->m_isPlaying = false;
			
			if (pAudioComponent->m_audioData.pSourceVoice)
			{
				pAudioComponent->m_audioData.pSourceVoice->DestroyVoice();
				pAudioComponent->m_audioData.pSourceVoice = nullptr;
			}
		}
	}

	if (m_audioComponents.size() <= 0)
	{
		PRINT_CONSOLE_OUTPUT("[MUSIC] NO MUSIC COMP !!!!!!!!! \n");
	}
}

void Engine::AudioSystem::PlayAudio(AudioComponent* component)
{
	IXAudio2SourceVoice* pSourceVoice = nullptr;

	if (component->m_pXAudio2->CreateSourceVoice(&component->m_audioData.pSourceVoice, &component->m_audioData.format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &component->m_audioData.callback) != S_OK)
	{
		PRINT_CONSOLE_OUTPUT("[MUSIC] Error creating source voice");
		return;
	}

	component->m_audioData.callback.finished = false;

	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = component->m_audioData.bufferSize;
	buffer.pAudioData = component->m_audioData.pBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	component->m_audioData.pSourceVoice->SubmitSourceBuffer(&buffer);

	if (component->m_stereoEnabled)
	{
		component->m_listener.Position = GameManager::GetActiveScene().GetMainCamera()->m_pTransform->GetPositionFLOAT();
		component->m_listener.OrientFront = GameManager::GetActiveScene().GetMainCamera()->m_pTransform->Forward();
		component->m_listener.OrientTop = GameManager::GetActiveScene().GetMainCamera()->m_pTransform->Up();

		component->m_emitter.Position = component->GetOwner().m_pTransform->GetPositionFLOAT();

		component->m_emitter.ChannelCount = 1;
		component->m_emitter.CurveDistanceScaler = 1.0f;
		component->m_emitter.OrientFront = component->GetOwner().m_pTransform->Forward();
		component->m_emitter.OrientTop = component->GetOwner().m_pTransform->Up();

		float matrix[8] = {};
		float delayTimes[2] = {};
		X3DAUDIO_DSP_SETTINGS dspSettings = {};
		dspSettings.SrcChannelCount = component->m_audioData.format.nChannels;
		dspSettings.DstChannelCount = 2;
		dspSettings.pMatrixCoefficients = matrix;
		

		DWORD flags = X3DAUDIO_CALCULATE_MATRIX;
		if (dspSettings.SrcChannelCount == 1)
		{
			dspSettings.pDelayTimes = delayTimes;
			flags |= X3DAUDIO_CALCULATE_DELAY;
		}

		X3DAudioCalculate(
			AudioManager::GetInstance().GetX3DAudioHandle(),
			&component->m_listener,
			&component->m_emitter,
			flags,
			&dspSettings);


		component->m_audioData.pSourceVoice->SetOutputMatrix(
			component->m_pMasterVoice,
			dspSettings.SrcChannelCount,
			dspSettings.DstChannelCount,
			matrix
		);

	}
	else
	{
		float matrix[2] = { 1.0f, 1.0f };
		component->m_audioData.pSourceVoice->SetOutputMatrix(component->m_pMasterVoice, 1, 2, matrix);
	}

	component->m_audioData.pSourceVoice->Start(0);

}

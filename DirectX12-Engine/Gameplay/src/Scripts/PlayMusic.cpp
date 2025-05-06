#include "Scripts/PlayMusic.h"


PlayMusic::PlayMusic(const char* audioPath)
{
	m_pAudioPath = audioPath;
	m_pAudioComponent = nullptr;
	volume = 10;
}

PlayMusic::~PlayMusic()
{
}

void PlayMusic::OnStart()
{
	m_pAudioComponent = &m_pOwner->GetComponent<Engine::AudioComponent>();
}

void PlayMusic::OnUpdate()
{
	m_pAudioComponent->SetVolume(volume);
}

void PlayMusic::OnFixedUpdate()
{
	if (GetAsyncKeyState('M') < 0)
	{
		m_pAudioComponent->Play(m_pAudioPath);
	}

	if (GetAsyncKeyState('P') < 0)
	{
		volume += 0.1f;
	}

	if (GetAsyncKeyState('L') < 0)
	{
		volume -= 0.1f;
	}
}

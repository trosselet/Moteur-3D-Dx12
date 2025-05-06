#ifndef PLAYMUSIC__H
#define PLAYMUSIC__H

#include "IScript.h"

#include "Components/AudioComponent.h"

class PlayMusic : public Engine::IScript
{
public:
	PlayMusic(const char* audioPath);
	~PlayMusic();

	void OnStart() override;
	void OnUpdate() override;
	void OnFixedUpdate() override;

private:
	const char* m_pAudioPath;
	Engine::AudioComponent* m_pAudioComponent;
	float volume;
};

#endif // !PLAYMUSIC__H
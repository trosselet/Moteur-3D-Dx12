#ifndef AUDIOSYSTEM__H
#define AUDIOSYSTEM__H

#include <vector>

namespace Engine { struct AudioComponent; };

namespace Engine
{
	class AudioSystem
	{
	public:
		AudioSystem() = default;
		~AudioSystem() = default;


		void UpdateAudio();

		std::vector<AudioComponent*> m_audioComponents = {};

	private:

		void PlayAudio(AudioComponent* component);

		friend class Scene;
		friend class GameManager;
	};
}


#endif // !AUDIOSYSTEM__H
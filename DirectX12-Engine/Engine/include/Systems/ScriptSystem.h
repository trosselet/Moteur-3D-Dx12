#ifndef SCRIPTSYSTEM__H
#define SCRIPTSYSTEM__H

#include <unordered_map>
#include <vector>

namespace Engine
{
	class IScript;

	class ScriptSystem
	{
	public:
		ScriptSystem() = default;
		~ScriptSystem();

		void OnUpdate();
		void OnFixedUpdate();


		std::unordered_map<unsigned int, std::vector<IScript*>> m_scriptsByEntity;

	private:
		friend class GameManager;
	};
}


#endif // !SCRIPTSYSTEM__H
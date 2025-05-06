#include "Systems/ScriptSystem.h"
#include "Define.h"
#include "IScript.h"

Engine::ScriptSystem::~ScriptSystem()
{
	m_scriptsByEntity.clear();
}

void Engine::ScriptSystem::OnUpdate()
{
	for (auto& [entityID, scripts] : m_scriptsByEntity)
	{
		for (IScript* script : scripts)
		{
			script->OnUpdate();
		}
	}
}

void Engine::ScriptSystem::OnFixedUpdate()
{
	for (auto& [entityID, scripts] : m_scriptsByEntity)
	{
		for (IScript* script : scripts)
		{
			script->OnFixedUpdate();
		}
	}
}

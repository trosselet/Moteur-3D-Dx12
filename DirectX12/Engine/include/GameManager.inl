#ifndef GAMEMANAGER__INL
#define GAMEMANAGER__INL

namespace Engine
{
	inline void GameManager::Init(HINSTANCE const hInstance) 
	{ 
		assert((m_pInstance == nullptr));
		m_pInstance = new GameManager(hInstance); 
	}

	inline void GameManager::Run() { m_pInstance->GameLoop(); }
	
	inline void GameManager::Release() 
	{	
		assert((m_pInstance != nullptr)); 
		delete m_pInstance; 
	}

	inline std::vector<Scene>& GameManager::GetScenes() { return m_pInstance->m_scenes; }
	inline Scene& GameManager::GetActiveScene() { return *m_pInstance->m_pActiveScene; }

	inline float32& GameManager::GetFixedDeltaTime() { return m_pInstance->m_fixedDeltaTime; }
}

#endif // !GAMEMANAGER__INL

#include "GameManager.h"
#include "GameObject.h"
#include "Scene.h"
#include "Color.h"

using Engine::GameManager;
using Engine::GameObject;
using Engine::Scene;

int main()
{
	GameManager::Init(nullptr);

	Scene& scene = Scene::Create();

	scene.Load();
	scene.SetActive();

	GameManager::Run();
	GameManager::Release();

	return 0;
}
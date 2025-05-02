

#include "Components/Camera.h"
#include "Components/MeshRenderer.h"

#include "GameObject.h"
#include "Scene.h"

#include "GameManager.h"

int main()
{
	Engine::GameManager::Init(nullptr);

	Engine::Scene& scene = Engine::Scene::Create();

	scene.Load();
	scene.SetActive();

	Engine::GameObject* const pMainCamera = new Engine::GameObject(scene);
	pMainCamera->m_pTransform->SetPosition({ 0.0f, 0.0f, 0.0f });
	Engine::Camera& cameraComponent = pMainCamera->AddComponent<Engine::Camera>();

	Engine::GameObject* const pTest = new Engine::GameObject(scene);
	pTest->m_pTransform->SetPosition({ 0.0f, 0.0f, 10.0f });
	Engine::MeshRenderer& meshRendererTest = pTest->AddComponent<Engine::MeshRenderer>();
	meshRendererTest.SetCircle("../Gameplay/texture/grid_placeholder_material.dds");



	Engine::GameManager::Run();
	Engine::GameManager::Release();

	return 0;
}
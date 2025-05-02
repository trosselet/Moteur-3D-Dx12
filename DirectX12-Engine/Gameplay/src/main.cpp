

#include "Components/Camera.h"
#include "Components/MeshRenderer.h"

#include "GameObject.h"
#include "Scene.h"

#include "GameManager.h"


using Engine::Camera;
using Engine::MeshRenderer;

using Engine::GameObject;
using Engine::Scene;

using Engine::GameManager;

int main()
{
	GameManager::Init(nullptr);

	Scene& scene = Scene::Create();

	scene.Load();
	scene.SetActive();

	GameObject* const pMainCamera = new GameObject(scene);
	pMainCamera->m_pTransform->SetPosition({ 0.0f, 0.0f, 0.0f });
	Camera& cameraComponent = pMainCamera->AddComponent<Camera>();

	GameObject* const pTest = new GameObject(scene);
	pTest->m_pTransform->SetPosition({ 0.0f, 0.0f, 10.0f });
	MeshRenderer& meshRendererTest = pTest->AddComponent<MeshRenderer>();
	meshRendererTest.SetCircle("../Gameplay/texture/grid_placeholder_material.dds");



	GameManager::Run();
	GameManager::Release();

	return 0;
}
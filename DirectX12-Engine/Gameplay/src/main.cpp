#include "Components/Camera.h"
#include "Components/MeshRenderer.h"

#include "GameObject.h"
#include "Scene.h"

#include "GameManager.h"

#include "Scripts/CameraMovement.h"

int main()
{
	Engine::GameManager::Init(nullptr);

	Engine::Scene& scene = Engine::Scene::Create();

	scene.Load();
	scene.SetActive();

	Engine::GameObject* const pMainCamera = new Engine::GameObject(scene);
	pMainCamera->m_pTransform->SetPosition({ 0.0f, 0.0f, 0.0f });
	Engine::Camera& cameraComponent = pMainCamera->AddComponent<Engine::Camera>();
	pMainCamera->SetName("Camera");
	pMainCamera->AddScript<CameraMovement>();

	Engine::GameObject* const pCircle = new Engine::GameObject(scene);
	pCircle->m_pTransform->SetPosition({ 0.0f, 0.0f, 5.0f });
	Engine::MeshRenderer& pCircleMeshRendererTest = pCircle->AddComponent<Engine::MeshRenderer>();
	pCircleMeshRendererTest.SetCircle("../Gameplay/texture/grid_placeholder_material.dds");
	pCircle->SetName("Circle");

	Engine::GameObject* const pRectangle = new Engine::GameObject(scene);
	pRectangle->m_pTransform->SetPosition({ 1.0f, 0.0f, 5.0f });
	Engine::MeshRenderer& pRectangleMeshRendererTest = pRectangle->AddComponent<Engine::MeshRenderer>();
	pRectangleMeshRendererTest.SetRectangle("../Gameplay/texture/grid_placeholder_material.dds");
	pRectangle->SetName("Rectangle");

	Engine::GameObject* const pCube = new Engine::GameObject(scene);
	pCube->m_pTransform->SetPosition({ 2.0f, 0.0f, 5.0f });
	Engine::MeshRenderer& pCubeMeshRendererTest = pCube->AddComponent<Engine::MeshRenderer>();
	pCubeMeshRendererTest.SetCube("../Gameplay/texture/grid_placeholder_material.dds");
	pCube->SetName("Cube");

	Engine::GameObject* const pSphere = new Engine::GameObject(scene);
	pSphere->m_pTransform->SetPosition({ -1.0f, 0.0f, 5.0f });
	pSphere->m_pTransform->Scale({ -1000.0f, -1000.0f, -1000.0f });
	Engine::MeshRenderer& pSphereMeshRendererTest = pSphere->AddComponent<Engine::MeshRenderer>();
	pSphereMeshRendererTest.SetSphere("../Gameplay/texture/sky.dds");
	pSphere->SetName("Sphere");



	Engine::GameManager::Run();
	Engine::GameManager::Release();

	return 0;
}
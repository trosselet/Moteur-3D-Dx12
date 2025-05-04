#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/Light.h"

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

	Engine::GameObject* const pBasePlateform = new Engine::GameObject(scene);
	pBasePlateform->m_pTransform->SetPosition({ 0.0f, -1.0f, 5.0f });
	pBasePlateform->m_pTransform->Scale({ 5.0f, 0.5f, 5.0f });
	Engine::MeshRenderer& pBasePlateformMeshRendererTest = pBasePlateform->AddComponent<Engine::MeshRenderer>();
	pBasePlateformMeshRendererTest.SetCube("grid_placeholder_material.dds");
	pBasePlateform->SetName("BasePlateform");

	Engine::GameObject* const pCircle = new Engine::GameObject(scene);
	pCircle->m_pTransform->SetPosition({ 0.0f, 0.0f, 5.0f });
	Engine::MeshRenderer& pCircleMeshRendererTest = pCircle->AddComponent<Engine::MeshRenderer>();
	pCircleMeshRendererTest.SetCircle("grid_placeholder_material.dds");
	pCircle->SetName("Circle");

	Engine::GameObject* const pRectangle = new Engine::GameObject(scene);
	pRectangle->m_pTransform->SetPosition({ 1.0f, 0.0f, 5.0f });
	Engine::MeshRenderer& pRectangleMeshRendererTest = pRectangle->AddComponent<Engine::MeshRenderer>();
	pRectangleMeshRendererTest.SetRectangle("grid_placeholder_material.dds");
	pRectangle->SetName("Rectangle");

	Engine::GameObject* const pCube = new Engine::GameObject(scene);
	pCube->m_pTransform->SetPosition({ -1.0f, 0.0f, 5.0f });
	Engine::MeshRenderer& pCubeMeshRendererTest = pCube->AddComponent<Engine::MeshRenderer>();
	pCubeMeshRendererTest.SetCube("grid_placeholder_material.dds");
	pCube->SetName("Cube");

	Engine::GameObject* const pSphere = new Engine::GameObject(scene);
	pSphere->m_pTransform->SetPosition({ -1.0f, 0.0f, 5.0f });
	pSphere->m_pTransform->Scale({ -1000.0f, -1000.0f, -1000.0f });
	Engine::MeshRenderer& pSphereMeshRendererTest = pSphere->AddComponent<Engine::MeshRenderer>();
	pSphereMeshRendererTest.SetSphere("sky.dds");
	pSphere->SetName("Sphere");

	Engine::GameObject* const pLight = new Engine::GameObject(scene);
	pLight->m_pTransform->SetPosition({ 0, 3.0f, 5.0f });
	pLight->m_pTransform->Scale({ 0.5f, 0.5f, 0.5f });
	Engine::Light& pLightLightComponent = pLight->AddComponent<Engine::Light>();

	/*pLightLightComponent.SetType(0);
	pLightLightComponent.SetColor(Vector3f({ 1.0f, 0.0f, 0.0f }));
	pLightLightComponent.SetIntensity(2.0f);
	pLightLightComponent.SetDirection(Vector3f({ -1.0f,-1.0f,-1.0f }));
	pLightLightComponent.SetSpotAngle(0.0f);*/

	/*pLightLightComponent.SetType(1);
	pLightLightComponent.SetColor(Vector3f({ 1.0f, 1.0f, 1.0f }));
	pLightLightComponent.SetPosition(Vector3f({ 0.0f, 3.0f, 5.0f }));
	pLightLightComponent.SetIntensity(20.0f);
	pLightLightComponent.SetDirection(Vector3f({ 0.0f, -1.0f, 0.0f }));
	pLightLightComponent.SetSpotAngle(-90.0f);*/

	pLightLightComponent.SetType(2);
	pLightLightComponent.SetColor(Vector3f({ 1.0f, 1.0f, 1.0f }));
	pLightLightComponent.SetPosition(Vector3f({ 0.0f, 3.0f, 5.0f }));
	pLightLightComponent.SetIntensity(25.0f);


	Engine::GameManager::Run();
	Engine::GameManager::Release();

	return 0;
}
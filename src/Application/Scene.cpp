#include "pch.h"
#include "Scene.h"
#include <random>


Scene::~Scene()
{
	for (auto& missile : mMissiles)
	{
		auto it = std::find(mpGraphicEngine->Get()->mObjects.begin(), mpGraphicEngine->Get()->mObjects.end(), missile.mesh);
		if (it != mpGraphicEngine->Get()->mObjects.end()) mpGraphicEngine->Get()->mObjects.erase(it);

		delete missile.mesh;
	}
	mMissiles.clear();
}

bool Scene::Initialize()
{
	srand(time(NULL));

	mCameraComponent = mpGraphicEngine->Get()->GetCamera();
	mInput = mpGraphicEngine->Get()->GetInput();
	mTransformComponent = mpGraphicEngine->Get()->GetCameraTransform();
	mScreenWidth = mpGraphicEngine->Get()->GetWidth();
	mScreenHeight = mpGraphicEngine->Get()->GetHeight();


	mpGraphicEngine->Get()->CreateCube(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, L"../../src/Engine/Texture/bricks.dds", nullptr);
	mpGraphicEngine->Get()->CreateCube(1.0f, 2.0f, 2.0f, 4.0f, 0.0f, 0.0f, L"../../src/Engine/Texture/wood.dds", nullptr);
	//mpGraphicEngine->Get()->CreateMeshFromFile(L"../../res/Engine/statue.obj", 1.0f, 1.0f, 1.0f, -20.0f, 0.0f, 0.0f, L"../../src/Engine/Texture/wood.dds",nullptr);
	//mpGraphicEngine->Get()->CreateMeshFromFile(L"../../res/Engine/sphere.obj", 1.0f, 1.0f, 1.0f, 6.0f, 0.0f, 0.0f, L"../../src/Engine/Texture/sky.dds", nullptr);
	mpGraphicEngine->Get()->CreateMeshFromFile(L"../../res/Engine/asteroid.obj", 0.5f, 0.5f, 0.5f, 0.0f, 20.0f, -2.0f, L"../../src/Engine/Texture/asteroid.dds", nullptr);
	//mpGraphicEngine->Get()->CreateMeshFromFile(L"../../res/Engine/spaceship.obj", 0.1f, 0.1f, 0.1f, 0.0f, 0.0f, -5.0f, L"../../src/Engine/Texture/bricks.dds", nullptr);
	mpGraphicEngine->Get()->CreateMeshFromFile(L"../../res/Engine/suzanne.obj", 0.5f, 0.5f, 0.5f, -2.0f, -0.0f, -2.0f, L"../../src/Engine/Texture/sky.dds", nullptr);
	mpGraphicEngine->Get()->CreateMeshFromFile(L"../../res/Engine/suzanne.obj", 0.5f, 0.5f, 0.5f, 0.0f, -0.0f, -8.0f, L"../../src/Engine/Texture/asteroid.dds", nullptr);

	int asteroidCount = 100;

	for (int i = 0; i < asteroidCount; i++)
	{
		float x = rand() % 100 + 1;
		float y = rand() % 100 + 1;
		float z = rand() % 100 - 1;
		float size = rand() % 5 + 1;

		mpGraphicEngine->Get()->CreateMeshFromFile(L"../../res/Engine/asteroid.obj", size / 10, size / 10, size / 10, x - 50, y - 50, z - 50, L"../../src/Engine/Texture/asteroid.dds", nullptr);
	}


	skyBox = mpGraphicEngine->Get()->CreateSphere(200.0f, 50, 50, 0.0f, 3.0f, 0.0f, L"../../src/Engine/Texture/stars_map.dds", true, false, nullptr);
	player = mpGraphicEngine->Get()->CreateSphere(1.0f, 50, 50, 0.0f, 3.0f, 0.0f, L"../../src/Engine/Texture/stars_map.dds", false, false, nullptr);

	mpGraphicEngine->Get()->CreateUI({ 0, 0 }, { mScreenWidth, mScreenHeight }, L"../../src/Engine/Texture/menu_ui.dds", (int)GameState::Menu);
	mpGraphicEngine->Get()->CreateUI({ 0, 0 }, { mScreenWidth, mScreenHeight }, L"../../src/Engine/Texture/pause_ui.dds", (int)GameState::Pause);
	mpGraphicEngine->Get()->CreateUI({ 0, 0 }, { mScreenWidth, mScreenHeight }, L"../../src/Engine/Texture/game_ui.dds", (int)GameState::Game);
	mpGraphicEngine->Get()->SetActiveUI((int)GameState::Menu);

	mpGraphicEngine->Get()->CreatePostProcess(0.0f, 1.0f, 0.0f, 0.0f, 1);
	mpGraphicEngine->Get()->CreatePostProcess(-0.1f, 1.0f, 1.0f, 0.0f, 2);
	mpGraphicEngine->Get()->SetActivePostProcessing(2);

    return true;
}

void Scene::Update(float deltaTime)
{
	if (mState == GameState::Menu)
	{
		if (mInput->CompareKeyState('S', mInput->PUSH))
		{

			mpGraphicEngine->Get()->RemoveActiveUI((int)mState);
			mpGraphicEngine->Get()->SetActiveUI((int)GameState::Game);
			mpGraphicEngine->Get()->SetActivePostProcessing(2);
			SetState(GameState::Game);
		}
	}

	if (mState == GameState::Pause)
	{
		if (mInput->CompareKeyState('R', mInput->PUSH))
		{
			mpGraphicEngine->Get()->RemoveActiveUI((int)mState);
			mpGraphicEngine->Get()->SetActiveUI((int)GameState::Game);
			mpGraphicEngine->Get()->SetActivePostProcessing(2);
			SetState(GameState::Game);
		}
	}

	if (mState == GameState::Game)
	{
		//SkyBox on camera
		skyBox->localMatrix = DirectX::XMMatrixTranslation(mTransformComponent->vPosition.x, mTransformComponent->vPosition.y, mTransformComponent->vPosition.z);
		player->localMatrix = skyBox->localMatrix;

		//SlowMotion
		if (mInput->CompareKeyState(VK_LCONTROL, mInput->DOWN))
		{
			// 20 %
			mTimeSpeed = 0.2f;
		}
		else
		{
			mTimeSpeed = 1.0f;
		}
		//DeltaTime with slowMotion
		float loopDeltaTime = deltaTime * mTimeSpeed;

		//Speed boost
		if (mInput->CompareKeyState(VK_SHIFT, mInput->DOWN) && mInput->CompareKeyState('Z', mInput->DOWN))
		{
			mFov = Utils::Get()->Lerp(mFov, mSpeedFov, 5.0f * loopDeltaTime);
			mSpeed = Utils::Get()->Lerp(mSpeed, 0.2f, 5.0f * loopDeltaTime);
		}
		else
		{
			mFov = Utils::Get()->Lerp(mFov, mBaseFov, 5.0f * loopDeltaTime);
			mSpeed = Utils::Get()->Lerp(mSpeed, 0.05f, 5.0f * loopDeltaTime);
		}
		mCameraComponent->SetProjection(mFov);
		mCameraComponent->SetSpeed(mSpeed);

		mDamageTime -= loopDeltaTime;

		if (mDamageTime <= 0)
		{
			canTakeDamage = true;
			canRegen = true;
			mDamageTime = 0;
		}

		//Shooting projectile
		if (mInput->CompareKeyState(VK_LBUTTON, mInput->PUSH))
		{
			DirectX::XMFLOAT4 camPos = mTransformComponent->vPosition;
			DirectX::XMFLOAT3 camDir = mTransformComponent->vDirection;

			float spawnDistance = 1.0f;
			float heightOffset = 0;
			float posX = camPos.x + camDir.x * spawnDistance;
			float posY = camPos.y + camDir.y * spawnDistance + heightOffset;
			float posZ = camPos.z + camDir.z * spawnDistance;

			GraphicEngine::Mesh* missileMesh = mpGraphicEngine->Get()->CreateSphere(0.1f, 6, 6, posX, posY, posZ,
				L"../../src/Engine/Texture/wood.dds",
				false,
				true,
				nullptr);

			Missile newMissile = { { posX, posY, posZ }, DirectX::XMFLOAT3(camDir.x, camDir.y + heightOffset, camDir.z), 60.0f, 5.0f, missileMesh };
			mMissiles.push_back(newMissile);
		}

		//Update Projectile
		for (size_t i = 0; i < mMissiles.size(); )
		{
			Missile& missile = mMissiles[i];

			if (missile.mesh)
			{
				missile.position.x += missile.direction.x * missile.speed * loopDeltaTime;
				missile.position.y += missile.direction.y * missile.speed * loopDeltaTime;
				missile.position.z += missile.direction.z * missile.speed * loopDeltaTime;
				missile.mesh->localMatrix = DirectX::XMMatrixTranslation(missile.position.x, missile.position.y, missile.position.z);

				missile.mesh->boundingBox.center = missile.position;
			}

			missile.lifeTime -= loopDeltaTime;

			bool collided = false;
			if (missile.mesh)
			{
				for (GraphicEngine::Mesh* object : mpGraphicEngine->Get()->mObjects)
				{
					if (object != missile.mesh && missile.mesh->boundingBox.Intersects(object->boundingBox) && object != skyBox && object != player)
					{
						mpGraphicEngine->Get()->Delete(missile.mesh);
						mpGraphicEngine->Get()->Delete(object);
						missile.mesh = nullptr;
						collided = true;
						break;
					}
				}
			}

			if (missile.lifeTime <= 0.0f)
			{
				if (missile.mesh)
				{
					auto it = std::find(mpGraphicEngine->Get()->mObjects.begin(), mpGraphicEngine->Get()->mObjects.end(), missile.mesh);
					if (it != mpGraphicEngine->Get()->mObjects.end())
					{
						mpGraphicEngine->Get()->mObjects.erase(it);
					}

					delete missile.mesh;
					missile.mesh = nullptr;
				}

				mMissiles.erase(mMissiles.begin() + i);
			}
			else
			{
				i++;
			}
		}
#ifdef _DEBUG
		if (mInput->CompareKeyState(VK_F1, mInput->PUSH))
		{
			mUIActive = !mUIActive;

			if (mUIActive)
			{
				mpGraphicEngine->Get()->SetActiveUI(1);
			}
			else
			{
				mpGraphicEngine->Get()->RemoveActiveUI(1);
			}
		}

		if (mInput->CompareKeyState(VK_F2, mInput->PUSH))
		{
			mPlayerHealth -= 25;
			mDamageTime = 3.0f;
		}
#endif

		if (canRegen)
		{
			mPlayerHealth = Utils::Get()->Lerp(mPlayerHealth, mMaxPlayerHealth, (0.1f + 3.9f * ((mMaxPlayerHealth - mPlayerHealth) / mMaxPlayerHealth)) * loopDeltaTime);
		}

		float damageIntensity = 1.0f - (mPlayerHealth / mMaxPlayerHealth);

		if (mSwitchLight)
		{
			mLight = Utils::Get()->Lerp(mLight, mMaxLight, 0.9 * loopDeltaTime);
			if (mLight >= mMaxLight - 0.01)
			{
				mSwitchLight = false;
			}
		}
		else
		{
			mLight = Utils::Get()->Lerp(mLight, mBaseLight, 0.9 * loopDeltaTime);

			if (mLight <= mBaseLight + 0.01)
			{
				mSwitchLight = true;
			}
		}

		mpGraphicEngine->Get()->ModifyPostProcess(2, mLight, 1.0f, 1.0f, damageIntensity);

		if (mPlayerHealth <= 0)
		{
			mpGraphicEngine->Get()->RemoveActivePostProcessing(2);
			mpGraphicEngine->Get()->SetActivePostProcessing(1);
			mPlayerHealth = 0;
		}

		if (mInput->CompareKeyState(VK_ESCAPE, mInput->PUSH))
		{
			mpGraphicEngine->Get()->SetActiveUI((int)GameState::Pause);
			mpGraphicEngine->Get()->RemoveActiveUI((int)mState);
			mpGraphicEngine->Get()->RemoveActivePostProcessing(2);
			SetState(GameState::Pause);
		}
	}
}

void Scene::FixedUpdate(float deltaTime)
{

	if (mState == GameState::Menu)
	{
		
	}

	if (mState == GameState::Pause)
	{
		
	}

	if (mState == GameState::Game)
	{
		for (GraphicEngine::Mesh* object : mpGraphicEngine->Get()->mObjects)
		{
			if (object != skyBox && object != player && player->boundingBox.Intersects(object->boundingBox))
			{
				if (canTakeDamage)
				{
					mPlayerHealth -= 25;
					mDamageTime = 1.0f;
					canTakeDamage = false;
					canRegen = false;
				}
			}
		}
	}
}

bool Scene::SetState(GameState state)
{
	int curretState = static_cast<int>(mState);
	int nextState = static_cast<int>(state);

	if (mTransition[curretState][nextState] == 0)
	{
		return false;
	}

	mState = state;
	return true;
}

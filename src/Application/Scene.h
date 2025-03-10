#pragma once

#include "Engine/GraphicEngine.h"
#include "Engine/ECSManager.h"
#include "Engine/Component.h"

class Scene
{
public:
	Scene() = default;
	~Scene();

	bool Initialize();
	void Update(float deltaTime);
	void FixedUpdate(float deltaTime);

private:
	GraphicEngine* mpGraphicEngine;
	ECSManager* mECSManager;
	int mCamera;
	CameraComponent* mCameraComponent;
	TransformComponent* mTransformComponent;
	InputControlerComponent* mInput;

	struct Missile
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 direction;
		float speed;
		float lifeTime;
		GraphicEngine::Mesh* mesh;
	};

	enum class GameState
	{
		Menu,
		Game,
		Pause,

		Count
	};

	static const int stateCount = (int)(GameState::Count);

private:
	GameState mState = GameState::Menu;

	int mTransition[stateCount][stateCount] =
	{	//Menu	//Game	//Pause
		{0,		1,		0}, //Menu
		{0,		0,		1},	//Game
		{0,		1,		0},	//Pause
	};

	bool SetState(GameState state);

private:

	float mScreenWidth;
	float mScreenHeight;

	float mFov = 65.0f;
	float mBaseFov = mFov;
	float mSpeedFov = 110.0f;
	float mSpeed;

	float mTimeSpeed;

	float mLight = -0.09;
	float mBaseLight = mLight;
	float mMaxLight = -0.05f;
	bool mSwitchLight = false;

	bool mUIActive = true;

	float mMaxPlayerHealth = 100;
	float mPlayerHealth = mMaxPlayerHealth;
	float mDamageTime = 3;

	bool canTakeDamage = false;
	bool canRegen = false;

	std::vector<Missile> mMissiles;

private:
	GraphicEngine::Mesh* skyBox;
	GraphicEngine::Mesh* player;


};


#pragma once
#include "pch.h"
#include <vector>
#include "ECSManager.h"
#include "System.h"
#include "Component.h"

class CameraSystem : public System
{
public:
	CameraSystem() = default;
	virtual ~CameraSystem() override = default;

	void Initialize();
	void InitializeWithComponent();
	void OnUpdate(std::vector<Component*>);

	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;

	void SetProjection(float fov);

	DirectX::XMFLOAT3 GetForwardVector();
	DirectX::XMFLOAT4 GetPosition() const;

	void SetSpeed(float speed);

private:
	TransformComponent* tr;
	InputControlerComponent* input;
	CameraComponent* cam;

	float mAngleY = 0;
	float mAngleZ = 0;

	bool mViewDirty = true;
	bool firstUpdate;
	float mSpeed;
	float sensitivity = 0.02f;


};
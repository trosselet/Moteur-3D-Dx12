#include "pch.h"
#include "CameraSystem.h"

void CameraSystem::Initialize()
{
	mNeededComponent = { 1, 4, 6 };

	firstUpdate = true;

	mViewDirty = true;

	mSpeed = 0.05f;

}

void CameraSystem::InitializeWithComponent()
{
	tr->vPosition = { 10.0f, 0.0f, -10.0f, 1.0f };
	tr->UpdateMatrix();
}

void CameraSystem::OnUpdate(std::vector<Component*> components)
{
	tr = dynamic_cast<TransformComponent*>(components[0]);
	input = dynamic_cast<InputControlerComponent*>(components[1]);
	cam = dynamic_cast<CameraComponent*>(components[2]);

	if (firstUpdate == true) {
		InitializeWithComponent();
		firstUpdate = false;
	}

	if (input->CompareKeyState(VK_RIGHT, input->DOWN)) mAngleZ += cam->mSpeed;
	if (input->CompareKeyState(VK_LEFT, input->DOWN)) mAngleZ -= cam->mSpeed;
	if (input->CompareKeyState(VK_DOWN, input->DOWN)) mAngleY += cam->mSpeed;
	if (input->CompareKeyState(VK_UP, input->DOWN)) mAngleY -= cam->mSpeed;
	if (input->deltaX != 0 || input->deltaY != 0)
	{
		mAngleZ += input->deltaX * sensitivity;
		mAngleY += input->deltaY * sensitivity;
	}

	tr->RotateWorld(0, mAngleY, mAngleZ);

	if (input->CompareKeyState('Z', input->DOWN))tr->Walk(cam->mSpeed);
	if (input->CompareKeyState('S', input->DOWN))tr->Walk(-cam->mSpeed);
	if (input->CompareKeyState('D', input->DOWN))tr->Strafe(cam->mSpeed);
	if (input->CompareKeyState('Q', input->DOWN))tr->Strafe(-cam->mSpeed);

	if (input->CompareKeyState(0x41, input->DOWN))tr->Elevate(cam->mSpeed);
	if (input->CompareKeyState(0x45, input->DOWN)) tr->Elevate(-cam->mSpeed);


	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookToLH(
		DirectX::XMLoadFloat4(&tr->vPosition),
		DirectX::XMLoadFloat3(&tr->vDirection),
		DirectX::XMLoadFloat3(&tr->vUp)
	);
	DirectX::XMStoreFloat4x4(&cam->mView, viewMatrix);
}

DirectX::XMMATRIX CameraSystem::GetViewMatrix() const
{
	return DirectX::XMLoadFloat4x4(&cam->mView);
}


DirectX::XMMATRIX CameraSystem::GetProjectionMatrix() const
{
	return DirectX::XMLoadFloat4x4(&cam->mProj);
}

void CameraSystem::SetProjection(float fov)
{
	DirectX::XMStoreFloat4x4(&cam->mProj, DirectX::XMMatrixPerspectiveFovLH(fov * (DirectX::XM_PI / 180.0f), 16.0f / 9.0f, 0.1f, 250.0f));
}


DirectX::XMFLOAT3 CameraSystem::GetForwardVector()
{
	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&tr->matrix);
	DirectX::XMVECTOR forward = world.r[2];
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, forward);
	return result;
}


DirectX::XMFLOAT4 CameraSystem::GetPosition() const
{
	DirectX::XMFLOAT4 position;
	position.x = tr->matrix._41;
	position.y = tr->matrix._42;
	position.z = tr->matrix._43;
	position.w = 1.0f;

	return position;
}

void CameraSystem::SetSpeed(float speed)
{
	cam->mSpeed = speed;
}

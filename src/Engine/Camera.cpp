#include "pch.h"
#include "Camera.h"

Camera::~Camera()
{
}

void Camera::Initialize()
{
	mRight = { 1.0f, 0.0f, 0.0f };
	mUp = { 0.0f, 1.0f, 0.0f };
	mLook = { 0.0f, 0.0f, 1.0f };

    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
    world = DirectX::XMMatrixTranslation(0, 0, -10);
    DirectX::XMStoreFloat4x4(&mWorldMatrix, world);


	mSpeed = 0.05f;
	mViewDirty = true;

	SetProjection(45.0f);
}

void Camera::Update()
{
    if (GetAsyncKeyState(0x5A) & 0x8000) Walk(mSpeed);
    if (GetAsyncKeyState(0x53) & 0x8000) Walk(-mSpeed);
    if (GetAsyncKeyState(0x44) & 0x8000) Strafe(mSpeed);
    if (GetAsyncKeyState(0x51) & 0x8000) Strafe(-mSpeed);

    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) RotateY(mSpeed);
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) RotateY(-mSpeed);
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) RotateX(mSpeed);
    if (GetAsyncKeyState(VK_UP) & 0x8000) RotateX(-mSpeed);
}




DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorldMatrix);
    world = DirectX::XMMatrixInverse(nullptr, world);
    return world;
}


DirectX::XMMATRIX Camera::GetProjectionMatrix() const
{
	return DirectX::XMLoadFloat4x4(&mProj);
}

void Camera::AttachToObject(GraphicEngine::Mesh* target)
{
    if (!target) return;

    DirectX::XMMATRIX objectWorldMatrix = target->worldMatrix;

    DirectX::XMStoreFloat4x4(&mWorldMatrix, objectWorldMatrix);

    mViewDirty = true;
}

void Camera::SetProjection(float fov)
{
	DirectX::XMStoreFloat4x4(&mProj, DirectX::XMMatrixPerspectiveFovLH(fov * (DirectX::XM_PI / 180.0f), 16.0f/9.0f, 0.1f, 250.0f));
}

DirectX::XMFLOAT3 Camera::GetForwardVector()
{
    DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorldMatrix);
    DirectX::XMVECTOR forward = world.r[2];
    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, forward);
    return result;
}


DirectX::XMFLOAT4 Camera::GetPosition() const
{
	DirectX::XMFLOAT4 position;
	position.x = mWorldMatrix._41;
	position.y = mWorldMatrix._42;
	position.z = mWorldMatrix._43;
	position.w = 1.0f;

	return position;
}


void Camera::SetSpeed(float speed)
{
	mSpeed = speed;
}

void Camera::Strafe(float d)
{
    DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorldMatrix);

    DirectX::XMVECTOR right = world.r[0];
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
    DirectX::XMVECTOR position = world.r[3];

    position = DirectX::XMVectorMultiplyAdd(s, right, position);

    world.r[3] = position;
    DirectX::XMStoreFloat4x4(&mWorldMatrix, world);
}

void Camera::Walk(float d)
{
    DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorldMatrix);

    DirectX::XMVECTOR look = world.r[2];
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(d);
    DirectX::XMVECTOR position = world.r[3];

    position = DirectX::XMVectorMultiplyAdd(s, look, position);

    world.r[3] = position;
    DirectX::XMStoreFloat4x4(&mWorldMatrix, world);
}

void Camera::RotateY(float angle)
{
    DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorldMatrix);

    DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angle);

    world.r[0] = DirectX::XMVector3TransformNormal(world.r[0], R);
    world.r[1] = DirectX::XMVector3TransformNormal(world.r[1], R);
    world.r[2] = DirectX::XMVector3TransformNormal(world.r[2], R);

    DirectX::XMStoreFloat4x4(&mWorldMatrix, world);
}

void Camera::RotateX(float angle)
{
    DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorldMatrix);

    DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(world.r[0], angle);

    world.r[1] = DirectX::XMVector3TransformNormal(world.r[1], R);
    world.r[2] = DirectX::XMVector3TransformNormal(world.r[2], R);

    DirectX::XMStoreFloat4x4(&mWorldMatrix, world);
}

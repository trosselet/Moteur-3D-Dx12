#include "pch.h"
#include "Component.h"
#include "ECSManager.h"

//--------------------------------------------------------------------- Component ---------------------------------------------------------------------


Component::Component()
{
}

void Component::Initialize()
{
    entity = 0;
    ID = 0;
}


//--------------------------------------------------------------------- TransformComponent ---------------------------------------------------------------------

TransformComponent::TransformComponent() 
{
}

void TransformComponent::Initialize()
{
    ID = 1;

    vScale = { 1.0f, 1.0f, 1.0f };
    vPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

    vDirection = { 0.0f, 0.0f, 1.0f };
    vRight = { 1.0f, 0.0f, 0.0f };
    vUp = { 0.0f, 1.0f, 0.0f };
    qRotation = { 0.0f, 0.0f, 0.0f, 1.0f };
    Identity();

    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
    world = DirectX::XMMatrixTranslation(0, 0, -10);
    DirectX::XMStoreFloat4x4(&matrix, world);


}

void TransformComponent::Identity()
{
    DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&mRot, DirectX::XMMatrixIdentity());
}

void TransformComponent::UpdateMatrix() {
    DirectX::XMMATRIX tmpMatrix = DirectX::XMMatrixIdentity();

    DirectX::XMMATRIX mmRot = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&qRotation));

    DirectX::XMMATRIX mSca = DirectX::XMMatrixScaling(vScale.x, vScale.y, vScale.z);
    DirectX::XMMATRIX mPos = DirectX::XMMatrixTranslation(vPosition.x, vPosition.y, vPosition.z);

    tmpMatrix = DirectX::XMMatrixMultiply(mmRot, mSca);
    tmpMatrix = DirectX::XMMatrixMultiply(tmpMatrix, mPos);

    DirectX::XMStoreFloat4x4(&matrix, tmpMatrix);
}

void TransformComponent::Rotate(float roll, float pitch, float yaw)
{

    DirectX::XMVECTOR qCurrentRotation = DirectX::XMLoadFloat4(&qRotation);

    DirectX::XMVECTOR qRotX = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&vRight), pitch);
    DirectX::XMVECTOR qRotY = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&vUp), yaw);
    DirectX::XMVECTOR qRotZ = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&vDirection), roll);

    qCurrentRotation = DirectX::XMQuaternionMultiply(qRotX, qCurrentRotation);
    qCurrentRotation = DirectX::XMQuaternionMultiply(qRotY, qCurrentRotation);
    qCurrentRotation = DirectX::XMQuaternionMultiply(qRotZ, qCurrentRotation);
    qCurrentRotation = DirectX::XMQuaternionNormalize(qCurrentRotation);

    DirectX::XMStoreFloat4(&qRotation, qCurrentRotation);

    UpdateMatrix();
}   

void TransformComponent::UpdateRotationFromMatrix() {
    vRight = { mRot._11, mRot._12, mRot._13 };
    vUp = { mRot._21, mRot._22, mRot._23 };
    vDirection = { mRot._31, mRot._32, mRot._33 };
}

void TransformComponent::RotateWorld(float roll, float pitch, float yaw) {

    float rollRad = roll * (DirectX::XM_PI / 180.0f);
    float pitchRad = pitch * (DirectX::XM_PI / 180.0f);
    float yawRad = yaw * (DirectX::XM_PI / 180.0f);

    DirectX::XMVECTOR qRotRPY = DirectX::XMQuaternionRotationRollPitchYaw(pitchRad, yawRad, rollRad);

    DirectX::XMStoreFloat4(&qRotation, qRotRPY);
    DirectX::XMStoreFloat4x4(&mRot, DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&qRotation)));

    UpdateRotationFromMatrix();
}

void TransformComponent::Move(float x, float y, float z)
{
    vPosition.x += x;
    vPosition.y += y;
    vPosition.z += z;
    UpdateMatrix();
}

void TransformComponent::Walk(float distance)
{
    DirectX::XMVECTOR direction = DirectX::XMLoadFloat3(&vDirection);
    DirectX::XMVECTOR movement = DirectX::XMVectorScale(direction, distance);

    DirectX::XMVECTOR position = DirectX::XMLoadFloat4(&vPosition);
    position = DirectX::XMVectorAdd(position, movement);

    DirectX::XMStoreFloat4(&vPosition, position);

    UpdateMatrix();
}

void TransformComponent::Strafe(float distance)
{
    DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&vRight);
    DirectX::XMVECTOR movement = DirectX::XMVectorScale(right, distance);

    DirectX::XMVECTOR position = DirectX::XMLoadFloat4(&vPosition);
    position = DirectX::XMVectorAdd(position, movement);

    DirectX::XMStoreFloat4(&vPosition, position);

    UpdateMatrix();
}

void TransformComponent::Elevate(float distance)
{
    DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&vUp);
    DirectX::XMVECTOR movement = DirectX::XMVectorScale(up, distance);

    DirectX::XMVECTOR position = DirectX::XMLoadFloat4(&vPosition);
    position = DirectX::XMVectorAdd(position, movement);

    DirectX::XMStoreFloat4(&vPosition, position);

    UpdateMatrix();
}


void TransformComponent::SetScale(float x, float y, float z)
{
    vScale = { x, y, z };
    UpdateMatrix();
}



//--------------------------------------------------------------------- MeshComponent ---------------------------------------------------------------------

MeshComponent::MeshComponent()
{
}

void MeshComponent::Initialize()
{
    ID = 2;
}


//--------------------------------------------------------------------- StatsComponent ---------------------------------------------------------------------

StatsComponent::StatsComponent()
{
}

void StatsComponent::Initialize()
{
    ID = 3;
}


//--------------------------------------------------------------------- InputControlerComponent ---------------------------------------------------------------------

InputControlerComponent::InputControlerComponent()
{
}

void InputControlerComponent::Initialize()
{
    ID = 4;
}

InputControlerComponent::KeyState InputControlerComponent::GetKeyState(int virtualKey)
{
    auto it = mKeyStates.find(virtualKey);
    return (it != mKeyStates.end()) ? it->second : KeyState::NONE;
}

bool InputControlerComponent::CompareKeyState(int virtualKey, KeyState wantedState)
{
    if (wantedState == GetKeyState(virtualKey)) return true;
    return false;
}

void InputControlerComponent::UpdateMousePosition(int x, int y, POINT center)
{
    deltaX = x - center.x;
    deltaY = y - center.y;

}


//--------------------------------------------------------------------- BehaviourComponent ---------------------------------------------------------------------

BehaviourComponent::BehaviourComponent()
{
}

void BehaviourComponent::Initialize()
{
    ID = 5;
}


//--------------------------------------------------------------------- CameraComponent ---------------------------------------------------------------------

CameraComponent::CameraComponent()
{
}

void CameraComponent::Initialize()
{
    DirectX::XMMATRIX temp = DirectX::XMMatrixIdentity();
    DirectX::XMStoreFloat4x4(&mView, temp);
    ID = 6;
}

DirectX::XMMATRIX CameraComponent::GetViewMatrix() const
{

    return DirectX::XMLoadFloat4x4(&mView);

}

DirectX::XMMATRIX CameraComponent::GetProjectionMatrix() const
{
    return DirectX::XMLoadFloat4x4(&mProj);
}

void CameraComponent::SetProjection(float fov)
{
    DirectX::XMStoreFloat4x4(&mProj, DirectX::XMMatrixPerspectiveFovLH(fov * (DirectX::XM_PI / 180.0f), 16.0f / 9.0f, 0.1f, 250.0f));
}

void CameraComponent::SetSpeed(float speed)
{
    mSpeed = speed;
}

//--------------------------------------------------------------------- struct transform ---------------------------------------------------------------------

   
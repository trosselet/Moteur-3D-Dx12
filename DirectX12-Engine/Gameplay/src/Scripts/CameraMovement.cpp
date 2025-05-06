#include "Scripts/CameraMovement.h"

CameraMovement::CameraMovement()
{
	cameraSpeed = 10;
}

CameraMovement::~CameraMovement()
{
}

void CameraMovement::OnStart()
{
}

void CameraMovement::OnUpdate()
{
    bool Z = GetAsyncKeyState('Z') < 0;
    bool Q = GetAsyncKeyState('Q') < 0;
    bool S = GetAsyncKeyState('S') < 0;
    bool D = GetAsyncKeyState('D') < 0;
    bool A = GetAsyncKeyState('A') < 0;
    bool E = GetAsyncKeyState('E') < 0;

    bool LEFT = GetAsyncKeyState(VK_LEFT) < 0;
    bool RIGHT = GetAsyncKeyState(VK_RIGHT) < 0;

    Vector3f forward = m_pOwner->m_pTransform->Forward();
    Vector3f right = m_pOwner->m_pTransform->Right();
    Vector3f up = m_pOwner->m_pTransform->Up();

    if (Z)
    {
        m_pOwner->m_pTransform->OffsetPosition(Vector3f{ forward.x * 0.05f, forward.y * 0.05f, forward.z * 0.05f }.ToXMFLOAT3());
    }
    if (Q)
    {
        m_pOwner->m_pTransform->OffsetPosition(Vector3f{ -right.x * 0.05f, -right.y * 0.05f, -right.z * 0.05f }.ToXMFLOAT3());
    }
    if (S)
    {
        m_pOwner->m_pTransform->OffsetPosition(Vector3f{ -forward.x * 0.05f, -forward.y * 0.05f, -forward.z * 0.05f }.ToXMFLOAT3());
    }
    if (D)
    {
        m_pOwner->m_pTransform->OffsetPosition(Vector3f{ right.x * 0.05f, right.y * 0.05f, right.z * 0.05f }.ToXMFLOAT3());
    }
    if (A)
    {
        m_pOwner->m_pTransform->OffsetPosition(Vector3f{ 0, -up.y * 0.05f, 0 }.ToXMFLOAT3());
    }
    if (E)
    {
        m_pOwner->m_pTransform->OffsetPosition(Vector3f{ 0, up.y * 0.05f, 0 }.ToXMFLOAT3());
    }

    if (LEFT)
    {
        m_pOwner->m_pTransform->RotateYPR(Vector3f{ -0.01f, 0.0f, 0.0f }.ToXMFLOAT3());
    }

    if (RIGHT)
    {
        m_pOwner->m_pTransform->RotateYPR(Vector3f{ 0.01f, 0.0f, 0.0f }.ToXMFLOAT3());
    }
}

void CameraMovement::OnFixedUpdate()
{
}

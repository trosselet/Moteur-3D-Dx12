#include "Components/Light.h"

void Engine::Light::SetColor(Vector3f color)
{
	m_lightCb.color = color.ToXMFLOAT3();
}

void Engine::Light::SetType(int32 type)
{
	m_lightCb.type = type;
}

void Engine::Light::SetPosition(Vector3f position)
{
	m_lightCb.Position = position.ToXMFLOAT3();
}

void Engine::Light::SetIntensity(float32 intensity)
{
	m_lightCb.intensity = intensity;
}

void Engine::Light::SetDirection(Vector3f direction)
{
	m_lightCb.Direction = direction.ToXMFLOAT3();
}

void Engine::Light::SetSpotAngle(float32 angle)
{
	m_lightCb.spotLightAngle = angle * (DirectX::XM_PI / 180);
}

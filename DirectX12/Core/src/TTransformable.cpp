#include "TTransformable.h"

Core::Transformable::Transformable() : m_transform()
{
	m_transform.Reset();
}

Core::Transformable::~Transformable()
{
}

void Core::Transformable::SetPosition(Vector3f pos)
{
	DirectX::FXMVECTOR _vec = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	m_transform.SetPosition(_vec);
}

void Core::Transformable::SetRotationX(float32 angle)
{
	DirectX::FXMVECTOR _ypr = DirectX::XMVectorSet(angle, m_transform.GetRotationFLOAT().y, m_transform.GetRotationFLOAT().z, m_transform.GetRotationFLOAT().w);
	m_transform.SetRotationYPR(_ypr);
}

void Core::Transformable::SetRotationY(float32 angle)
{
	DirectX::FXMVECTOR _ypr = DirectX::XMVectorSet(m_transform.GetRotationFLOAT().x, angle, m_transform.GetRotationFLOAT().z, m_transform.GetRotationFLOAT().w);
	m_transform.SetRotationYPR(_ypr);
}

void Core::Transformable::SetRotationZ(float32 angle)
{
	DirectX::FXMVECTOR _ypr = DirectX::XMVectorSet(m_transform.GetRotationFLOAT().x, m_transform.GetRotationFLOAT().y, angle, m_transform.GetRotationFLOAT().w);
	m_transform.SetRotationYPR(_ypr);
}

void Core::Transformable::SetScale(Vector3f scale)
{
	DirectX::FXMVECTOR _vec = DirectX::XMVectorSet(scale.x, scale.y, scale.z, 1.0f);
	m_transform.SetScaling(_vec);
}

void Core::Transformable::Translate(Vector3f offset)
{
	m_transform.OffsetPosition(DirectX::XMFLOAT3(offset.x, offset.y, offset.z));
}

void Core::Transformable::RotateX(float32 angle)
{
	m_transform.RotateYPR(DirectX::XMFLOAT3(angle, 0, 0));
}

void Core::Transformable::RotateY(float32 angle)
{
	m_transform.RotateYPR(DirectX::XMFLOAT3(0, angle, 0));
}

void Core::Transformable::RotateZ(float32 angle)
{
	m_transform.RotateYPR(DirectX::XMFLOAT3(0, 0, angle));
}

void Core::Transformable::Scale(Vector3f offset)
{
	m_transform.Scale(DirectX::XMFLOAT3(offset.x, offset.y, offset.z));
}

Vector3f Core::Transformable::GetPosition()
{
	Vector3f position;
	position.x = m_transform.GetPositionFLOAT().x;
	position.y = m_transform.GetPositionFLOAT().y;
	position.z = m_transform.GetPositionFLOAT().z;
	return position;
}

Vector3f Core::Transformable::GetScale()
{
	Vector3f scale;
	scale.x = m_transform.GetScalingFLOAT().x;
	scale.y = m_transform.GetScalingFLOAT().y;
	scale.z = m_transform.GetScalingFLOAT().z;
	return scale;
}

Vector3f Core::Transformable::Forward()
{
	return Vector3f(m_transform.mvForward.x, m_transform.mvForward.y, m_transform.mvForward.z);
}

Vector3f Core::Transformable::Right()
{
	return Vector3f(m_transform.mvRight.x, m_transform.mvRight.y, m_transform.mvRight.z);
}

Vector3f Core::Transformable::Up()
{
	return Vector3f(m_transform.mvUp.x, m_transform.mvUp.y, m_transform.mvUp.z);
}

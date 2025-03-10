#pragma once
#include "GraphicEngine.h"

class Camera
{
public:
	Camera() = default;
	~Camera();

	void Initialize();
	void Update();
	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;
	void AttachToObject(GraphicEngine::Mesh* target);


	void SetProjection(float fov);

	DirectX::XMFLOAT3 GetForwardVector();
	DirectX::XMFLOAT4 GetPosition() const;

	void SetSpeed(float speed);


	void Strafe(float d);
	void Walk(float d);

	void RotateY(float angle);
	void RotateX(float angle);

private:
	DirectX::XMFLOAT3 mRight;
	DirectX::XMFLOAT3 mUp;
	DirectX::XMFLOAT3 mLook;

	bool mViewDirty = true;
	DirectX::XMFLOAT4X4 mWorldMatrix;
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;

	float mSpeed;

};
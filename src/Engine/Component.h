#pragma once
#include "pch.h"

class ECSManager;

class Entity;


//--------------------------------------------------------------------- Component ---------------------------------------------------------------------

class Component
{
public:
    Component();
    virtual ~Component() = default;
    virtual void Initialize();

public:
    ECSManager::EntityID entity;
    int ID;
    int GetID() { return ID; };
};

//--------------------------------------------------------------------- TransformComponent ---------------------------------------------------------------------

class TransformComponent : public Component
{
public:
    TransformComponent();
    virtual ~TransformComponent() = default;

    void Initialize() override;

    void Identity();
    void UpdateMatrix();
    void UpdateRotationFromMatrix();

    void Rotate(float roll, float pitch, float yaw);
    void RotateWorld(float roll, float pitch, float yaw);

    void Move(float x, float y, float z);
    void SetScale(float x, float y, float z);

    void Strafe(float d);
    void Walk(float d);
    void Elevate(float d);

public:

    DirectX::XMFLOAT3 vScale;

    DirectX::XMFLOAT3 vDirection;
    DirectX::XMFLOAT3 vRight;
    DirectX::XMFLOAT3 vUp;

    DirectX::XMFLOAT4 vPosition;

    DirectX::XMFLOAT4 qRotation;
    DirectX::XMFLOAT4X4 mRot;

    DirectX::XMFLOAT4X4 matrix;

};


//--------------------------------------------------------------------- MeshComponent ---------------------------------------------------------------------

class MeshComponent : public Component
{
public:
    MeshComponent();
    virtual ~MeshComponent() = default;

    void Initialize() override;


public:

    const char* mShape;
    DirectX::XMFLOAT4 color;
    //const char* texture;
};


//--------------------------------------------------------------------- StatsComponent ---------------------------------------------------------------------

class StatsComponent : public Component
{
public:
    StatsComponent();
    virtual ~StatsComponent() = default;

    void Initialize() override;

public:

    float mHP;
    float mSpeed;
    float mFireingDelay;
    float mReloadTime;
    float mDamage;
};


//--------------------------------------------------------------------- InputControlerComponent ---------------------------------------------------------------------

class InputControlerComponent : public Component
{
public:
    InputControlerComponent();
    virtual ~InputControlerComponent() = default;

    void Initialize() override;

    enum KeyState {
        NONE,  // Pas de changement
        PUSH,  // Touche pressée pour la première fois
        DOWN,  // Touche maintenue
        UP     // Touche relâchée
    };

    KeyState GetKeyState(int virtualKey);
    bool CompareKeyState(int virtualKey, KeyState wantedState);

    std::unordered_map<int, KeyState> mKeyStates;
    std::unordered_map<int, bool> mPreviousKeyStates;

    int deltaX = 0;
    int deltaY = 0;

    void UpdateMousePosition(int x, int y, POINT center);

public:
};


//--------------------------------------------------------------------- BehaviourComponent ---------------------------------------------------------------------

class BehaviourComponent : public Component
{
public:
    BehaviourComponent();
    virtual ~BehaviourComponent() = default;

    void Initialize() override;

public:

    int mEntityBehavior;
};


//--------------------------------------------------------------------- CameraComponent ---------------------------------------------------------------------

class CameraComponent : public Component
{
public:
    CameraComponent();
    virtual ~CameraComponent() = default;

    void Initialize() override;

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;

    void SetProjection(float fov);
    void SetSpeed(float speed);


public:

    float mSpeed;
    float fov;
    DirectX::XMFLOAT4X4 mView;
    DirectX::XMFLOAT4X4 mProj;
};




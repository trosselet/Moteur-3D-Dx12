#pragma once

#include "pch.h"
#include "ECSManager.h"
#include "System.h"

class InputSystem : public System
{
public:
    InputSystem();
    virtual ~InputSystem() override;

    void Initialize();

    void OnUpdate(std::vector<Component*> components);  // Mise à jour des états des touches

    void ConfineCursor();

    void ResetCursorPosition(POINT center);

    void ToggleFPSMode();

private:

    bool IsKeyDown(int virtualKey);
    bool isFPSMode = true;
};
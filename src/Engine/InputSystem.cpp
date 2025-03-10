#include "pch.h"
#include "InputSystem.h"

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
}

void InputSystem::Initialize()
{
    mNeededComponent = { 4 };

    ShowCursor(FALSE);
}

bool InputSystem::IsKeyDown(int virtualKey)
{
    return (GetAsyncKeyState(virtualKey) & 0x8000) != 0;
}

void InputSystem::OnUpdate(std::vector<Component*> components)
{
    InputControlerComponent* inputComponent = dynamic_cast<InputControlerComponent*>(components[0]);

    for (int i = 0; i < 256; ++i) {
        bool keyDown = IsKeyDown(i);

        if (keyDown) {
            if (!inputComponent->mPreviousKeyStates[i]) {
                inputComponent->mKeyStates[i] = inputComponent->PUSH;
            }
            else {
                inputComponent->mKeyStates[i] = inputComponent->DOWN;
            }
        }
        else {
            if (inputComponent->mPreviousKeyStates[i]) {
                inputComponent->mKeyStates[i] = inputComponent->UP;
            }
            else {
                inputComponent->mKeyStates[i] = inputComponent->NONE;
            }
        }

        inputComponent->mPreviousKeyStates[i] = keyDown;
    }

    if (inputComponent->CompareKeyState(VK_ESCAPE, inputComponent->PUSH)) {
        ToggleFPSMode();
    }

    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(GetActiveWindow(), &mousePos);

    if (isFPSMode) {
        RECT rect;
        GetClientRect(GetActiveWindow(), &rect);

        POINT center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };

        ConfineCursor();
        inputComponent->UpdateMousePosition(mousePos.x, mousePos.y, center);
        ResetCursorPosition(center);
    }
}


void InputSystem::ConfineCursor()
{
    RECT rect;
    GetClientRect(GetActiveWindow(), &rect);

    POINT ul = { rect.left, rect.top };
    POINT lr = { rect.right, rect.bottom };

    ClientToScreen(GetActiveWindow(), &ul);
    ClientToScreen(GetActiveWindow(), &lr);

    rect.left = ul.x;
    rect.top = ul.y;
    rect.right = lr.x;
    rect.bottom = lr.y;

    ClipCursor(&rect);
}

void InputSystem::ResetCursorPosition(POINT center)
{
    ClientToScreen(GetActiveWindow(), &center);
    SetCursorPos(center.x, center.y);
}

void InputSystem::ToggleFPSMode()
{
    isFPSMode = !isFPSMode;

    if (isFPSMode)
    {
        ShowCursor(FALSE);
        ConfineCursor();
    }
    else
    {
        ShowCursor(TRUE);
        ClipCursor(NULL);
    }
}

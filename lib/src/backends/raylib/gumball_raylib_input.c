#include <gumball/core/gumball_backend.h>
#include <gumball/gumball_events.h>
#include <gumball/core/gumball_logger.h>
#include <raylib.h>

// ---------------------------------- Mouse ---------------------------------- //

void GUM_Backend_Mouse_update(GUM_Mouse* pMouse) {
    Vector2 position_ = GetMousePosition();
    Vector2 delta_    = GetMouseDelta();

    pMouse->position.x = position_.x;
    pMouse->position.y = position_.y;
    pMouse->delta.x    = delta_.x;
    pMouse->delta.y    = delta_.y;

    GUM_INPUTDEVICE(pMouse)->buttons = 0;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))   GUM_INPUTDEVICE(pMouse)->buttons |= GUM_MOUSE_BUTTON_LEFT;
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))  GUM_INPUTDEVICE(pMouse)->buttons |= GUM_MOUSE_BUTTON_RIGHT;
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) GUM_INPUTDEVICE(pMouse)->buttons |= GUM_MOUSE_BUTTON_MIDDLE;
}

bool GUM_Backend_Gamepad_isConnected(int index) {
    return IsGamepadAvailable(index);
}

void GUM_Backend_Gamepad_update(GUM_Gamepad* pGamepad) {
    int index = pGamepad->rawIndex;

    GUM_INPUTDEVICE(pGamepad)->buttons = 0;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))  GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_A;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_B;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))  GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_X;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_FACE_UP))    GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_Y;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_TRIGGER_1))   GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_LB;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_RIGHT_TRIGGER_1))  GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_RB;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_MIDDLE_RIGHT))     GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_START;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_MIDDLE_LEFT))      GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_SELECT;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_FACE_UP))     GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_DPAD_UP;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_FACE_DOWN))   GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_DPAD_DOWN;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_FACE_LEFT))   GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_DPAD_LEFT;
    if (IsGamepadButtonDown(index, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))  GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_DPAD_RIGHT;
}

const char* GUM_Backend_Gamepad_name(int index) {
    return GetGamepadName(index);
}
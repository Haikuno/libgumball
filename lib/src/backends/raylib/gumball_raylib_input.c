#include <gumball/core/gumball_backend.h>
#include <gumball/gumball_events.h>
#include <gumball/core/gumball_logger.h>
#include <raylib.h>

// ---------------------------------- Mouse ---------------------------------- //

void GUM_Backend_Mouse_update(GUM_Mouse* pMouse) {
    Vector2 position_ = GetMousePosition();
    Vector2 delta_    = GetMouseDelta();
    Vector2 wheel_    = GetMouseWheelMoveV();

    pMouse->position = (GUM_Vector2){position_.x, position_.y};
    pMouse->delta    = (GUM_Vector2){delta_.x, delta_.y};
    pMouse->wheel    = (GUM_Vector2){wheel_.x, wheel_.y};

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

// ---------------------------------- Keyboard ---------------------------------- //

void GUM_Backend_Keyboard_update(GUM_Keyboard* pKeyboard) {
    GUM_INPUTDEVICE(pKeyboard)->buttons = 0;
    if (IsKeyDown(KEY_UP))     GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_UP;
    if (IsKeyDown(KEY_DOWN))   GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_DOWN;
    if (IsKeyDown(KEY_LEFT))   GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_LEFT;
    if (IsKeyDown(KEY_RIGHT))  GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_RIGHT;
    if (IsKeyDown(KEY_W))      GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_W;
    if (IsKeyDown(KEY_A))      GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_A;
    if (IsKeyDown(KEY_S))      GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_S;
    if (IsKeyDown(KEY_D))      GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_D;
    if (IsKeyDown(KEY_ENTER))  GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_ENTER;
    if (IsKeyDown(KEY_ESCAPE)) GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_ESCAPE;
}
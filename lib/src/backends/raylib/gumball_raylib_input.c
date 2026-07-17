#include <gumball/core/gumball_backend.h>
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
#include "gumball_sdl3_internal.h"
#include <gumball/backends/gumball_sdl3.h>
#include <gumball/core/gumball_backend.h>

constexpr int GUM_SDL3_MAX_GAMEPADS_ = 16;

typedef struct {
    SDL_JoystickID id;
    SDL_Gamepad*   pGamepad;
} GUM_SDL3_GamepadSlot_;

static GUM_SDL3_GamepadSlot_ gamepads_[GUM_SDL3_MAX_GAMEPADS_];
static GUM_Vector2           wheel_ = { 0 };

static bool GUM_SDL3_gamepadPresent_(const SDL_JoystickID* pIds, int count, SDL_JoystickID id) {
    for (int i = 0; i < count; ++i)
        if (pIds[i] == id) return true;

    return false;
}

void GUM_SDL3_gamepadsUpdate_(void) {
    int count = 0;
    SDL_JoystickID* pIds = SDL_GetGamepads(&count);

    if (!pIds && count)
        return;

    for (int i = 0; i < GUM_SDL3_MAX_GAMEPADS_; ++i) {
        GUM_SDL3_GamepadSlot_* pSlot = &gamepads_[i];
        if (!pSlot->pGamepad) continue;

        if (!GUM_SDL3_gamepadPresent_(pIds, count, pSlot->id)) {
            SDL_CloseGamepad(pSlot->pGamepad);
            *pSlot = (GUM_SDL3_GamepadSlot_){ 0 };
        }
    }

    for (int i = 0; i < count; ++i) {
        bool assigned = false;

        for (int slot = 0; slot < GUM_SDL3_MAX_GAMEPADS_; ++slot) {
            if (gamepads_[slot].id == pIds[i]) {
                assigned = true;
                break;
            }
        }

        if (assigned) continue;

        for (int slot = 0; slot < GUM_SDL3_MAX_GAMEPADS_; ++slot) {
            if (gamepads_[slot].pGamepad) continue;

            SDL_Gamepad* pGamepad = SDL_OpenGamepad(pIds[i]);
            if (!pGamepad) break;

            gamepads_[slot] = (GUM_SDL3_GamepadSlot_){ .id = pIds[i], .pGamepad = pGamepad };
            break;
        }
    }

    SDL_free(pIds);
}

void GUM_SDL3_gamepadsDeinit_(void) {
    for (int i = 0; i < GUM_SDL3_MAX_GAMEPADS_; ++i) {
        if (gamepads_[i].pGamepad)
            SDL_CloseGamepad(gamepads_[i].pGamepad);

        gamepads_[i] = (GUM_SDL3_GamepadSlot_){ 0 };
    }

    wheel_ = (GUM_Vector2){ 0 };
}

GBL_EXPORT void GUM_SDL3_processEvent(const SDL_Event* pEvent) {
    if (!pEvent) return;

    if (pEvent->type == SDL_EVENT_MOUSE_WHEEL) {
        float x = pEvent->wheel.x;
        float y = pEvent->wheel.y;

        if (pEvent->wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
            x = -x;
            y = -y;
        }

        wheel_.x += x;
        wheel_.y += y;
    }
}

void GUM_Backend_Mouse_update(GUM_Mouse* pMouse) {
    float x;
    float y;
    const SDL_MouseButtonFlags buttons = SDL_GetMouseState(&x, &y);
    SDL_Renderer* pRenderer = GUM_SDL3_nativeRenderer_(nullptr);

    if (pRenderer)
        SDL_RenderCoordinatesFromWindow(pRenderer, x, y, &x, &y);

    pMouse->delta    = (GUM_Vector2){ x - pMouse->position.x, y - pMouse->position.y };
    pMouse->position = (GUM_Vector2){ x, y };
    pMouse->wheel    = wheel_;
    wheel_           = (GUM_Vector2){ 0 };

    GUM_INPUTDEVICE(pMouse)->buttons = 0;
    if (buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT))   GUM_INPUTDEVICE(pMouse)->buttons |= GUM_MOUSE_BUTTON_LEFT;
    if (buttons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT))  GUM_INPUTDEVICE(pMouse)->buttons |= GUM_MOUSE_BUTTON_RIGHT;
    if (buttons & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE)) GUM_INPUTDEVICE(pMouse)->buttons |= GUM_MOUSE_BUTTON_MIDDLE;
}

bool GUM_Backend_Gamepad_isConnected(int index) {
    return index >= 0 && index < GUM_SDL3_MAX_GAMEPADS_ && gamepads_[index].pGamepad;
}

const char* GUM_Backend_Gamepad_name(int index) {
    if (!GUM_Backend_Gamepad_isConnected(index)) return "";

    const char* pName = SDL_GetGamepadName(gamepads_[index].pGamepad);
    return pName ? pName : "";
}

void GUM_Backend_Gamepad_update(GUM_Gamepad* pGamepad) {
    const int index = pGamepad->rawIndex;
    if (!GUM_Backend_Gamepad_isConnected(index)) return;

    SDL_Gamepad* pSdlGamepad = gamepads_[index].pGamepad;
    GUM_INPUTDEVICE(pGamepad)->buttons = 0;

    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_SOUTH))          GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_A;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_EAST))           GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_B;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_WEST))           GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_X;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_NORTH))          GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_Y;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER))  GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_LB;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_RB;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_START))          GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_START;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_BACK))           GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_BUTTON_SELECT;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_DPAD_UP))        GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_DPAD_UP;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN))      GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_DPAD_DOWN;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT))      GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_DPAD_LEFT;
    if (SDL_GetGamepadButton(pSdlGamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT))     GUM_INPUTDEVICE(pGamepad)->buttons |= GUM_GAMEPAD_DPAD_RIGHT;
}

void GUM_Backend_Keyboard_update(GUM_Keyboard* pKeyboard) {
    const bool* pKeys = SDL_GetKeyboardState(nullptr);

    GUM_INPUTDEVICE(pKeyboard)->buttons = 0;
    if (pKeys[SDL_SCANCODE_UP])     GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_UP;
    if (pKeys[SDL_SCANCODE_DOWN])   GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_DOWN;
    if (pKeys[SDL_SCANCODE_LEFT])   GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_LEFT;
    if (pKeys[SDL_SCANCODE_RIGHT])  GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_RIGHT;
    if (pKeys[SDL_SCANCODE_W])      GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_W;
    if (pKeys[SDL_SCANCODE_A])      GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_A;
    if (pKeys[SDL_SCANCODE_S])      GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_S;
    if (pKeys[SDL_SCANCODE_D])      GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_D;
    if (pKeys[SDL_SCANCODE_RETURN]) GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_ENTER;
    if (pKeys[SDL_SCANCODE_ESCAPE]) GUM_INPUTDEVICE(pKeyboard)->buttons |= GUM_KEYBOARD_KEY_ESCAPE;
}

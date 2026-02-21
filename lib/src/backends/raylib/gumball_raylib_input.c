#include <gumball/core/gumball_backend.h>
#include <raylib.h>

GBL_EXPORT GBL_RESULT GUM_Backend_pollInput(GUM_Controller* pController) {
    typedef struct {
        KeyboardKey              key;
        GUM_CONTROLLER_BUTTON_ID button_ui;
    } KeyBinding;

    typedef struct {
        GamepadButton            button_gamepad;
        GUM_CONTROLLER_BUTTON_ID button_ui;
    } ButtonBinding;

    static KeyBinding constexpr key_bindings[] = {
        { KEY_UP,    GUM_CONTROLLER_UP        },
        { KEY_RIGHT, GUM_CONTROLLER_RIGHT     },
        { KEY_DOWN,  GUM_CONTROLLER_DOWN      },
        { KEY_LEFT,  GUM_CONTROLLER_LEFT      },
        { KEY_A,     GUM_CONTROLLER_PRIMARY   },
        { KEY_S,     GUM_CONTROLLER_SECONDARY },
        { KEY_D,     GUM_CONTROLLER_TERTIARY  },
    };
    static size_t constexpr n_key_bindings = sizeof(key_bindings) / sizeof(key_bindings[0]);

    static ButtonBinding constexpr button_bindings[] = {
        { GAMEPAD_BUTTON_LEFT_FACE_UP,     GUM_CONTROLLER_UP        },
        { GAMEPAD_BUTTON_LEFT_FACE_RIGHT,  GUM_CONTROLLER_RIGHT     },
        { GAMEPAD_BUTTON_LEFT_FACE_DOWN,   GUM_CONTROLLER_DOWN      },
        { GAMEPAD_BUTTON_LEFT_FACE_LEFT,   GUM_CONTROLLER_LEFT      },
        { GAMEPAD_BUTTON_RIGHT_FACE_DOWN,  GUM_CONTROLLER_PRIMARY   },
        { GAMEPAD_BUTTON_RIGHT_FACE_RIGHT, GUM_CONTROLLER_SECONDARY },
        { GAMEPAD_BUTTON_RIGHT_FACE_UP,    GUM_CONTROLLER_TERTIARY  },
    };
    static size_t constexpr n_button_bindings = sizeof(button_bindings) / sizeof(button_bindings[0]);

    if (pController->isKeyboard) {
        for (size_t i = 0; i < n_key_bindings; ++i) {
            if (IsKeyPressed(key_bindings[i].key))
                GUM_Controller_sendButton(pController, GUM_CONTROLLER_BUTTON_PRESS, key_bindings[i].button_ui);
            if (IsKeyReleased(key_bindings[i].key))
                GUM_Controller_sendButton(pController, GUM_CONTROLLER_BUTTON_RELEASE, key_bindings[i].button_ui);
        }

        return GBL_RESULT_SUCCESS;
    }

    for (size_t i = 0; i < n_button_bindings; ++i) {
        if (IsGamepadButtonPressed(pController->controllerId, button_bindings[i].button_gamepad))
            GUM_Controller_sendButton(pController, GUM_CONTROLLER_BUTTON_PRESS, button_bindings[i].button_ui);
        if (IsGamepadButtonReleased(pController->controllerId, button_bindings[i].button_gamepad))
            GUM_Controller_sendButton(pController, GUM_CONTROLLER_BUTTON_RELEASE, button_bindings[i].button_ui);
    }

    return GBL_RESULT_SUCCESS;
}

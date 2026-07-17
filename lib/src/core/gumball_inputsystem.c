#include <gumball/core/gumball_inputsystem.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/gumball_elements.h>
#include <gumball/gumball_devices.h>
#include <gumball/gumball_events.h>


// TODO: this is REEST, fuck raylib
#define GUM_MAX_GAMEPADS 16

static GUM_Mouse*   pMouse_                       = nullptr;
static GUM_Gamepad* pGamepads_[GUM_MAX_GAMEPADS]  = { nullptr };
static GblArrayList bindings_[GUM_INPUTACTION_COUNT];

void GUM_InputSystem_init(void) {
    pMouse_    = GUM_Mouse_create();

    for (size_t i = 0; i < GBL_COUNT_OF(bindings_); i++)
        GblArrayList_construct(&bindings_[i], sizeof(GUM_InputBinding));

    // Default bindings
    GUM_InputSystem_bind(GUM_MOUSE_TYPE, GUM_INPUTACTION_CONFIRM, GUM_MOUSE_BUTTON_LEFT );
    GUM_InputSystem_bind(GUM_MOUSE_TYPE, GUM_INPUTACTION_CANCEL,  GUM_MOUSE_BUTTON_RIGHT);

    GUM_InputSystem_bind(GUM_GAMEPAD_TYPE, GUM_INPUTACTION_CONFIRM,    GUM_GAMEPAD_BUTTON_A);
    GUM_InputSystem_bind(GUM_GAMEPAD_TYPE, GUM_INPUTACTION_CANCEL,     GUM_GAMEPAD_BUTTON_B);
    GUM_InputSystem_bind(GUM_GAMEPAD_TYPE, GUM_INPUTACTION_MOVE_UP,    GUM_GAMEPAD_DPAD_UP);
    GUM_InputSystem_bind(GUM_GAMEPAD_TYPE, GUM_INPUTACTION_MOVE_DOWN,  GUM_GAMEPAD_DPAD_DOWN);
    GUM_InputSystem_bind(GUM_GAMEPAD_TYPE, GUM_INPUTACTION_MOVE_LEFT,  GUM_GAMEPAD_DPAD_LEFT);
    GUM_InputSystem_bind(GUM_GAMEPAD_TYPE, GUM_INPUTACTION_MOVE_RIGHT, GUM_GAMEPAD_DPAD_RIGHT);

}

void GUM_InputSystem_deinit(void) {
    GUM_unref(pMouse_);

    for (int i = 0; i < GUM_MAX_GAMEPADS; i++)
        if (pGamepads_[i]) GUM_unref(pGamepads_[i]);

    for (size_t i = 0; i < GBL_COUNT_OF(bindings_); i++)
        GblArrayList_destruct(&bindings_[i]);

}

GBL_RESULT GUM_InputSystem_bind(GblType deviceType, GUM_InputAction action, GblFlags button) {
    GUM_InputBinding binding = { .deviceType = deviceType, .button = button };
    return GblArrayList_pushBack(&bindings_[action], &binding);
}

GBL_RESULT GUM_InputSystem_unbind(GblType deviceType, GUM_InputAction action, GblFlags button) {
    GblArrayList* pList = &bindings_[action];

    for (size_t i = 0; i < GblArrayList_size(pList); i++) {
        GUM_InputBinding* pBinding = GblArrayList_at(pList, i);

        if (pBinding->deviceType == deviceType && pBinding->button == button)
            return GblArrayList_erase(pList, i, 1);
    }

    return GBL_RESULT_ERROR_INVALID_ARG;
}

static GUM_InputAction GUM_InputSystem_actionFor_(GblType deviceType, GblFlags button) {
    for (GUM_InputAction action = GUM_INPUTACTION_NULL + 1; action < GUM_INPUTACTION_COUNT; ++action) {
        GblArrayList* pList = &bindings_[action];

        for (size_t i = 0; i < GblArrayList_size(pList); i++) {
            GUM_InputBinding* pBinding = GblArrayList_at(pList, i);

            if (pBinding->deviceType == deviceType && pBinding->button == button)
                return action;
        }
    }

    return GUM_INPUTACTION_UNBOUND;
}

static GUM_Widget* GUM_InputSystem_focusedWidget_(void) {
    return nullptr;
}

///////// MOUSE /////////

static GUM_Widget* GUM_InputSystem_Mouse_hitTest_(GUM_Vector2 mousePos) {
    // using drawQueue because it's already Z-sorted
    GblArrayList* drawQueue = GUM_drawQueue_get();

    for (size_t i = GblArrayList_size(drawQueue); i-- > 0;) {
        GblObject*  pObj       = *(GblObject**)GblArrayList_at(drawQueue, i);
        GUM_Widget* pWidget    = GUM_WIDGET(pObj);
        GUM_Vector2 widgetPos  = GUM_get_absolute_position_(pWidget);
        GUM_Vector2 widgetSize = (GUM_Vector2){pWidget->w, pWidget->h};

        if (mousePos.x >= widgetPos.x &&
            mousePos.x <  widgetPos.x + widgetSize.x &&
            mousePos.y >= widgetPos.y &&
            mousePos.y <  widgetPos.y + widgetSize.y) {
            return pWidget;
        }
    }

    return nullptr;
}

static void GUM_InputSystem_Mouse_dispatchEvent_(GblFlags button, GUM_InputState state) {
    GUM_Event_Mouse* pEvent = GUM_Event_Mouse_create();
    GUM_EVENT_INPUT(pEvent)->button       = button;
    GUM_EVENT_INPUT(pEvent)->state        = state;
    GUM_EVENT_INPUT(pEvent)->action       = GUM_InputSystem_actionFor_(GUM_MOUSE_TYPE, button);
    GUM_EVENT_INPUT(pEvent)->pInputDevice = GUM_INPUTDEVICE(pMouse_);

    GUM_Widget* pTarget = GUM_InputSystem_Mouse_hitTest_(pMouse_->position);
    if (pTarget)
        GblObject_notifyEvent(GBL_OBJECT(pTarget), GBL_EVENT(pEvent));

    GBL_UNREF(pEvent);
}

static void GUM_InputSystem_Mouse_update_(void) {
    GUM_Backend_Mouse_update(pMouse_);
    GblFlags pressed  = GUM_INPUTDEVICE(pMouse_)->buttons     & ~GUM_INPUTDEVICE(pMouse_)->buttonsPrev;
    GblFlags released = GUM_INPUTDEVICE(pMouse_)->buttonsPrev & ~GUM_INPUTDEVICE(pMouse_)->buttons;
    GblFlags changed  = pressed | released;

    while (changed) {
        GblFlags bit = changed & (~changed + 1); // isolate lowest set bit
        changed &= ~bit;                         // clear it for next iteration

        GUM_InputSystem_Mouse_dispatchEvent_(bit, (pressed & bit) ? GUM_INPUTSTATE_PRESS : GUM_INPUTSTATE_RELEASE);
    }

    GUM_INPUTDEVICE(pMouse_)->buttonsPrev = GUM_INPUTDEVICE(pMouse_)->buttons;
}

///////// GAMEPAD /////////

static bool GUM_InputSystem_Gamepad_isNameValid(const char* pName) {
    if (!pName || pName[0] == '\0' || strcmp(pName, "UNKNOWN") == 0)
        return false;

    static const char* pBlocklist_[] = {
        "Motion Sensors",
        "Consumer Control",
        "Keychron",
        "Touchpad",
        "Keyboard"
    };

    for (size_t i = 0; i < GBL_COUNT_OF(pBlocklist_); i++) {
        if (strstr(pName, pBlocklist_[i]))
            return false;
    }

    return true;
}

static void GUM_InputSystem_Gamepad_dispatchEvent_(GUM_Gamepad* pGamepad, GblFlags button, GUM_InputState state) {
    GUM_Event_Gamepad* pEvent = GUM_Event_Gamepad_create();
    GUM_EVENT_INPUT(pEvent)->button       = button;
    GUM_EVENT_INPUT(pEvent)->state        = state;
    GUM_EVENT_INPUT(pEvent)->action       = GUM_InputSystem_actionFor_(GUM_GAMEPAD_TYPE, button);
    GUM_EVENT_INPUT(pEvent)->pInputDevice = GUM_INPUTDEVICE(pGamepad);

    // GUM_Widget* pTarget = GUM_InputSystem_focusedWidget_(); // TODO: placeholder

    // if (pTarget)
    //     GblObject_notifyEvent(GBL_OBJECT(pTarget), GBL_EVENT(pEvent));

    GUM_LOG_DEBUG_SCOPE("GAMEPAD EVENT!!") {
        GUM_LOG_DEBUG("Device name is : %s", GUM_INPUTDEVICE(pGamepad)->deviceName);
        GUM_LOG_DEBUG("Action is : %s"     , GblEnum_nick(GUM_EVENT_INPUT(pEvent)->action,  GBL_TYPEID(GUM_InputAction)));
        GUM_LOG_DEBUG("Button is : %s"     , GblFlags_nick(GUM_EVENT_INPUT(pEvent)->button, GBL_TYPEID(GUM_GAMEPAD_FLAGS)));
    }

    GBL_UNREF(pEvent);
}

static void GUM_InputSystem_Gamepad_update_(void) {
    uint8_t gamepadCount_ = 0;

    for (int rawIndex = 0; rawIndex < GUM_MAX_GAMEPADS; rawIndex++) {
        if (!GUM_Backend_Gamepad_isConnected(rawIndex) || !GUM_InputSystem_Gamepad_isNameValid(GUM_Backend_Gamepad_name(rawIndex)))
            continue;

        if (gamepadCount_ >= GUM_MAX_GAMEPADS)
            break;

        GUM_Gamepad* pGamepad = pGamepads_[gamepadCount_];

        if (!pGamepad || pGamepad->rawIndex != rawIndex) {
            if (pGamepad) GUM_unref(pGamepad); // old instance is stale, rebuild

            pGamepad = GUM_Gamepad_create("rawIndex", (uint8_t)rawIndex,
                                          "index",    gamepadCount_);
            pGamepads_[gamepadCount_] = pGamepad;
            GUM_LOG_DEBUG("Gamepad with name %s at index %i connected", GUM_INPUTDEVICE(pGamepads_[gamepadCount_])->deviceName, gamepadCount_);
        }

        gamepadCount_++;
    }

    // any player slots left over are no longer backed by a real device
    for (uint8_t i = gamepadCount_; i < GUM_MAX_GAMEPADS; i++) {
        if (pGamepads_[i]) {
            GUM_LOG_DEBUG("Gamepad with name %s at index %i disconnected", GUM_INPUTDEVICE(pGamepads_[gamepadCount_])->deviceName, gamepadCount_);
            GUM_unref(pGamepads_[i]);
            pGamepads_[i] = nullptr;
        }
    }

    // update and dispatch events
    for (uint8_t i = 0; i < GUM_MAX_GAMEPADS; i++) {
        GUM_Gamepad* pGamepad = pGamepads_[i];
        if (!pGamepad) continue;


        GUM_Backend_Gamepad_update(pGamepad);

        GblFlags pressed  = GUM_INPUTDEVICE(pGamepad)->buttons     & ~GUM_INPUTDEVICE(pGamepad)->buttonsPrev;
        GblFlags released = GUM_INPUTDEVICE(pGamepad)->buttonsPrev & ~GUM_INPUTDEVICE(pGamepad)->buttons;
        GblFlags changed  = pressed | released;

        while (changed) {
            GblFlags bit = changed & (~changed + 1);
            changed &= ~bit;

            GUM_InputSystem_Gamepad_dispatchEvent_(pGamepad, bit,
                (pressed & bit) ? GUM_INPUTSTATE_PRESS : GUM_INPUTSTATE_RELEASE);
        }

        GUM_INPUTDEVICE(pGamepad)->buttonsPrev = GUM_INPUTDEVICE(pGamepad)->buttons;
    }
}


void GUM_InputSystem_update(void) {
    GUM_InputSystem_Mouse_update_();
    GUM_InputSystem_Gamepad_update_();
}
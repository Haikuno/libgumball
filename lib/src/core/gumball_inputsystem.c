#include <gumball/core/gumball_inputsystem.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_navigation.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/gumball_elements.h>
#include <gumball/gumball_devices.h>
#include <gumball/gumball_events.h>
#include <gumball/gumball_types.h>

constexpr int GUM_MAX_GAMEPADS = 16;

static GUM_Mouse*    pMouse_                       = nullptr;
static GUM_Widget*   pHoveredWidget_               = nullptr; // different to pMouse_'s pFocusedWidget, this doesn't need to be selectable.
static GUM_Gamepad*  pGamepads_[GUM_MAX_GAMEPADS]  = { nullptr };
static GUM_Keyboard* pKeyboard_                    = nullptr;
static GblArrayList  bindings_[GUM_INPUTACTION_COUNT];

void GUM_InputSystem_init(void) {
    pMouse_    = GUM_Mouse_create();
    pKeyboard_ = GUM_Keyboard_create();

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

    GUM_InputSystem_bind(GUM_KEYBOARD_TYPE, GUM_INPUTACTION_CONFIRM,    GUM_KEYBOARD_KEY_ENTER);
    GUM_InputSystem_bind(GUM_KEYBOARD_TYPE, GUM_INPUTACTION_CANCEL,     GUM_KEYBOARD_KEY_ESCAPE);
    GUM_InputSystem_bind(GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_UP,    GUM_KEYBOARD_KEY_UP);
    GUM_InputSystem_bind(GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_DOWN,  GUM_KEYBOARD_KEY_DOWN);
    GUM_InputSystem_bind(GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_LEFT,  GUM_KEYBOARD_KEY_LEFT);
    GUM_InputSystem_bind(GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_RIGHT, GUM_KEYBOARD_KEY_RIGHT);
    GUM_InputSystem_bind(GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_UP,    GUM_KEYBOARD_KEY_W);
    GUM_InputSystem_bind(GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_DOWN,  GUM_KEYBOARD_KEY_S);
    GUM_InputSystem_bind(GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_LEFT,  GUM_KEYBOARD_KEY_A);
    GUM_InputSystem_bind(GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_RIGHT, GUM_KEYBOARD_KEY_D);
}

void GUM_InputSystem_deinit(void) {
    GUM_unref(pMouse_);
    GUM_unref(pKeyboard_);

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

void GUM_InputSystem_widgetDestroyed(GUM_Widget* pWidget) {
    if (!pWidget) return;

    if (pMouse_ && GUM_INPUTDEVICE(pMouse_)->pFocusedWidget == pWidget)
        GUM_INPUTDEVICE(pMouse_)->pFocusedWidget = nullptr;

    if (pKeyboard_ && GUM_INPUTDEVICE(pKeyboard_)->pFocusedWidget == pWidget)
        GUM_INPUTDEVICE(pKeyboard_)->pFocusedWidget = nullptr;

    for (int i = 0; i < GUM_MAX_GAMEPADS; i++)
        if (pGamepads_[i] && GUM_INPUTDEVICE(pGamepads_[i])->pFocusedWidget == pWidget)
            GUM_INPUTDEVICE(pGamepads_[i])->pFocusedWidget = nullptr;
}

static size_t GUM_InputSystem_liveDevices_(GUM_InputDevice* pOut[], size_t maxOut) {
    size_t n = 0;

    if (pMouse_    && n < maxOut) pOut[n++] = GUM_INPUTDEVICE(pMouse_);
    if (pKeyboard_ && n < maxOut) pOut[n++] = GUM_INPUTDEVICE(pKeyboard_);

    for (int i = 0; i < GUM_MAX_GAMEPADS && n < maxOut; i++)
        if (pGamepads_[i]) pOut[n++] = GUM_INPUTDEVICE(pGamepads_[i]);

    return n;
}

void GUM_InputSystem_drawFocusRings(GUM_Renderer* pRenderer) {
    GUM_InputDevice* devices[2 + GUM_MAX_GAMEPADS]; // Mouse, keyboard and all gamepads.
    size_t           deviceCount = GUM_InputSystem_liveDevices_(devices, GBL_COUNT_OF(devices));

    // TODO: a way to set these
    const float thickness = 3.0f;
    const float gap       = 1.0f;

    for (size_t i = 0; i < deviceCount; i++) {
        GUM_Widget* pWidget = devices[i]->pFocusedWidget;
        if (!pWidget || !devices[i]->highlight_a) continue;

        size_t rank = 0;
        for (size_t j = 0; j < i; j++)
            if (devices[j]->pFocusedWidget == pWidget) rank++;

        GUM_Vector2 pos    = GUM_get_absolute_position_(pWidget);
        float       outset = (float)(rank + 1) * (thickness + gap);

        GUM_Rectangle ring = { pos.x - outset, pos.y - outset,
                               pWidget->w + outset * 2, pWidget->h + outset * 2 };

        const bool needsClip = pWidget->clipRect.x     != GUM_CLIP_RECT_NONE_.x ||
                               pWidget->clipRect.width != GUM_CLIP_RECT_NONE_.width;

        GBL_SCOPE(needsClip ? GUM_Backend_beginScissor(pRenderer, pWidget->clipRect) : 0,
                  needsClip ? GUM_Backend_endScissor(pRenderer) : 0) {
            GUM_Backend_rectangleLinesDraw(pRenderer, ring, pWidget->border_radius, thickness,
                (GUM_Color){ devices[i]->highlight_r, devices[i]->highlight_g,
                             devices[i]->highlight_b, devices[i]->highlight_a });
        }
    }
}

typedef void (*GUM_InputSystem_ButtonDispatchFn_)(void* pContext, GblFlags button, GUM_InputState state);

static void GUM_InputSystem_dispatchButton_(GUM_InputDevice* pDevice, void* pContext,
                                            GUM_InputSystem_ButtonDispatchFn_ pFnDispatch) {
    GblFlags pressed  = pDevice->buttons     & ~pDevice->buttonsPrev;
    GblFlags released = pDevice->buttonsPrev & ~pDevice->buttons;
    GblFlags changed  = pressed | released;

    while (changed) {
        GblFlags bit = changed & (~changed + 1);
        changed &= ~bit;

        pFnDispatch(pContext, bit, (pressed & bit) ? GUM_INPUTSTATE_PRESS : GUM_INPUTSTATE_RELEASE);
    }

    pDevice->buttonsPrev = pDevice->buttons;
}

static void GUM_InputSystem_NavDevice_dispatchEvent_(GUM_InputDevice* pDevice, GblType deviceType,
                                                     GblType eventType, GblFlags button, GUM_InputState state) {
    GUM_Event_Input* pEvent = GUM_EVENT_INPUT(GblEvent_create(eventType));
    pEvent->button       = button;
    pEvent->state        = state;
    pEvent->action       = GUM_InputSystem_actionFor_(deviceType, button);
    pEvent->pInputDevice = pDevice;

    if (pDevice->pFocusedWidget)
        GblObject_notifyEvent(GBL_OBJECT(pDevice->pFocusedWidget), GBL_EVENT(pEvent));

    if (state == GUM_INPUTSTATE_PRESS &&
        pEvent->action >= GUM_INPUTACTION_MOVE_UP && pEvent->action <= GUM_INPUTACTION_MOVE_RIGHT) {
        GUM_Nav_move(pDevice, pEvent->action);
    }


    GBL_UNREF(pEvent);
}

// ---------------------------------- Mouse ---------------------------------- //

static void GUM_InputSystem_Mouse_hitTest_(void) {
    // using drawQueue because it's already Z-sorted
    GblArrayList* drawQueue = GUM_drawQueue_get();

    GUM_Vector2 mousePos = pMouse_->position;

    for (size_t i = GblArrayList_size(drawQueue); i-- > 0;) {
        GblObject*  pObj       = *(GblObject**)GblArrayList_at(drawQueue, i);
        GUM_Widget* pWidget    = GUM_WIDGET(pObj);
        GUM_Vector2 widgetPos  = GUM_get_absolute_position_(pWidget);
        GUM_Vector2 widgetSize = (GUM_Vector2){pWidget->w, pWidget->h};

        const GUM_Rectangle clip = pWidget->clipRect;
        const bool inClip = mousePos.x >= clip.x && mousePos.x < clip.x + clip.width &&
                            mousePos.y >= clip.y && mousePos.y < clip.y + clip.height;

        if (inClip &&
            mousePos.x >= widgetPos.x &&
            mousePos.x <  widgetPos.x + widgetSize.x &&
            mousePos.y >= widgetPos.y &&
            mousePos.y <  widgetPos.y + widgetSize.y) {
            pHoveredWidget_ = pWidget;
            if (pWidget->isSelectable)
                GUM_Nav_focus(GUM_INPUTDEVICE(pMouse_), pWidget);
            else
                GUM_Nav_focus(GUM_INPUTDEVICE(pMouse_), nullptr);
            break;
        }
    }
}

static void GUM_InputSystem_Mouse_dispatchEvent_(void* pContext, GblFlags button, GUM_InputState state) {
    GBL_UNUSED(pContext);

    GUM_Event_Mouse* pEvent = GUM_Event_Mouse_create();
    GUM_EVENT_INPUT(pEvent)->button       = button;
    GUM_EVENT_INPUT(pEvent)->state        = state;
    GUM_EVENT_INPUT(pEvent)->action       = GUM_InputSystem_actionFor_(GUM_MOUSE_TYPE, button);
    GUM_EVENT_INPUT(pEvent)->pInputDevice = GUM_INPUTDEVICE(pMouse_);

    if (GUM_INPUTDEVICE(pMouse_)->pFocusedWidget)
        GblObject_notifyEvent(GBL_OBJECT(GUM_INPUTDEVICE(pMouse_)->pFocusedWidget), GBL_EVENT(pEvent));

    GBL_UNREF(pEvent);
}

static void GUM_InputSystem_Mouse_update_(void) {
    GUM_Backend_Mouse_update(pMouse_);
    GUM_InputSystem_Mouse_hitTest_();

    // scrolling
    if (pHoveredWidget_) {
        for (GblObject* pAncestor = GBL_OBJECT(pHoveredWidget_); pAncestor; pAncestor = GblObject_parent(pAncestor)) {
            if (!GBL_TYPECHECK(GUM_Container, pAncestor)) continue;
            GUM_Container* pContainer = GUM_CONTAINER(pAncestor);
            if (!pContainer->scrollable) continue;

            const bool isHorizontal = pContainer->direction == GUM_DIRECTION_HORIZONTAL;
            float delta = (isHorizontal ? pMouse_->wheel.x : pMouse_->wheel.y) * -70.0f; // TODO: tune scroll speed

            if (isHorizontal)
                GUM_Animator_set(&pContainer->scrollAnimatorX,
                                 GBL_CLAMP(pContainer->scrollAnimatorX.to + delta, 0, GUM_WIDGET(pContainer)->w));
            else
                GUM_Animator_set(&pContainer->scrollAnimatorY,
                                 GBL_CLAMP(pContainer->scrollAnimatorY.to + delta, 0, GUM_WIDGET(pContainer)->h));

            if (delta) {
                GUM_CONTAINER_CLASSOF(pContainer)->pFnUpdateContent(pContainer); // re-layout + re-clamp
                break;
            }
        }
    }

    GUM_InputSystem_dispatchButton_(GUM_INPUTDEVICE(pMouse_), nullptr, GUM_InputSystem_Mouse_dispatchEvent_);
}

// ---------------------------------- Gamepad ---------------------------------- //

static bool GUM_InputSystem_Gamepad_isNameValid(const char* pName) {
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

static void GUM_InputSystem_Gamepad_dispatchEvent_(void* pContext, GblFlags button, GUM_InputState state) {
    GUM_Gamepad* pGamepad = (GUM_Gamepad*)pContext;
    GUM_InputSystem_NavDevice_dispatchEvent_(GUM_INPUTDEVICE(pGamepad), GUM_GAMEPAD_TYPE,
                                              GUM_EVENT_GAMEPAD_TYPE, button, state);
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
            GUM_LOG_DEBUG("Gamepad with name %s at index %i disconnected", GUM_INPUTDEVICE(pGamepads_[i])->deviceName, i);
            GUM_unref(pGamepads_[i]);
            pGamepads_[i] = nullptr;
        }
    }

    // update and dispatch events
    for (uint8_t i = 0; i < GUM_MAX_GAMEPADS; i++) {
        GUM_Gamepad* pGamepad = pGamepads_[i];
        if (!pGamepad) continue;


        GUM_Backend_Gamepad_update(pGamepad);
        GUM_InputSystem_dispatchButton_(GUM_INPUTDEVICE(pGamepad), pGamepad, GUM_InputSystem_Gamepad_dispatchEvent_);
    }
}

// ---------------------------------- Keyboard ---------------------------------- //

static void GUM_InputSystem_Keyboard_dispatchEvent_(void* pContext, GblFlags button, GUM_InputState state) {
    GBL_UNUSED(pContext);
    GUM_InputSystem_NavDevice_dispatchEvent_(GUM_INPUTDEVICE(pKeyboard_), GUM_KEYBOARD_TYPE,
                                              GUM_EVENT_KEY_TYPE, button, state);
}

static void GUM_InputSystem_Keyboard_update_(void) {
    GUM_Backend_Keyboard_update(pKeyboard_);
    GUM_InputSystem_dispatchButton_(GUM_INPUTDEVICE(pKeyboard_), nullptr, GUM_InputSystem_Keyboard_dispatchEvent_);
}


void GUM_InputSystem_update(void) {
    GUM_InputSystem_Mouse_update_();
    GUM_InputSystem_Gamepad_update_();
    GUM_InputSystem_Keyboard_update_();
}
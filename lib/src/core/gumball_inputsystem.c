#include <gumball/core/gumball_inputsystem.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_navigation.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/gumball_elements.h>
#include <gumball/gumball_devices.h>
#include <gumball/gumball_events.h>
#include <gumball/gumball_types.h>

#include "gumball_inputsystem_.h"
#include "../devices/gumball_inputdevice_.h"
#include "../elements/gumball_container_.h"
#include "../elements/gumball_root_.h"
#include "../elements/gumball_widget_.h"

constexpr int GUM_MAX_GAMEPADS = 16;

static GUM_Mouse*    pMouse_                       = nullptr;
static GUM_Widget*   pHoveredWidget_               = nullptr;
static GUM_Gamepad*  pGamepads_[GUM_MAX_GAMEPADS]  = { nullptr };
static GUM_Keyboard* pKeyboard_                    = nullptr;
static GblArrayList  bindings_[GUM_INPUTACTION_COUNT];
static GblArrayList  disabledInputTypes_;
static GblArrayList  disabledFocusRingTypes_;
static size_t        bindingListsConstructed_      = 0;
static uint64_t      generation_                   = 0;
static bool          inputTypeListConstructed_     = false;
static bool          focusRingTypeListConstructed_ = false;
static bool          inputTypeTransition_          = false;
static bool          gamepadResumeBaseline_        = false;
static bool          initialized_                  = false;
static bool          initializing_                 = false;
static bool          resetting_                    = false;
static bool          updating_                     = false;

static bool GUM_InputSystem_generationCurrent_(uint64_t generation) {
    return initialized_ && generation_ == generation;
}

uint64_t GUM_InputSystem_generation_(void) {
    return generation_;
}

static bool GUM_InputSystem_deviceTypeValid_(GblType deviceType) {
    return deviceType != GBL_INVALID_TYPE && GblType_check(deviceType, GUM_INPUTDEVICE_TYPE);
}

static size_t GUM_InputSystem_typeGateIndex_(const GblArrayList* pList, GblType deviceType) {
    const size_t count = GblArrayList_size(pList);
    for (size_t i = 0; i < count; ++i) {
        const GblType* pDisabledType = GblArrayList_at(pList, i);
        if (*pDisabledType == deviceType)
            return i;
    }
    return GBL_INDEX_INVALID;
}

static bool GUM_InputSystem_typeGateEnabled_(const GblArrayList* pList,
                                             bool constructed,
                                             GblType deviceType) {
    if (!constructed)
        return true;

    const size_t count = GblArrayList_size(pList);
    for (size_t i = 0; i < count; ++i) {
        const GblType* pDisabledType = GblArrayList_at(pList, i);
        if (GblType_check(deviceType, *pDisabledType))
            return false;
    }
    return true;
}

static GBL_RESULT GUM_InputSystem_typeGateDisable_(GblArrayList* pList,
                                                   GblType deviceType,
                                                   bool* pAdded) {
    if (GUM_InputSystem_typeGateIndex_(pList, deviceType) != GBL_INDEX_INVALID) {
        if (pAdded) *pAdded = false;
        return GBL_RESULT_SUCCESS;
    }

    const GBL_RESULT result = GblArrayList_pushBack(pList, &deviceType);
    if (pAdded) *pAdded = GBL_RESULT_SUCCESS(result);
    return result;
}

static void GUM_InputSystem_typeGateEnable_(GblArrayList* pList, GblType deviceType) {
    const size_t index = GUM_InputSystem_typeGateIndex_(pList, deviceType);
    if (index != GBL_INDEX_INVALID)
        GblArrayList_erase(pList, index, 1);
}

static GBL_RESULT GUM_InputSystem_bindInto_(GblType deviceType, GUM_InputAction action, GblFlags button) {
    if (action <= GUM_INPUTACTION_NULL || action >= GUM_INPUTACTION_COUNT)
        return GBL_RESULT_ERROR_INVALID_ARG;

    GUM_InputBinding binding = { .deviceType = deviceType, .button = button };
    return GblArrayList_pushBack(&bindings_[action], &binding);
}

static void GUM_InputSystem_reset_(void) {
    if (resetting_) {
        GUM_LOG_WARN("Ignoring recursive InputSystem reset.");
        return;
    }

    resetting_ = true;
    initializing_ = true;
    initialized_ = false;
    ++generation_;
    pHoveredWidget_ = nullptr;
    inputTypeTransition_ = false;
    gamepadResumeBaseline_ = false;

    GUM_Mouse* pMouse = pMouse_;
    GUM_Keyboard* pKeyboard = pKeyboard_;
    GUM_Gamepad* pGamepads[GBL_COUNT_OF(pGamepads_)];
    const size_t bindingCount = bindingListsConstructed_;

    pMouse_ = nullptr;
    pKeyboard_ = nullptr;
    bindingListsConstructed_ = 0;

    for (size_t i = 0; i < GBL_COUNT_OF(pGamepads_); ++i) {
        pGamepads[i] = pGamepads_[i];
        pGamepads_[i] = nullptr;
    }

    if (pMouse)
        GUM_unref(pMouse);
    if (pKeyboard)
        GUM_unref(pKeyboard);

    for (size_t i = 0; i < GBL_COUNT_OF(pGamepads); ++i)
        if (pGamepads[i])
            GUM_unref(pGamepads[i]);

    for (size_t i = 0; i < bindingCount; ++i)
        GblArrayList_destruct(&bindings_[i]);

    if (inputTypeListConstructed_) {
        GblArrayList_destruct(&disabledInputTypes_);
        inputTypeListConstructed_ = false;
    }
    if (focusRingTypeListConstructed_) {
        GblArrayList_destruct(&disabledFocusRingTypes_);
        focusRingTypeListConstructed_ = false;
    }

    initializing_ = false;
    resetting_ = false;
}

GBL_RESULT GUM_InputSystem_init(void) {
    if (initialized_)
        return GBL_RESULT_SUCCESS;

    if GBL_UNLIKELY (initializing_ || updating_) {
        GUM_LOG_WARN("Rejecting InputSystem initialization during another InputSystem operation.");
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    }

    initializing_ = true;
    pHoveredWidget_ = nullptr;
    inputTypeTransition_ = false;
    gamepadResumeBaseline_ = false;

    GBL_RESULT result = GblArrayList_construct(&disabledInputTypes_, sizeof(GblType));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GUM_InputSystem_reset_();
        return result;
    }
    inputTypeListConstructed_ = true;

    result = GblArrayList_construct(&disabledFocusRingTypes_, sizeof(GblType));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GUM_InputSystem_reset_();
        return result;
    }
    focusRingTypeListConstructed_ = true;

    pMouse_ = GUM_Mouse_create();
    if GBL_UNLIKELY (!pMouse_) {
        GUM_InputSystem_reset_();
        return GBL_RESULT_ERROR_MEM_ALLOC;
    }

    pKeyboard_ = GUM_Keyboard_create();
    if GBL_UNLIKELY (!pKeyboard_) {
        GUM_InputSystem_reset_();
        return GBL_RESULT_ERROR_MEM_ALLOC;
    }

    for (size_t i = 0; i < GBL_COUNT_OF(bindings_); ++i) {
        result = GblArrayList_construct(&bindings_[i], sizeof(GUM_InputBinding));
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            GUM_InputSystem_reset_();
            return result;
        }
        ++bindingListsConstructed_;
    }

    const struct {
        GblType         deviceType;
        GUM_InputAction action;
        GblFlags        button;
    } defaultBindings[] = {
        { GUM_MOUSE_TYPE,    GUM_INPUTACTION_CONFIRM,    GUM_MOUSE_BUTTON_LEFT  },
        { GUM_MOUSE_TYPE,    GUM_INPUTACTION_CANCEL,     GUM_MOUSE_BUTTON_RIGHT },
        { GUM_GAMEPAD_TYPE,  GUM_INPUTACTION_CONFIRM,    GUM_GAMEPAD_BUTTON_A },
        { GUM_GAMEPAD_TYPE,  GUM_INPUTACTION_CANCEL,     GUM_GAMEPAD_BUTTON_B },
        { GUM_GAMEPAD_TYPE,  GUM_INPUTACTION_MOVE_UP,    GUM_GAMEPAD_DPAD_UP },
        { GUM_GAMEPAD_TYPE,  GUM_INPUTACTION_MOVE_DOWN,  GUM_GAMEPAD_DPAD_DOWN },
        { GUM_GAMEPAD_TYPE,  GUM_INPUTACTION_MOVE_LEFT,  GUM_GAMEPAD_DPAD_LEFT },
        { GUM_GAMEPAD_TYPE,  GUM_INPUTACTION_MOVE_RIGHT, GUM_GAMEPAD_DPAD_RIGHT },
        { GUM_KEYBOARD_TYPE, GUM_INPUTACTION_CONFIRM,    GUM_KEYBOARD_KEY_ENTER },
        { GUM_KEYBOARD_TYPE, GUM_INPUTACTION_CANCEL,     GUM_KEYBOARD_KEY_ESCAPE },
        { GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_UP,    GUM_KEYBOARD_KEY_UP },
        { GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_DOWN,  GUM_KEYBOARD_KEY_DOWN },
        { GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_LEFT,  GUM_KEYBOARD_KEY_LEFT },
        { GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_RIGHT, GUM_KEYBOARD_KEY_RIGHT },
        { GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_UP,    GUM_KEYBOARD_KEY_W },
        { GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_DOWN,  GUM_KEYBOARD_KEY_S },
        { GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_LEFT,  GUM_KEYBOARD_KEY_A },
        { GUM_KEYBOARD_TYPE, GUM_INPUTACTION_MOVE_RIGHT, GUM_KEYBOARD_KEY_D }
    };

    for (size_t i = 0; i < GBL_COUNT_OF(defaultBindings); ++i) {
        result = GUM_InputSystem_bindInto_(defaultBindings[i].deviceType,
                                           defaultBindings[i].action,
                                           defaultBindings[i].button);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            GUM_InputSystem_reset_();
            return result;
        }
    }

    initializing_ = false;
    initialized_ = true;
    ++generation_;
    return GBL_RESULT_SUCCESS;
}

void GUM_InputSystem_deinit(void) {
    GUM_InputSystem_reset_();
}

GBL_RESULT GUM_InputSystem_bind(GblType deviceType, GUM_InputAction action, GblFlags button) {
    if GBL_UNLIKELY (!initialized_)
        return GBL_RESULT_ERROR_INVALID_OPERATION;

    return GUM_InputSystem_bindInto_(deviceType, action, button);
}

GBL_RESULT GUM_InputSystem_unbind(GblType deviceType, GUM_InputAction action, GblFlags button) {
    if GBL_UNLIKELY (!initialized_)
        return GBL_RESULT_ERROR_INVALID_OPERATION;

    if (action <= GUM_INPUTACTION_NULL || action >= GUM_INPUTACTION_COUNT)
        return GBL_RESULT_ERROR_INVALID_ARG;

    GblArrayList* pList = &bindings_[action];

    for (size_t i = 0; i < GblArrayList_size(pList); i++) {
        GUM_InputBinding* pBinding = GblArrayList_at(pList, i);

        if (pBinding->deviceType == deviceType && pBinding->button == button)
            return GblArrayList_erase(pList, i, 1);
    }

    return GBL_RESULT_ERROR_INVALID_ARG;
}

static void GUM_InputSystem_resetPointerTransient_(GUM_InputDevice* pDevice) {
    const GblType type = GBL_TYPEOF(pDevice);
    if (GblType_check(type, GUM_POINTER_TYPE))
        GUM_POINTER(pDevice)->delta = (GUM_Vector2){ 0, 0 };
    if (GblType_check(type, GUM_MOUSE_TYPE))
        GUM_MOUSE(pDevice)->wheel = (GUM_Vector2){ 0, 0 };
}

static void GUM_InputSystem_disableDevice_(GUM_InputDevice* pDevice, void* pClosure) {
    if (pClosure && !GUM_InputSystem_generationCurrent_(*(const uint64_t*)pClosure))
        return;

    GUM_InputDevice_resetTransient_(pDevice);
    GUM_InputDevice_requestBaseline_(pDevice);
    GUM_InputSystem_resetPointerTransient_(pDevice);
    GUM_Nav_focus(pDevice, nullptr);
}

GBL_EXPORT GblBool GUM_InputSystem_deviceTypeEnabled(GblType deviceType) {
    if (!initialized_ || !GUM_InputSystem_deviceTypeValid_(deviceType))
        return GBL_FALSE;

    return GUM_InputSystem_typeGateEnabled_(&disabledInputTypes_,
                                            inputTypeListConstructed_,
                                            deviceType);
}

GBL_EXPORT GblBool GUM_InputSystem_deviceEnabled(const GUM_InputDevice* pDevice) {
    if (!initialized_ || !pDevice || !GUM_InputDevice_enabled_(pDevice))
        return GBL_FALSE;

    return GUM_InputSystem_deviceTypeEnabled(GBL_TYPEOF(pDevice));
}

GBL_EXPORT GBL_RESULT GUM_InputSystem_enableDeviceInstance(GUM_InputDevice* pDevice) {
    if (!pDevice)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (!initialized_ || resetting_)
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    if (!GUM_InputSystem_deviceTypeValid_(GBL_TYPEOF(pDevice)))
        return GBL_RESULT_ERROR_INVALID_TYPE;
    if (GUM_InputDevice_enabled_(pDevice))
        return GBL_RESULT_SUCCESS;

    GUM_InputDevice_setEnabled_(pDevice, true);
    GUM_InputDevice_requestBaseline_(pDevice);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_InputSystem_disableDeviceInstance(GUM_InputDevice* pDevice) {
    if (!pDevice)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (!initialized_ || resetting_)
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    if (!GUM_InputSystem_deviceTypeValid_(GBL_TYPEOF(pDevice)))
        return GBL_RESULT_ERROR_INVALID_TYPE;
    if (!GUM_InputDevice_enabled_(pDevice))
        return GBL_RESULT_SUCCESS;

    GblBox_ref(GBL_BOX(pDevice));
    GUM_InputDevice_setEnabled_(pDevice, false);
    if (pMouse_ && pDevice == GUM_INPUTDEVICE(pMouse_))
        pHoveredWidget_ = nullptr;
    GUM_InputSystem_disableDevice_(pDevice, nullptr);
    GUM_InputDevice_setEnabled_(pDevice, false);
    GblBox_unref(GBL_BOX(pDevice));
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_InputSystem_disableDeviceType(GblType deviceType) {
    if (!initialized_ || resetting_)
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    if GBL_UNLIKELY (inputTypeTransition_) {
        GUM_LOG_WARN("Rejecting reentrant InputSystem device-type policy change.");
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    }
    if (!GUM_InputSystem_deviceTypeValid_(deviceType))
        return GBL_RESULT_ERROR_INVALID_TYPE;

    uint64_t generation = generation_;
    const bool gamepadsWereEnabled = GUM_InputSystem_deviceTypeEnabled(GUM_GAMEPAD_TYPE);
    bool added = false;
    GBL_RESULT result = GUM_InputSystem_typeGateDisable_(&disabledInputTypes_, deviceType, &added);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;
    if (!added)
        return GBL_RESULT_SUCCESS;

    inputTypeTransition_ = true;
    result = GUM_InputDevice_foreach_(deviceType, GUM_InputSystem_disableDevice_, &generation);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        inputTypeTransition_ = false;
        GUM_InputSystem_typeGateEnable_(&disabledInputTypes_, deviceType);
        return result;
    }

    if (!GUM_InputSystem_generationCurrent_(generation)) {
        inputTypeTransition_ = false;
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    }
    inputTypeTransition_ = false;

    if (GblType_check(GUM_MOUSE_TYPE, deviceType))
        pHoveredWidget_ = nullptr;

    if (gamepadsWereEnabled && !GUM_InputSystem_deviceTypeEnabled(GUM_GAMEPAD_TYPE))
        gamepadResumeBaseline_ = true;

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_InputSystem_enableDeviceType(GblType deviceType) {
    if (!initialized_ || resetting_)
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    if GBL_UNLIKELY (inputTypeTransition_) {
        GUM_LOG_WARN("Rejecting reentrant InputSystem device-type policy change.");
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    }
    if (!GUM_InputSystem_deviceTypeValid_(deviceType))
        return GBL_RESULT_ERROR_INVALID_TYPE;

    GUM_InputSystem_typeGateEnable_(&disabledInputTypes_, deviceType);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblBool GUM_InputSystem_focusRingTypeEnabled(GblType deviceType) {
    if (!initialized_ || !GUM_InputSystem_deviceTypeValid_(deviceType))
        return GBL_FALSE;

    return GUM_InputSystem_typeGateEnabled_(&disabledFocusRingTypes_,
                                            focusRingTypeListConstructed_,
                                            deviceType);
}

GBL_EXPORT GblBool GUM_InputSystem_focusRingEnabled(const GUM_InputDevice* pDevice) {
    if (!initialized_ || !pDevice || !GUM_InputDevice_focusRingEnabled_(pDevice))
        return GBL_FALSE;

    return GUM_InputSystem_focusRingTypeEnabled(GBL_TYPEOF(pDevice));
}

GBL_EXPORT GBL_RESULT GUM_InputSystem_enableFocusRingInstance(GUM_InputDevice* pDevice) {
    if (!pDevice)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (!initialized_ || resetting_)
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    if (!GUM_InputSystem_deviceTypeValid_(GBL_TYPEOF(pDevice)))
        return GBL_RESULT_ERROR_INVALID_TYPE;

    GUM_InputDevice_setFocusRingEnabled_(pDevice, true);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_InputSystem_disableFocusRingInstance(GUM_InputDevice* pDevice) {
    if (!pDevice)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (!initialized_ || resetting_)
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    if (!GUM_InputSystem_deviceTypeValid_(GBL_TYPEOF(pDevice)))
        return GBL_RESULT_ERROR_INVALID_TYPE;

    GUM_InputDevice_setFocusRingEnabled_(pDevice, false);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_InputSystem_enableFocusRingType(GblType deviceType) {
    if (!initialized_ || resetting_)
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    if (!GUM_InputSystem_deviceTypeValid_(deviceType))
        return GBL_RESULT_ERROR_INVALID_TYPE;

    GUM_InputSystem_typeGateEnable_(&disabledFocusRingTypes_, deviceType);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_InputSystem_disableFocusRingType(GblType deviceType) {
    if (!initialized_ || resetting_)
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    if (!GUM_InputSystem_deviceTypeValid_(deviceType))
        return GBL_RESULT_ERROR_INVALID_TYPE;

    return GUM_InputSystem_typeGateDisable_(&disabledFocusRingTypes_, deviceType, nullptr);
}

static GUM_InputAction GUM_InputSystem_actionFor_(GblType deviceType, GblFlags button) {
    if (!initialized_)
        return GUM_INPUTACTION_UNBOUND;

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

void GUM_InputSystem_widgetDestroyed_(GUM_Widget* pWidget) {
    if (!pWidget) return;

    if (pHoveredWidget_ == pWidget)
        pHoveredWidget_ = nullptr;

    GUM_InputDevice_widgetDestroyed_(pWidget);
}

static size_t GUM_InputSystem_liveDevices_(GUM_InputDevice* pOut[], size_t maxOut) {
    size_t n = 0;

    if (pMouse_    && n < maxOut) pOut[n++] = GUM_INPUTDEVICE(pMouse_);
    if (pKeyboard_ && n < maxOut) pOut[n++] = GUM_INPUTDEVICE(pKeyboard_);

    for (int i = 0; i < GUM_MAX_GAMEPADS && n < maxOut; i++)
        if (pGamepads_[i]) pOut[n++] = GUM_INPUTDEVICE(pGamepads_[i]);

    return n;
}

GBL_RESULT GUM_InputSystem_drawFocusRings_(GUM_Renderer* pRenderer) {
    GUM_InputDevice* devices[2 + GUM_MAX_GAMEPADS];
    size_t deviceCount = GUM_InputSystem_liveDevices_(devices, GBL_COUNT_OF(devices));

    const float thickness = 3.0f;
    const float gap       = 1.0f;

    for (size_t i = 0; i < deviceCount; i++) {
        GUM_Widget* pWidget = GUM_InputDevice_focusedWidget(devices[i]);
        if (!pWidget || !devices[i]->highlight_a || !GUM_InputSystem_focusRingEnabled(devices[i]))
            continue;

        size_t rank = 0;
        for (size_t j = 0; j < i; j++) {
            if (!devices[j]->highlight_a || !GUM_InputSystem_focusRingEnabled(devices[j]))
                continue;
            if (GUM_InputDevice_focusedWidget(devices[j]) == pWidget)
                ++rank;
        }

        const GUM_Vector2 pos = GUM_get_absolute_position_(pWidget);
        const float outset = (float)(rank + 1) * (thickness + gap);
        const GUM_Rectangle ring = { pos.x - outset, pos.y - outset,
                                     pWidget->w + outset * 2, pWidget->h + outset * 2 };
        const GUM_Rectangle clip = GUM_Widget_clipRect_(pWidget);
        const bool needsClip = clip.x != GUM_CLIP_RECT_NONE_.x ||
                               clip.width != GUM_CLIP_RECT_NONE_.width;
        bool scissorActive = false;
        GBL_RESULT result = GBL_RESULT_SUCCESS;

        if (needsClip) {
            result = GUM_Backend_beginScissor(pRenderer, clip);
            if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                return result;
            scissorActive = true;
        }

        result = GUM_Backend_rectangleLinesDraw(pRenderer,
                                                ring,
                                                pWidget->border_radius,
                                                thickness,
                                                (GUM_Color){ devices[i]->highlight_r,
                                                             devices[i]->highlight_g,
                                                             devices[i]->highlight_b,
                                                             devices[i]->highlight_a });

        if (scissorActive) {
            const GBL_RESULT endResult = GUM_Backend_endScissor(pRenderer);
            if (GBL_RESULT_SUCCESS(result) && !GBL_RESULT_SUCCESS(endResult))
                result = endResult;
        }

        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            return result;
    }

    return GBL_RESULT_SUCCESS;
}

typedef void (*GUM_InputSystem_ButtonDispatchFn_)(void* pContext, GblFlags button, GUM_InputState state);

typedef struct GUM_InputSystem_NavDispatch_ {
    GUM_InputDevice* pDevice;
    GblType          deviceType;
    GblType          eventType;
    uint64_t         generation;
} GUM_InputSystem_NavDispatch_;

typedef struct GUM_InputSystem_MouseDispatch_ {
    GUM_Mouse* pMouse;
    uint64_t   generation;
} GUM_InputSystem_MouseDispatch_;

static void GUM_InputSystem_dispatchButton_(GUM_InputDevice* pDevice, void* pContext,
                                            GUM_InputSystem_ButtonDispatchFn_ pFnDispatch,
                                            uint64_t generation) {
    if (!pDevice || !pFnDispatch || !GUM_InputSystem_deviceEnabled(pDevice))
        return;

    GblBox_ref(GBL_BOX(pDevice));

    const GblFlags pressed  = pDevice->buttons     & ~pDevice->buttonsPrev;
    const GblFlags released = pDevice->buttonsPrev & ~pDevice->buttons;
    GblFlags changed        = pressed | released;

    while (changed) {
        const GblFlags bit = changed & (~changed + 1);
        changed &= ~bit;
        pFnDispatch(pContext, bit, (pressed & bit) ? GUM_INPUTSTATE_PRESS : GUM_INPUTSTATE_RELEASE);
        if (!GUM_InputSystem_generationCurrent_(generation) ||
            !GUM_InputSystem_deviceEnabled(pDevice) ||
            GUM_InputDevice_baselinePending_(pDevice))
            break;
    }

    if (GUM_InputSystem_generationCurrent_(generation) &&
        GUM_InputSystem_deviceEnabled(pDevice) &&
        !GUM_InputDevice_baselinePending_(pDevice))
        pDevice->buttonsPrev = pDevice->buttons;

    GblBox_unref(GBL_BOX(pDevice));
}

static void GUM_InputSystem_NavDevice_dispatchEvent_(void* pContext,
                                                     GblFlags button,
                                                     GUM_InputState state) {
    GUM_InputSystem_NavDispatch_* pDispatch = pContext;
    GUM_InputDevice* pDevice = pDispatch->pDevice;
    GUM_Event_Input* pEvent = GUM_EVENT_INPUT(GblEvent_create(pDispatch->eventType));
    if (!pEvent)
        return;

    pEvent->button       = button;
    pEvent->state        = state;
    pEvent->action       = GUM_InputSystem_actionFor_(pDispatch->deviceType, button);
    pEvent->pInputDevice = pDevice;

    GUM_Widget* pFocusedWidget = GUM_InputDevice_focusedWidget(pDevice);
    if (pFocusedWidget)
        GblObject_notifyEvent(GBL_OBJECT(pFocusedWidget), GBL_EVENT(pEvent));

    if (GUM_InputSystem_generationCurrent_(pDispatch->generation) &&
        GUM_InputSystem_deviceEnabled(pDevice) &&
        !GUM_InputDevice_baselinePending_(pDevice) &&
        state == GUM_INPUTSTATE_PRESS &&
        pEvent->action >= GUM_INPUTACTION_MOVE_UP && pEvent->action <= GUM_INPUTACTION_MOVE_RIGHT) {
        GUM_Nav_move(pDevice, pEvent->action);
    }

    GBL_UNREF(pEvent);
}

static bool GUM_InputSystem_Mouse_hitTest_(GUM_Mouse* pMouse, uint64_t generation) {
    pHoveredWidget_ = GUM_Root_pointerTargetAt_(GUM_Root_active_(), GUM_POINTER(pMouse)->position);
    GUM_Nav_focus(GUM_INPUTDEVICE(pMouse),
                  pHoveredWidget_ && pHoveredWidget_->isSelectable ? pHoveredWidget_ : nullptr);
    return GUM_InputSystem_generationCurrent_(generation) &&
           GUM_InputSystem_deviceEnabled(GUM_INPUTDEVICE(pMouse)) &&
           !GUM_InputDevice_baselinePending_(GUM_INPUTDEVICE(pMouse));
}

static void GUM_InputSystem_Mouse_dispatchEvent_(void* pContext, GblFlags button, GUM_InputState state) {
    GUM_InputSystem_MouseDispatch_* pDispatch = pContext;
    GUM_Event_Mouse* pEvent = GUM_Event_Mouse_createFrom(pDispatch->pMouse);
    if (!pEvent)
        return;

    GUM_EVENT_INPUT(pEvent)->button = button;
    GUM_EVENT_INPUT(pEvent)->state  = state;
    GUM_EVENT_INPUT(pEvent)->action = GUM_InputSystem_actionFor_(GUM_MOUSE_TYPE, button);

    if (GUM_InputSystem_generationCurrent_(pDispatch->generation) &&
        GUM_InputSystem_deviceEnabled(GUM_INPUTDEVICE(pDispatch->pMouse)) &&
        pHoveredWidget_)
        GblObject_notifyEvent(GBL_OBJECT(pHoveredWidget_), GBL_EVENT(pEvent));

    GBL_UNREF(pEvent);
}

static bool GUM_InputSystem_Mouse_update_(uint64_t generation) {
    GUM_Mouse* pMouse = pMouse_;
    if (!pMouse)
        return false;

    GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pMouse);
    if (!GUM_InputSystem_deviceEnabled(pDevice))
        return true;

    GblBox_ref(GBL_BOX(pMouse));
    GUM_Backend_Mouse_update(pMouse);

    const bool baseline = GUM_InputDevice_takeBaseline_(pDevice);
    if (baseline) {
        pDevice->buttonsPrev = pDevice->buttons;
        GUM_InputSystem_resetPointerTransient_(pDevice);
    }

    bool current = GUM_InputSystem_generationCurrent_(generation) &&
                   GUM_InputSystem_Mouse_hitTest_(pMouse, generation);

    if (current && !baseline && pHoveredWidget_) {
        for (GblObject* pAncestor = GBL_OBJECT(pHoveredWidget_); pAncestor; pAncestor = GblObject_parent(pAncestor)) {
            if (!GBL_TYPECHECK(GUM_Container, pAncestor)) continue;
            GUM_Container* pContainer = GUM_CONTAINER(pAncestor);
            if (!GUM_Container_scrollable(pContainer)) continue;

            const GUM_Direction axis = GUM_Container_direction(pContainer);
            if (axis != GUM_DIRECTION_HORIZONTAL && axis != GUM_DIRECTION_VERTICAL)
                continue;

            const float delta = (axis == GUM_DIRECTION_HORIZONTAL ? pMouse->wheel.x : pMouse->wheel.y) * -70.0f;
            if (!delta)
                continue;

            GUM_Container_scrollBy_(pContainer, axis, delta);
            break;
        }
    }

    if (current && !baseline && GUM_InputSystem_generationCurrent_(generation)) {
        GUM_InputSystem_MouseDispatch_ dispatch = {
            .pMouse = pMouse,
            .generation = generation
        };
        GUM_InputSystem_dispatchButton_(pDevice,
                                        &dispatch,
                                        GUM_InputSystem_Mouse_dispatchEvent_,
                                        generation);
    }

    GblBox_unref(GBL_BOX(pMouse));
    return GUM_InputSystem_generationCurrent_(generation);
}

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

static bool GUM_InputSystem_Gamepad_update_(uint64_t generation) {
    if (!GUM_InputSystem_deviceTypeEnabled(GUM_GAMEPAD_TYPE))
        return true;

    const bool resumeBaseline = gamepadResumeBaseline_;
    uint8_t gamepadCount_ = 0;

    for (int rawIndex = 0; rawIndex < GUM_MAX_GAMEPADS; rawIndex++) {
        if (!GUM_InputSystem_deviceTypeEnabled(GUM_GAMEPAD_TYPE))
            return true;
        if (!GUM_Backend_Gamepad_isConnected(rawIndex) || !GUM_InputSystem_Gamepad_isNameValid(GUM_Backend_Gamepad_name(rawIndex)))
            continue;

        if (gamepadCount_ >= GUM_MAX_GAMEPADS)
            break;

        GUM_Gamepad* pGamepad = pGamepads_[gamepadCount_];

        if (!pGamepad || pGamepad->rawIndex != rawIndex) {
            pGamepads_[gamepadCount_] = nullptr;
            if (pGamepad)
                GUM_unref(pGamepad);

            if (!GUM_InputSystem_generationCurrent_(generation))
                return false;
            if (!GUM_InputSystem_deviceTypeEnabled(GUM_GAMEPAD_TYPE))
                return true;

            pGamepad = GUM_Gamepad_create("rawIndex", (uint8_t)rawIndex,
                                          "index",    gamepadCount_);
            if (!pGamepad) {
                GUM_LOG_ERROR("Failed to create gamepad at index %i", gamepadCount_);
                ++gamepadCount_;
                continue;
            }

            pGamepads_[gamepadCount_] = pGamepad;
            GUM_LOG_DEBUG("Gamepad with name %s at index %i connected",
                          GUM_InputDevice_name(GUM_INPUTDEVICE(pGamepad)),
                          gamepadCount_);
        }

        gamepadCount_++;
    }

    for (uint8_t i = gamepadCount_; i < GUM_MAX_GAMEPADS; i++) {
        GUM_Gamepad* pGamepad = pGamepads_[i];
        if (!pGamepad)
            continue;

        GUM_LOG_DEBUG("Gamepad with name %s at index %i disconnected",
                      GUM_InputDevice_name(GUM_INPUTDEVICE(pGamepad)),
                      i);
        pGamepads_[i] = nullptr;
        GUM_unref(pGamepad);

        if (!GUM_InputSystem_generationCurrent_(generation))
            return false;
        if (!GUM_InputSystem_deviceTypeEnabled(GUM_GAMEPAD_TYPE))
            return true;
    }

    if (resumeBaseline) {
        for (uint8_t i = 0; i < GUM_MAX_GAMEPADS; ++i)
            if (pGamepads_[i])
                GUM_InputDevice_requestBaseline_(GUM_INPUTDEVICE(pGamepads_[i]));
        gamepadResumeBaseline_ = false;
    }

    for (uint8_t i = 0; i < GUM_MAX_GAMEPADS; i++) {
        GUM_Gamepad* pGamepad = pGamepads_[i];
        if (!pGamepad) continue;

        GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pGamepad);
        if (!GUM_InputSystem_deviceEnabled(pDevice))
            continue;

        GUM_Backend_Gamepad_update(pGamepad);
        if (!GUM_InputSystem_generationCurrent_(generation))
            return false;

        if (GUM_InputDevice_takeBaseline_(pDevice)) {
            pDevice->buttonsPrev = pDevice->buttons;
            continue;
        }

        GUM_InputSystem_NavDispatch_ dispatch = {
            .pDevice = pDevice,
            .deviceType = GUM_GAMEPAD_TYPE,
            .eventType = GUM_EVENT_GAMEPAD_TYPE,
            .generation = generation
        };
        GUM_InputSystem_dispatchButton_(pDevice,
                                        &dispatch,
                                        GUM_InputSystem_NavDevice_dispatchEvent_,
                                        generation);

        if (!GUM_InputSystem_generationCurrent_(generation))
            return false;
        if (!GUM_InputSystem_deviceTypeEnabled(GUM_GAMEPAD_TYPE))
            return true;
    }

    return true;
}

static bool GUM_InputSystem_Keyboard_update_(uint64_t generation) {
    GUM_Keyboard* pKeyboard = pKeyboard_;
    if (!pKeyboard)
        return false;

    GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pKeyboard);
    if (!GUM_InputSystem_deviceEnabled(pDevice))
        return true;

    GblBox_ref(GBL_BOX(pKeyboard));
    GUM_Backend_Keyboard_update(pKeyboard);

    if (GUM_InputSystem_generationCurrent_(generation) &&
        GUM_InputSystem_deviceEnabled(pDevice)) {
        if (GUM_InputDevice_takeBaseline_(pDevice)) {
            pDevice->buttonsPrev = pDevice->buttons;
        } else {
            GUM_InputSystem_NavDispatch_ dispatch = {
                .pDevice = pDevice,
                .deviceType = GUM_KEYBOARD_TYPE,
                .eventType = GUM_EVENT_KEY_TYPE,
                .generation = generation
            };
            GUM_InputSystem_dispatchButton_(pDevice,
                                            &dispatch,
                                            GUM_InputSystem_NavDevice_dispatchEvent_,
                                            generation);
        }
    }

    GblBox_unref(GBL_BOX(pKeyboard));
    return GUM_InputSystem_generationCurrent_(generation);
}

void GUM_InputSystem_update(void) {
    if (!initialized_)
        return;

    if GBL_UNLIKELY (updating_) {
        GUM_LOG_WARN("Ignoring recursive InputSystem update.");
        return;
    }

    updating_ = true;
    const uint64_t generation = generation_;

    if (!GUM_InputSystem_Mouse_update_(generation))
        goto done;
    if (!GUM_InputSystem_Gamepad_update_(generation))
        goto done;
    GUM_InputSystem_Keyboard_update_(generation);

done:
    updating_ = false;
}

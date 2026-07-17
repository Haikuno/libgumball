#include <gumball/core/gumball_inputsystem.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/gumball_elements.h>
#include <gumball/gumball_devices.h>
#include <gumball/gumball_events.h>

static GUM_Mouse*   pMouse_    = nullptr;
static GblArrayList bindings_[GUM_INPUTACTION_COUNT];

void GUM_InputSystem_init(void) {
    pMouse_    = GUM_Mouse_create();

    for (size_t i = 0; i < GBL_COUNT_OF(bindings_); i++)
        GblArrayList_construct(&bindings_[i], sizeof(GUM_InputBinding));

    GUM_InputSystem_bind(GUM_MOUSE_TYPE, GUM_INPUTACTION_CONFIRM, GUM_MOUSE_BUTTON_LEFT );
    GUM_InputSystem_bind(GUM_MOUSE_TYPE, GUM_INPUTACTION_CANCEL,  GUM_MOUSE_BUTTON_RIGHT);
}

void GUM_InputSystem_deinit(void) {
    GUM_unref(pMouse_);

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

static void GUM_InputSystem_Mouse_dispatchButton_(GblFlags button, GUM_InputState state) {
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

        GUM_InputSystem_Mouse_dispatchButton_(bit, (pressed & bit) ? GUM_INPUTSTATE_PRESS : GUM_INPUTSTATE_RELEASE);
    }

    GUM_INPUTDEVICE(pMouse_)->buttonsPrev = GUM_INPUTDEVICE(pMouse_)->buttons;
}

static void GUM_InputSystem_Device_update_(void) {
    //
}

void GUM_InputSystem_update(void) {
    GUM_InputSystem_Mouse_update_();
}
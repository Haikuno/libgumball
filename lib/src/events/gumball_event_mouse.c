#include <gumball/events/gumball_event_mouse.h>
#include <gumball/devices/gumball_mouse.h>

GUM_Event_Mouse* GUM_Event_Mouse_createFrom(GUM_Mouse* pMouse) {
    GUM_Event_Mouse* pEvent = GUM_Event_Mouse_create();
    if (!pEvent || !pMouse)
        return pEvent;

    GUM_Pointer* pPointer = GUM_POINTER(pMouse);
    GUM_Event_Pointer* pPointerEvent = GUM_EVENT_POINTER(pEvent);

    pPointerEvent->position = pPointer->position;
    pPointerEvent->delta    = pPointer->delta;
    pEvent->wheel           = pMouse->wheel;
    GUM_EVENT_INPUT(pEvent)->pInputDevice = GUM_INPUTDEVICE(pMouse);
    return pEvent;
}

GblType GUM_Event_Mouse_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Event_Mouse"),
                                GUM_EVENT_POINTER_TYPE,
                                &(static GblTypeInfo){ .classSize    = sizeof(GUM_Event_MouseClass),
                                                       .instanceSize = sizeof(GUM_Event_Mouse) },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

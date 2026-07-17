#include <gumball/events/gumball_event_mouse.h>

GblType GUM_Event_Mouse_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Event_Mouse"),
                                GUM_EVENT_INPUT_TYPE,
                                &(static GblTypeInfo){.classSize       = sizeof(GUM_Event_MouseClass),
                                                      .instanceSize    = sizeof(GUM_Event_Mouse)},
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
#include <gumball/events/gumball_event_input.h>

GblType GUM_Event_Input_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Event_Input"),
                                GUM_EVENT_TYPE,
                                &(static GblTypeInfo){.classSize       = sizeof(GUM_Event_InputClass),
                                                      .instanceSize    = sizeof(GUM_Event_Input) },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
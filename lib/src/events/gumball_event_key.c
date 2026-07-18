#include <gumball/events/gumball_event_key.h>

GblType GUM_Event_Key_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Event_Key"),
                                GUM_EVENT_INPUT_TYPE,
                                &(static GblTypeInfo){.classSize       = sizeof(GUM_Event_KeyClass),
                                                      .instanceSize    = sizeof(GUM_Event_Key)},
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

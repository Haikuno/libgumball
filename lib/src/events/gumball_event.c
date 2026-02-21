#include <gumball/events/gumball_event.h>
#include <gumball/core/gumball_backend.h>

static GBL_RESULT GUM_Event_init_(GblInstance* pInstance) {
    GUM_EVENT(pInstance)->timestamp = GUM_Backend_timestamp();
    return GBL_RESULT_SUCCESS;
}

GblType GUM_Event_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Event"),
                                GBL_EVENT_TYPE,
                                &(static GblTypeInfo){.classSize       = sizeof(GUM_EventClass),
                                                      .instanceSize    = sizeof(GUM_Event),
                                                      .pFnInstanceInit = GUM_Event_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

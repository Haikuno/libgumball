#include <gumball/events/gumball_event_input.h>

static GBL_RESULT GUM_Event_Input_init_(GblInstance* pInstance) {
    GUM_Event_Input* pSelf = GUM_EVENT_INPUT(pInstance);

    pSelf->pInputDevice = nullptr;
    pSelf->button = 0;
    pSelf->state  = GUM_INPUTSTATE_NULL;
    pSelf->action = GUM_INPUTACTION_NULL;

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Event_Input_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Event_Input"),
                                GUM_EVENT_TYPE,
                                &(static GblTypeInfo){.classSize       = sizeof(GUM_Event_InputClass),
                                                      .instanceSize    = sizeof(GUM_Event_Input),
                                                      .pFnInstanceInit = GUM_Event_Input_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
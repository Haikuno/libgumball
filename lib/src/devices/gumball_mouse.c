#include <gumball/devices/gumball_mouse.h>

static GBL_RESULT GUM_Mouse_init_(GblInstance* pInstance) {
    GUM_Mouse* pSelf = GUM_MOUSE(pInstance);

    pSelf->position = (GUM_Vector2){0,0};
    pSelf->delta    = (GUM_Vector2){0,0};

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Mouse_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Mouse"),
                                GUM_INPUTDEVICE_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_MouseClass),
                                                       .instanceSize    = sizeof(GUM_Mouse),
                                                       .pFnInstanceInit = GUM_Mouse_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
#include <gumball/devices/gumball_gamepad.h>

static GBL_RESULT GUM_Gamepad_init_(GblInstance* pInstance) {
    GBL_UNUSED(pInstance);
    return GBL_RESULT_SUCCESS;
}

GblType GUM_Gamepad_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Gamepad"),
                                GUM_INPUTDEVICE_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_GamepadClass),
                                                       .instanceSize    = sizeof(GUM_Gamepad),
                                                       .pFnInstanceInit = GUM_Gamepad_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
#include <gumball/devices/gumball_keyboard.h>

static GBL_RESULT GUM_Keyboard_init_(GblInstance* pInstance) {
    GUM_INPUTDEVICE(pInstance)->highlight_r = 255;
    GUM_INPUTDEVICE(pInstance)->highlight_g = 180;
    GUM_INPUTDEVICE(pInstance)->highlight_b = 40;
    GUM_INPUTDEVICE(pInstance)->highlight_a = 255;

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Keyboard_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Keyboard"),
                                GUM_INPUTDEVICE_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_KeyboardClass),
                                                       .instanceSize    = sizeof(GUM_Keyboard),
                                                       .pFnInstanceInit = GUM_Keyboard_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

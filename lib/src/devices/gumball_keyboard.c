#include <gumball/devices/gumball_keyboard.h>

GblType GUM_Keyboard_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Keyboard"),
                                GUM_INPUTDEVICE_TYPE,
                                &(static GblTypeInfo){ .classSize    = sizeof(GUM_KeyboardClass),
                                                       .instanceSize = sizeof(GUM_Keyboard) },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

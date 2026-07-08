#include <gumball/devices/gumball_inputdevice.h>

GblType GUM_InputDevice_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_InputDevice"),
                                GBL_OBJECT_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_InputDeviceClass),
                                                       .instanceSize    = sizeof(GUM_InputDevice) },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
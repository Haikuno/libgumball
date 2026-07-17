#include <gumball/devices/gumball_inputdevice.h>

static GBL_RESULT GUM_InputDevice_init_(GblInstance* pInstance) {
    GUM_InputDevice* pSelf = GUM_INPUTDEVICE(pInstance);

    pSelf->buttons     = 0;
    pSelf->buttonsPrev = 0;
    pSelf->deviceName  = GblStringRef_create("UNKNOWN");

    return GBL_RESULT_SUCCESS;
}

GblType GUM_InputDevice_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_InputDevice"),
                                GBL_OBJECT_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_InputDeviceClass),
                                                       .instanceSize    = sizeof(GUM_InputDevice),
                                                       .pFnInstanceInit = GUM_InputDevice_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
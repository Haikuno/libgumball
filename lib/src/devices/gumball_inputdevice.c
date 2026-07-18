#include <gumball/devices/gumball_inputdevice.h>

static GBL_RESULT GUM_InputDevice_init_(GblInstance* pInstance) {
    GUM_InputDevice* pSelf = GUM_INPUTDEVICE(pInstance);

    pSelf->buttons        = 0;
    pSelf->buttonsPrev    = 0;
    pSelf->deviceName     = GblStringRef_create("UNKNOWN");

    pSelf->pFocusedWidget = nullptr;

    pSelf->highlight_r    = 255;
    pSelf->highlight_g    = 255;
    pSelf->highlight_b    = 255;
    pSelf->highlight_a    = 255;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_InputDevice_GblBox_destructor_(GblBox* pBox) {
    GUM_InputDevice* pSelf = GUM_INPUTDEVICE(pBox);
    GblStringRef_unref(pSelf->deviceName);

    GblObjectClass* pObjClass = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));
    return pObjClass->base.pFnDestructor(pBox);
}

static GBL_RESULT GUM_InputDeviceClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    GBL_BOX_CLASS(pClass)->pFnDestructor = GUM_InputDevice_GblBox_destructor_;

    return GBL_RESULT_SUCCESS;
}

GblType GUM_InputDevice_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_InputDevice"),
                                GBL_OBJECT_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_InputDeviceClass),
                                                       .pFnClassInit    = GUM_InputDeviceClass_init_,
                                                       .instanceSize    = sizeof(GUM_InputDevice),
                                                       .pFnInstanceInit = GUM_InputDevice_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
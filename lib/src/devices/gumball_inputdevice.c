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

static GBL_RESULT GUM_InputDevice_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GUM_InputDevice* pSelf = GUM_INPUTDEVICE(pObject);
    switch (pProp->id) {
        case GUM_InputDevice_Property_Id_highlight_color:
            uint32_t highlight_color_;
            GblVariant_valueCopy(pValue, &highlight_color_);
            pSelf->highlight_r = (highlight_color_ >> 24) & 0xFF;
            pSelf->highlight_g = (highlight_color_ >> 16) & 0xFF;
            pSelf->highlight_b = (highlight_color_ >> 8)  & 0xFF;
            pSelf->highlight_a =  highlight_color_        & 0xFF;
            break;
        case GUM_InputDevice_Property_Id_highlight_r:
            GblVariant_valueCopy(pValue, &pSelf->highlight_r);
            break;
        case GUM_InputDevice_Property_Id_highlight_g:
            GblVariant_valueCopy(pValue, &pSelf->highlight_g);
            break;
        case GUM_InputDevice_Property_Id_highlight_b:
            GblVariant_valueCopy(pValue, &pSelf->highlight_b);
            break;
        case GUM_InputDevice_Property_Id_highlight_a:
            GblVariant_valueCopy(pValue, &pSelf->highlight_a);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_InputDevice_GblObject_property_(const GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GUM_InputDevice* pSelf = GUM_INPUTDEVICE(pObject);
    switch (pProp->id) {
        case GUM_InputDevice_Property_Id_highlight_color:
            GblVariant_setUint32(pValue, pSelf->highlight_r << 24 |
                                         pSelf->highlight_g << 16 |
                                         pSelf->highlight_b << 8  |
                                         pSelf->highlight_a);
            break;
        case GUM_InputDevice_Property_Id_highlight_r:
            GblVariant_setUint8(pValue, pSelf->highlight_r);
            break;
        case GUM_InputDevice_Property_Id_highlight_g:
            GblVariant_setUint8(pValue, pSelf->highlight_g);
            break;
        case GUM_InputDevice_Property_Id_highlight_b:
            GblVariant_setUint8(pValue, pSelf->highlight_b);
            break;
        case GUM_InputDevice_Property_Id_highlight_a:
            GblVariant_setUint8(pValue, pSelf->highlight_a);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_InputDeviceClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_INPUTDEVICE_TYPE))
        GBL_PROPERTIES_REGISTER(GUM_InputDevice);

    GBL_BOX_CLASS(pClass)->pFnDestructor     = GUM_InputDevice_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GUM_InputDevice_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GUM_InputDevice_GblObject_property_;

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
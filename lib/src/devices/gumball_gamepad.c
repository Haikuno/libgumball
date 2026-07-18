#include <gumball/devices/gumball_gamepad.h>
#include <gumball/core/gumball_backend.h>

// Default focus-ring colors, keyed by player index. Wraps around for index >= count.
static const uint8_t GUM_Gamepad_playerColors_[][4] = {
    { 255,  90,  90, 255 }, // red
    {  80, 160, 255, 255 }, // blue
    { 255, 210,  60, 255 }, // yellow
    {  90, 220, 120, 255 }, // green
};
#define GUM_GAMEPAD_PLAYER_COLOR_COUNT (sizeof(GUM_Gamepad_playerColors_) / sizeof(GUM_Gamepad_playerColors_[0]))

static void GUM_Gamepad_applyPlayerColor_(GUM_Gamepad* pSelf) {
    const uint8_t* pColor = GUM_Gamepad_playerColors_[pSelf->index % GUM_GAMEPAD_PLAYER_COLOR_COUNT];

    GUM_INPUTDEVICE(pSelf)->highlight_r = pColor[0];
    GUM_INPUTDEVICE(pSelf)->highlight_g = pColor[1];
    GUM_INPUTDEVICE(pSelf)->highlight_b = pColor[2];
    GUM_INPUTDEVICE(pSelf)->highlight_a = pColor[3];
}

static GBL_RESULT GUM_Gamepad_init_(GblInstance* pInstance) {
    GUM_GAMEPAD(pInstance)->index    = 0;
    GUM_GAMEPAD(pInstance)->rawIndex = 0;
    GUM_Gamepad_applyPlayerColor_(GUM_GAMEPAD(pInstance));
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Gamepad_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GUM_Gamepad* pSelf = GUM_GAMEPAD(pObject);
    switch (pProp->id) {
        case GUM_Gamepad_Property_Id_index:
            GblVariant_valueCopy(pValue, &pSelf->index);
            GUM_Gamepad_applyPlayerColor_(pSelf);
            break;
        case GUM_Gamepad_Property_Id_rawIndex:
            GblVariant_valueCopy(pValue, &pSelf->rawIndex);
            GblStringRef_unref(GUM_INPUTDEVICE(pSelf)->deviceName);
            GUM_INPUTDEVICE(pSelf)->deviceName = GblStringRef_create(GUM_Backend_Gamepad_name(pSelf->rawIndex));
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Gamepad_GblObject_property_(const GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GUM_Gamepad* pSelf = GUM_GAMEPAD(pObject);
    switch (pProp->id) {
        case GUM_Gamepad_Property_Id_index:
            GblVariant_setUint8(pValue, pSelf->index);
            break;
        case GUM_Gamepad_Property_Id_rawIndex:
            GblVariant_setUint8(pValue, pSelf->rawIndex);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_GamepadClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_GAMEPAD_TYPE))
        GBL_PROPERTIES_REGISTER(GUM_Gamepad);

    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GUM_Gamepad_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GUM_Gamepad_GblObject_property_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_Gamepad_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Gamepad"),
                                GUM_INPUTDEVICE_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_GamepadClass),
                                                       .pFnClassInit    = GUM_GamepadClass_init_,
                                                       .instanceSize    = sizeof(GUM_Gamepad),
                                                       .pFnInstanceInit = GUM_Gamepad_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}
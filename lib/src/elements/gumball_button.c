#include <gumball/elements/gumball_button.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/elements/gumball_common.h>

static GBL_RESULT GUM_Button_init_(GblInstance* pInstance) {
    GUM_Button* pButton = GUM_BUTTON(pInstance);

    pButton->isActive            = true;
    pButton->isSelectable        = true;
    pButton->isSelectedByDefault = false;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Button_Widget_handleInputEvent_(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    GUM_Button* pButton = GUM_BUTTON(pSelf);

    if (!pButton->isActive || !pEvent->state || !pEvent->action)
        return GBL_RESULT_SUCCESS;


    static const char* pressActionSignals_[] = {
        [GUM_INPUTACTION_CONFIRM]    = "onPressConfirm",
        [GUM_INPUTACTION_CANCEL]     = "onPressCancel",
        [GUM_INPUTACTION_MOVE_UP]    = "onPressMoveUp",
        [GUM_INPUTACTION_MOVE_DOWN]  = "onPressMoveDown",
        [GUM_INPUTACTION_MOVE_LEFT]  = "onPressMoveLeft",
        [GUM_INPUTACTION_MOVE_RIGHT] = "onPressMoveRight"
    };

    static const char* releaseActionSignals_[] = {
        [GUM_INPUTACTION_CONFIRM]    = "onReleaseConfirm",
        [GUM_INPUTACTION_CANCEL]     = "onReleaseCancel",
        [GUM_INPUTACTION_MOVE_UP]    = "onReleaseMoveUp",
        [GUM_INPUTACTION_MOVE_DOWN]  = "onReleaseMoveDown",
        [GUM_INPUTACTION_MOVE_LEFT]  = "onReleaseMoveLeft",
        [GUM_INPUTACTION_MOVE_RIGHT] = "onReleaseMoveRight"
    };

    const char* signal;

    if (pEvent->state == GUM_INPUTSTATE_PRESS) {
        GBL_EMIT(pButton, "onPress", pEvent);
        signal = pEvent->action == GUM_INPUTACTION_UNBOUND ? "onPressUnbound" : pressActionSignals_[pEvent->action];
    } else if (pEvent->state == GUM_INPUTSTATE_RELEASE) {
        GBL_EMIT(pButton, "onRelease", pEvent);
        signal = pEvent->action == GUM_INPUTACTION_UNBOUND ? "onReleaseUnbound" : releaseActionSignals_[pEvent->action];
    }

    GBL_EMIT(pButton, signal);
    GblEvent_accept(GBL_EVENT(pEvent));

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Button_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GUM_Button* pSelf = GUM_BUTTON(pObject);
    switch (pProp->id) {
        case GUM_Button_Property_Id_isActive:
            GblVariant_valueCopy(pValue, &pSelf->isActive);
            break;
        case GUM_Button_Property_Id_isSelectable:
            GblVariant_valueCopy(pValue, &pSelf->isSelectable);
            break;
        case GUM_Button_Property_Id_isSelectedByDefault:
            GblVariant_valueCopy(pValue, &pSelf->isSelectedByDefault);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Button_GblObject_property_(const GblObject* pObject, const GblProperty* pProp,
                                                 GblVariant* pValue) {
    GUM_Button* pSelf = GUM_BUTTON(pObject);

    switch (pProp->id) {
        case GUM_Button_Property_Id_isActive:
            GblVariant_setBool(pValue, pSelf->isActive);
            break;
        case GUM_Button_Property_Id_isSelectable:
            GblVariant_setBool(pValue, pSelf->isSelectable);
            break;
        case GUM_Button_Property_Id_isSelectedByDefault:
            GblVariant_setBool(pValue, pSelf->isSelectedByDefault);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ButtonClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_BUTTON_TYPE))
        GBL_PROPERTIES_REGISTER(GUM_Button);

    GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GUM_Button_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty    = GUM_Button_GblObject_property_;

    GUM_WIDGET_CLASS(pClass)->pFnInputEvent = GUM_Button_Widget_handleInputEvent_;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ButtonClass_final_(GblClass* pClass, const void* pClassData) {
    GBL_UNUSED(pClassData);

    if (!GblType_classRefCount(GUM_BUTTON_TYPE))
        GblProperty_uninstallAll(GUM_BUTTON_TYPE);

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Button_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Button"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_ButtonClass),
                                                       .pFnClassInit    = GUM_ButtonClass_init_,
                                                       .instanceSize    = sizeof(GUM_Button),
                                                       .pFnInstanceInit = GUM_Button_init_,
                                                       .pFnClassFinal   = GUM_ButtonClass_final_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

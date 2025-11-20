#include <gumball/elements/gumball_button.h>
#include <gumball/elements/gumball_controller.h>
#include <gumball/elements/gumball_root.h>

static GBL_RESULT GUM_Button_init_(GblInstance *pInstance) {
	GUM_Button *pButton = GUM_BUTTON(pInstance);

	pButton->isActive				= true;
	pButton->isSelectable			= true;
	pButton->isSelected				= false;
	pButton->isSelectedByDefault	= false;

	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Button_GblObject_setProperty_(GblObject *pObject, const GblProperty *pProp, GblVariant *pValue) {
	GUM_Button *pSelf = GUM_BUTTON(pObject);
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

static GBL_RESULT GUM_Button_GblObject_property_(const GblObject *pObject, const GblProperty *pProp, GblVariant *pValue) {
	GUM_Button *pSelf = GUM_BUTTON(pObject);

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

static GBL_RESULT GUM_ButtonClass_init_(GblClass *pClass, const void *pData) {
	GBL_UNUSED(pData);

	if (!GblType_classRefCount(GBL_CLASS_TYPEOF(pClass))) {
		GBL_PROPERTIES_REGISTER(GUM_Button);

		GblSignal_install(  GUM_BUTTON_TYPE,
							"onPressPrimary",
							GblMarshal_CClosure_VOID__INSTANCE,
							0);

		GblSignal_install(  GUM_BUTTON_TYPE,
							"onPressSecondary",
							GblMarshal_CClosure_VOID__INSTANCE,
							0);

		GblSignal_install(  GUM_BUTTON_TYPE,
							"onPressTertiary",
							GblMarshal_CClosure_VOID__INSTANCE,
							0);
	}

	GBL_OBJECT_CLASS(pClass)->pFnSetProperty = GUM_Button_GblObject_setProperty_;
	GBL_OBJECT_CLASS(pClass)->pFnProperty    = GUM_Button_GblObject_property_;

	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ButtonClass_final_(GblClass* pClass, const void* pClassData) {
    GBL_UNUSED(pClassData);

    if(!GblType_classRefCount(GUM_BUTTON_TYPE)) {
        GblSignal_uninstall(GUM_BUTTON_TYPE, "onPressPrimary");
		GblSignal_uninstall(GUM_BUTTON_TYPE, "onPressSecondary");
		GblSignal_uninstall(GUM_BUTTON_TYPE, "onPressTertiary");
    }

	return GBL_RESULT_SUCCESS;
}

GblType GUM_Button_type(void) {
	static GblType type = GBL_INVALID_TYPE;

	if (type == GBL_INVALID_TYPE) {
		type = GblType_register(GblQuark_internStatic("GUM_Button"),
								GUM_WIDGET_TYPE,
								&(static GblTypeInfo){.classSize = sizeof(GUM_ButtonClass),
													  .pFnClassInit = GUM_ButtonClass_init_,
													  .instanceSize = sizeof(GUM_Button),
													  .pFnInstanceInit = GUM_Button_init_,
													  .pFnClassFinal = GUM_ButtonClass_final_},
								GBL_TYPE_FLAG_TYPEINFO_STATIC);

	}

	return type;
}
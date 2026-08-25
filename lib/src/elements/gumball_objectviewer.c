#include <gumball/elements/gumball_objectviewer.h>
#include <gumball/elements/gumball_common.h>
#include <gimbal/utils/gimbal_ref.h>

static GBL_RESULT GUM_ObjectViewer_init_(GblInstance* pInstance) {
    GUM_OBJECTVIEWER(pInstance)->pObject = nullptr;
    GUM_WIDGET(pInstance)->a = 0;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ObjectViewer_GblObject_setProperty_(GblObject* pObject,
                                                          const GblProperty* pProp,
                                                          GblVariant* pValue) {
    GUM_ObjectViewer* pSelf = GUM_OBJECTVIEWER(pObject);

    switch (pProp->id) {
        case GUM_ObjectViewer_Property_Id_object: {
            GblObject* pReplacement = nullptr;
            const GBL_RESULT result = GblVariant_valueCopy(pValue, &pReplacement);
            if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                return result;

            if (pSelf->pObject)
                GBL_UNREF(pSelf->pObject);
            pSelf->pObject = pReplacement;
            GUM_WIDGET(pSelf)->shouldUpdate = true;
            break;
        }
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ObjectViewer_GblObject_property_(const GblObject* pObject,
                                                       const GblProperty* pProp,
                                                       GblVariant* pValue) {
    const GUM_ObjectViewer* pSelf = GUM_OBJECTVIEWER(pObject);

    switch (pProp->id) {
        case GUM_ObjectViewer_Property_Id_object:
            return GblVariant_setValueCopy(pValue, pProp->valueType, pSelf->pObject);
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }
}

// Presentation is deferred until reusable Table/Tree primitives exist.
static GBL_RESULT GUM_ObjectViewer_update_(GUM_Widget* pSelf) {
    pSelf->shouldUpdate = false;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ObjectViewer_Object_instantiated_(GblObject* pObject) {
    GblObjectClass* pWidgetClass = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE));
    return pWidgetClass->pFnInstantiated(pObject);
}

static GBL_RESULT GUM_ObjectViewer_Widget_deactivate_(GUM_Widget* pSelf) {
    GUM_ObjectViewer* pViewer = GUM_OBJECTVIEWER(pSelf);
    if (pViewer->pObject) {
        GBL_UNREF(pViewer->pObject);
        pViewer->pObject = nullptr;
    }

    GUM_WidgetClass* pWidgetClass = GUM_WIDGET_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE));
    return pWidgetClass->pFnDeactivate(pSelf);
}

static GBL_RESULT GUM_ObjectViewer_GblBox_destructor_(GblBox* pBox) {
    GUM_ObjectViewer* pSelf = GUM_OBJECTVIEWER(pBox);
    if (pSelf->pObject) {
        GBL_UNREF(pSelf->pObject);
        pSelf->pObject = nullptr;
    }

    GblBoxClass* pWidgetClass = GBL_BOX_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE));
    return pWidgetClass->pFnDestructor(pBox);
}

static GBL_RESULT GUM_ObjectViewerClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_OBJECTVIEWER_TYPE))
        GBL_PROPERTIES_REGISTER(GUM_ObjectViewer);

    GBL_BOX_CLASS(pClass)->pFnDestructor      = GUM_ObjectViewer_GblBox_destructor_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty  = GUM_ObjectViewer_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty     = GUM_ObjectViewer_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)->pFnInstantiated = GUM_ObjectViewer_Object_instantiated_;

    GUM_WIDGET_CLASS(pClass)->pFnDeactivate = GUM_ObjectViewer_Widget_deactivate_;
    GUM_WIDGET_CLASS(pClass)->pFnUpdate     = GUM_ObjectViewer_update_;

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_ObjectViewer_setObject(GUM_ObjectViewer* pSelf, GblObject* pObject) {
    if GBL_UNLIKELY (!pSelf || !pObject)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GblObject* pNewObject = GBL_OBJECT(GBL_REF(pObject));
    if (pSelf->pObject)
        GBL_UNREF(pSelf->pObject);

    pSelf->pObject = pNewObject;
    GUM_WIDGET(pSelf)->shouldUpdate = true;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_ObjectViewer_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_ObjectViewer"),
                                GUM_CONTAINER_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_ObjectViewerClass),
                                                       .pFnClassInit    = GUM_ObjectViewerClass_init_,
                                                       .instanceSize    = sizeof(GUM_ObjectViewer),
                                                       .pFnInstanceInit = GUM_ObjectViewer_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

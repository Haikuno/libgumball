#include <gumball/elements/gumball_objectviewer.h>
#include <gumball/elements/gumball_common.h>
#include <gumball/elements/gumball_button.h>
#include <gumball/core/gumball_logger.h>
#include <gimbal/utils/gimbal_ref.h>

static GBL_RESULT GUM_ObjectViewer_init_(GblInstance* pInstance) {
    GUM_OBJECTVIEWER(pInstance)->pObject = nullptr;

    GUM_WIDGET(pInstance)->a = 0;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ObjectViewer_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProp,
                                                          GblVariant* pValue) {
    GUM_ObjectViewer* pSelf = GUM_OBJECTVIEWER(pObject);

    switch (pProp->id) {
        case GUM_ObjectViewer_Property_Id_object:
            if (pSelf->pObject)
                GBL_UNREF(pSelf->pObject);
            GblVariant_valueCopy(pValue, &pSelf->pObject);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ObjectViewer_GblObject_property_(const GblObject* pObject, const GblProperty* pProp,
                                                       GblVariant* pValue) {
    GUM_ObjectViewer* pSelf = GUM_OBJECTVIEWER(pObject);

    switch (pProp->id) {
        case GUM_ObjectViewer_Property_Id_object:
            GblVariant_setValueCopy(pValue,
                                    pProp->valueType,
                                    pSelf->pObject);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ObjectViewer_update_(GUM_Widget* pSelf) {
    GUM_ObjectViewer* pViewer = GUM_OBJECTVIEWER(pSelf);
    if (!pViewer->pObject)
        return GBL_RESULT_INCOMPLETE;

    GblObject_foreachChild(GBL_OBJECT(pSelf), pChild)
        GUM_unref(pChild);

    GUM_Container_create("color", 255,   "parent", pSelf, "direction", GUM_DIRECTION_HORIZONTAL,
                         "margin", 2.0f, "padding", 0.0f, "children",
        GUM_childrenList(
            GUM_Widget_create("font_size", 15, "color", 0xB0B0B0FF, "label", "Type"),
            GUM_Widget_create("font_size", 15, "color", 0x909090FF, "label", "Key"),
            GUM_Widget_create("font_size", 15, "color", 0x707070FF, "label", "Value")
        )
    );

    GBL_VARIANT(table);
    GBL_VARIANT(key);
    GBL_VARIANT(value);

    GblVariant_constructObjectCopy(&table, pViewer->pObject);
    GblVariant_construct(&key);
    GblVariant_construct(&value);

    while (GblVariant_next(&table, &key, &value)) {
        if (!GblVariant_canConvert(GblVariant_typeOf(&value), GBL_STRING_TYPE))
            continue;

        GUM_Container_create("color", 255,   "parent", pSelf, "direction", GUM_DIRECTION_HORIZONTAL,
                             "margin", 2.0f, "padding", 0.0f, "children",
            GUM_childrenList(
                GUM_Button_create("font_size", 15, "color", 0xB0B0B0FF, "label",        GblVariant_typeName(&value)),
                GUM_Button_create("font_size", 15, "color", 0x909090FF, "label",        GblVariant_string(&key)),
                GUM_Button_create("font_size", 15, "color", 0x707070FF, "labelAcquire", GblVariant_asString(&value))
            )
        );
    }

    GblVariant_destruct(&value);
    GblVariant_destruct(&key);
    GblVariant_destruct(&table);

    GUM_CONTAINER_CLASSOF(pSelf)->pFnUpdateContent(GUM_CONTAINER(pSelf));

    pSelf->shouldUpdate = false;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ObjectViewer_Object_instantiated_(GblObject* pObject) {
    GBL_CTX_BEGIN(nullptr);
    GBL_VCALL_DEFAULT(GUM_Widget, base.pFnInstantiated, pObject);
    GBL_CTX_END();

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ObjectViewer_Widget_deactivate_(GUM_Widget* pSelf) {
    GUM_ObjectViewer* pViewer = GUM_OBJECTVIEWER(pSelf);
    if (pViewer->pObject) {
        GBL_UNREF(pViewer->pObject);
        pViewer->pObject = nullptr;
    }

    GBL_CTX_BEGIN(nullptr);
    GBL_VCALL_DEFAULT(GUM_Widget, pFnDeactivate, pSelf);
    GBL_CTX_END();

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ObjectViewerClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_OBJECTVIEWER_TYPE))
        GBL_PROPERTIES_REGISTER(GUM_ObjectViewer);

    GBL_OBJECT_CLASS(pClass)->pFnSetProperty  = GUM_ObjectViewer_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty     = GUM_ObjectViewer_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)->pFnInstantiated = GUM_ObjectViewer_Object_instantiated_;

    GUM_WIDGET_CLASS(pClass)->pFnDeactivate = GUM_ObjectViewer_Widget_deactivate_;
    GUM_WIDGET_CLASS(pClass)->pFnUpdate     = GUM_ObjectViewer_update_;

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_ObjectViewer_setObject(GUM_ObjectViewer* pSelf, GblObject* pObject) {
    if GBL_UNLIKELY (!pObject)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    pSelf->pObject = pObject;
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
                                                       .pFnInstanceInit = GUM_ObjectViewer_init_},
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

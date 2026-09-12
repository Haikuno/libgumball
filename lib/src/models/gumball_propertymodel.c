#include <gumball/models/gumball_propertymodel.h>

#include <gimbal/gimbal_containers.h>
#include <gimbal/strings/gimbal_string_buffer.h>

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#define GUM_PROPERTYMODEL_(self) (GBL_PRIVATE(GUM_PropertyModel, self))

typedef struct GUM_PropertyModel_ {
    GblObject*   pObject;
    GblArrayList properties;
    GblArrayList hidden;
} GUM_PropertyModel_;

static const GblProperty* GUM_PropertyModel_effectiveProperty_(const GblObject* pObject,
                                                               const GblProperty* pDeclared) {
    if (!pObject || !pDeclared)
        return nullptr;

    const GblProperty* pProperty = GblProperty_findQuark(GBL_TYPEOF(pObject),
                                                          pDeclared->name);
    return pProperty && (pProperty->flags & GBL_PROPERTY_FLAG_READ) ? pProperty
                                                                    : nullptr;
}

static size_t GUM_PropertyModel_hiddenIndex_(const GblArrayList* pHidden,
                                             GblQuark name) {
    const size_t count = GblArrayList_size(pHidden);
    for (size_t i = 0; i < count; ++i)
        if (*(const GblQuark*)GblArrayList_at(pHidden, i) == name)
            return i;
    return SIZE_MAX;
}

static GblBool GUM_PropertyModel_hidden_(const GblArrayList* pHidden,
                                         GblQuark name) {
    return GUM_PropertyModel_hiddenIndex_(pHidden, name) != SIZE_MAX;
}

static GblBool GUM_PropertyModel_propertyInList_(const GblArrayList* pProperties,
                                                  GblQuark name) {
    const size_t count = GblArrayList_size(pProperties);
    for (size_t i = 0; i < count; ++i) {
        const GblProperty* pProperty = *(const GblProperty* const*)GblArrayList_at(pProperties, i);
        if (pProperty->name == name)
            return GBL_TRUE;
    }
    return GBL_FALSE;
}

static GBL_RESULT GUM_PropertyModel_buildProperties_(const GblObject* pObject,
                                                      const GblArrayList* pHidden,
                                                      GblArrayList* pProperties) {
    GBL_RESULT result = GblArrayList_construct(pProperties, sizeof(const GblProperty*));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result) || !pObject)
        return result;

    const GblType objectType = GBL_TYPEOF(pObject);
    const size_t depth = GblType_depth(objectType);

    for (size_t d = 0; d <= depth; ++d) {
        const GblType type = GblType_base(objectType, d);
        const GblProperty* pDeclared = nullptr;

        while ((pDeclared = GblProperty_next(type,
                                             pDeclared,
                                             GBL_PROPERTY_FLAG_READ))) {
            const GblProperty* pProperty = GUM_PropertyModel_effectiveProperty_(pObject,
                                                                                pDeclared);
            if (!pProperty ||
                GUM_PropertyModel_hidden_(pHidden, pProperty->name) ||
                GUM_PropertyModel_propertyInList_(pProperties, pProperty->name)) {
                continue;
            }

            result = GblArrayList_pushBack(pProperties, &pProperty);
            if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
                GblArrayList_destruct(pProperties);
                return result;
            }
        }
    }

    return GBL_RESULT_SUCCESS;
}

static const GblProperty* GUM_PropertyModel_propertyAt_(const GUM_PropertyModel* pSelf,
                                                        size_t row) {
    const GblArrayList* pProperties = &GUM_PROPERTYMODEL_(pSelf)->properties;
    return row < GblArrayList_size(pProperties)
         ? *(const GblProperty* const*)GblArrayList_at(pProperties, row)
         : nullptr;
}

static const GblProperty* GUM_PropertyModel_propertyNamed_(const GUM_PropertyModel* pSelf,
                                                           GblQuark name) {
    const GblArrayList* pProperties = &GUM_PROPERTYMODEL_(pSelf)->properties;
    const size_t count = GblArrayList_size(pProperties);

    for (size_t i = 0; i < count; ++i) {
        const GblProperty* pProperty = *(const GblProperty* const*)GblArrayList_at(pProperties, i);
        if (pProperty->name == name)
            return pProperty;
    }
    return nullptr;
}

static GblBool GUM_PropertyModel_colorProperty_(const GblProperty* pProperty) {
    if (!pProperty || pProperty->valueType != GBL_UINT32_TYPE)
        return GBL_FALSE;

    const char* pName = GblProperty_name(pProperty);
    const size_t length = pName ? strlen(pName) : 0;
    return length >= 5 &&
           (!strcmp(pName + length - 5, "color") ||
            !strcmp(pName + length - 5, "Color"));
}

static size_t GUM_PropertyModel_columnCount_(const GUM_IItemModel* pModel,
                                             GUM_ModelIndex parent) {
    GBL_UNUSED(pModel);
    return GUM_ModelIndex_valid(parent) ? 0 : 2;
}

static GBL_RESULT GUM_PropertyModel_childrenDisplay_(const GblObject* pObject,
                                                     GblVariant* pValue) {
    GblStringBuffer buffer;
    GBL_RESULT result = GblStringBuffer_construct(&buffer);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    result = GblStringBuffer_append(&buffer, "[");
    size_t childIndex = 0;
    if (GBL_RESULT_SUCCESS(result)) {
        GblObject_foreachChild((GblObject*)pObject, pChild) {
            if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                break;

            if (childIndex++)
                result = GblStringBuffer_append(&buffer, ", ");

            if (GBL_RESULT_SUCCESS(result)) {
                const char* pChildName = GblObject_name(pChild);
                result = GblStringBuffer_append(&buffer,
                                                pChildName && pChildName[0]
                                              ? pChildName
                                              : GblType_name(GBL_TYPEOF(pChild)));
            }
        }
    }

    if (GBL_RESULT_SUCCESS(result))
        result = GblStringBuffer_append(&buffer, "]");
    if (GBL_RESULT_SUCCESS(result))
        result = GblVariant_setString(pValue, GblStringBuffer_cString(&buffer));

    GblStringBuffer_destruct(&buffer);
    return result;
}

static size_t GUM_PropertyModel_rowCount_(const GUM_IItemModel* pModel,
                                          GUM_ModelIndex parent) {
    return GUM_ModelIndex_valid(parent)
         ? 0
         : GblArrayList_size(&GUM_PROPERTYMODEL_(GUM_PROPERTYMODEL(pModel))->properties);
}

static GUM_ModelIndex GUM_PropertyModel_index_(const GUM_IItemModel* pModel,
                                               size_t row,
                                               size_t column,
                                               GUM_ModelIndex parent) {
    if (GUM_ModelIndex_valid(parent) || column >= 2)
        return GUM_MODEL_INDEX_INVALID;

    const GblProperty* pProperty = GUM_PropertyModel_propertyAt_(GUM_PROPERTYMODEL(pModel), row);
    return pProperty ? (GUM_ModelIndex){ .pModel = pModel,
                                        .pHandle = pProperty,
                                        .column = column }
                     : GUM_MODEL_INDEX_INVALID;
}

static GUM_ModelIndex GUM_PropertyModel_parent_(const GUM_IItemModel* pModel,
                                                GUM_ModelIndex index) {
    GBL_UNUSED(pModel, index);
    return GUM_MODEL_INDEX_INVALID;
}

static GBL_RESULT GUM_PropertyModel_data_(const GUM_IItemModel* pModel,
                                          GUM_ModelIndex index,
                                          GblVariant* pValue) {
    const GUM_PropertyModel_* pSelf_ = GUM_PROPERTYMODEL_(GUM_PROPERTYMODEL(pModel));
    if GBL_UNLIKELY (!pSelf_->pObject || index.column >= 2)
        return GBL_RESULT_ERROR_INVALID_ARG;

    const GblProperty* pProperty = (const GblProperty*)index.pHandle;
    if (index.column == 0)
        return GblVariant_setString(pValue, GblProperty_name(pProperty));
    return GblObject_propertyVariantByQuark(pSelf_->pObject, pProperty->name, pValue);
}

static GBL_RESULT GUM_PropertyModel_displayData_(const GUM_IItemModel* pModel,
                                                 GUM_ModelIndex index,
                                                 GblVariant* pValue) {
    const GUM_PropertyModel_* pSelf_ = GUM_PROPERTYMODEL_(GUM_PROPERTYMODEL(pModel));
    if GBL_UNLIKELY (!pSelf_->pObject || index.column >= 2)
        return GBL_RESULT_ERROR_INVALID_ARG;

    const GblProperty* pProperty = (const GblProperty*)index.pHandle;
    const char* pName = GblProperty_name(pProperty);

    if (index.column == 0)
        return GblVariant_setString(pValue, pName);

    if (pName && !strcmp(pName, "name")) {
        const char* pObjectName = GblObject_name(pSelf_->pObject);
        return GblVariant_setString(pValue,
                                    pObjectName && pObjectName[0]
                                  ? pObjectName
                                  : GblType_name(GBL_TYPEOF(pSelf_->pObject)));
    }

    if (pName && !strcmp(pName, "children"))
        return GUM_PropertyModel_childrenDisplay_(pSelf_->pObject, pValue);

    GBL_RESULT result = GblObject_propertyVariantByQuark(pSelf_->pObject,
                                                          pProperty->name,
                                                          pValue);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    if (GblVariant_typeOf(pValue) == GBL_INVALID_TYPE)
        return GblVariant_setString(pValue, "(null)");

    if (GUM_PropertyModel_colorProperty_(pProperty)) {
        char value[11];
        snprintf(value, sizeof(value), "0x%08" PRIX32, GblVariant_uint32(pValue));
        return GblVariant_setString(pValue, value);
    }

    if (GblType_check(GblVariant_typeOf(pValue), GBL_OBJECT_TYPE)) {
        const GblObject* pObject = GblVariant_objectPeek(pValue);
        if (!pObject)
            return GblVariant_setString(pValue, "(null)");

        const char* pObjectName = GblObject_name(pObject);
        return GblVariant_setString(pValue,
                                    pObjectName && pObjectName[0]
                                  ? pObjectName
                                  : GblType_name(GBL_TYPEOF(pObject)));
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_PropertyModel_setData_(GUM_IItemModel* pModel,
                                             GUM_ModelIndex index,
                                             GblVariant* pValue) {
    GUM_PropertyModel_* pSelf_ = GUM_PROPERTYMODEL_(GUM_PROPERTYMODEL(pModel));
    if GBL_UNLIKELY (!pSelf_->pObject || index.column != 1)
        return GBL_RESULT_ERROR_INVALID_ARG;

    const GblProperty* pProperty = (const GblProperty*)index.pHandle;
    if (!(pProperty->flags & GBL_PROPERTY_FLAG_WRITE))
        return GBL_RESULT_ERROR_INVALID_OPERATION;

    return GblObject_setPropertyVariantByQuark(pSelf_->pObject, pProperty->name, pValue);
}

static GblFlags GUM_PropertyModel_flags_(const GUM_IItemModel* pModel,
                                         GUM_ModelIndex index) {
    GBL_UNUSED(pModel);
    if (index.column >= 2)
        return GUM_ITEM_NONE;

    GblFlags flags = GUM_ITEM_SELECTABLE;
    const GblProperty* pProperty = (const GblProperty*)index.pHandle;
    if (index.column == 1 && (pProperty->flags & GBL_PROPERTY_FLAG_WRITE))
        flags |= GUM_ITEM_EDITABLE;
    return flags;
}

static void GUM_PropertyModel_propertyChange_(GblInstance* pReceiver,
                                              GblBox* pPropertyBox) {
    GUM_PropertyModel* pSelf = GUM_PROPERTYMODEL(pReceiver);
    const GblObject* pObject = GUM_PROPERTYMODEL_(pSelf)->pObject;
    const GblProperty* pProperty = GUM_PropertyModel_effectiveProperty_(
        pObject,
        (const GblProperty*)pPropertyBox);
    if (!pProperty)
        return;

    pProperty = GUM_PropertyModel_propertyNamed_(pSelf, pProperty->name);
    if (!pProperty)
        return;

    GUM_IItemModel_emitDataChanged(GUM_IITEMMODEL(pSelf),
                                   (GUM_ModelIndex){
                                       .pModel = GUM_IITEMMODEL(pSelf),
                                       .pHandle = pProperty,
                                       .column = 1
                                   });
}

static void GUM_PropertyModel_disconnect_(GUM_PropertyModel* pSelf, GblObject* pObject) {
    if (pObject)
        GblSignal_disconnect(GBL_INSTANCE(pObject),
                             "propertyChange",
                             GBL_INSTANCE(pSelf),
                             nullptr);
}

static GBL_RESULT GUM_PropertyModel_init_(GblInstance* pInstance) {
    GUM_PropertyModel* pSelf = GUM_PROPERTYMODEL(pInstance);
    GUM_PropertyModel_* pSelf_ = GUM_PROPERTYMODEL_(pSelf);

    GBL_RESULT result = GblArrayList_construct(&pSelf_->properties,
                                               sizeof(const GblProperty*));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    result = GblArrayList_construct(&pSelf_->hidden, sizeof(GblQuark));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GblArrayList_destruct(&pSelf_->properties);
        return result;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_PropertyModel_GblBox_destructor_(GblBox* pBox) {
    GUM_PropertyModel* pSelf = GUM_PROPERTYMODEL(pBox);
    GUM_PropertyModel_* pSelf_ = GUM_PROPERTYMODEL_(pSelf);

    if (pSelf_->pObject) {
        GUM_PropertyModel_disconnect_(pSelf, pSelf_->pObject);
        GblBox_unref(GBL_BOX(pSelf_->pObject));
        pSelf_->pObject = nullptr;
    }

    GblArrayList_destruct(&pSelf_->hidden);
    GblArrayList_destruct(&pSelf_->properties);
    return GBL_BOX_CLASS(GblClass_weakRefDefault(GBL_BOX_TYPE))->pFnDestructor(pBox);
}

static GBL_RESULT GUM_PropertyModelClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GBL_BOX_CLASS(pClass)->pFnDestructor = GUM_PropertyModel_GblBox_destructor_;

    GUM_IItemModelClass* pModel = GUM_IITEMMODEL_CLASS(pClass);
    pModel->pFnColumnCount = GUM_PropertyModel_columnCount_;
    pModel->pFnRowCount    = GUM_PropertyModel_rowCount_;
    pModel->pFnIndex       = GUM_PropertyModel_index_;
    pModel->pFnParent      = GUM_PropertyModel_parent_;
    pModel->pFnData        = GUM_PropertyModel_data_;
    pModel->pFnDisplayData = GUM_PropertyModel_displayData_;
    pModel->pFnSetData     = GUM_PropertyModel_setData_;
    pModel->pFnFlags       = GUM_PropertyModel_flags_;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblObject* GUM_PropertyModel_object(const GUM_PropertyModel* pSelf) {
    return pSelf ? GUM_PROPERTYMODEL_(pSelf)->pObject : nullptr;
}

GBL_EXPORT GBL_RESULT GUM_PropertyModel_setObject(GUM_PropertyModel* pSelf,
                                                  GblObject* pObject) {
    if (!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_PropertyModel_* pSelf_ = GUM_PROPERTYMODEL_(pSelf);
    if (pSelf_->pObject == pObject)
        return GBL_RESULT_SUCCESS;

    GblArrayList properties;
    GBL_RESULT result = GUM_PropertyModel_buildProperties_(pObject,
                                                           &pSelf_->hidden,
                                                           &properties);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    if (pObject) {
        GblBox_ref(GBL_BOX(pObject));
        result = GblSignal_connect(GBL_INSTANCE(pObject),
                                   "propertyChange",
                                   GBL_INSTANCE(pSelf),
                                   (GblFnPtr)GUM_PropertyModel_propertyChange_,
                                   nullptr);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            GblBox_unref(GBL_BOX(pObject));
            GblArrayList_destruct(&properties);
            return result;
        }
    }

    GblObject* pOldObject = pSelf_->pObject;
    if (pOldObject)
        GUM_PropertyModel_disconnect_(pSelf, pOldObject);

    GblArrayList oldProperties = pSelf_->properties;
    pSelf_->pObject = pObject;
    pSelf_->properties = properties;

    GblArrayList_destruct(&oldProperties);
    if (pOldObject)
        GblBox_unref(GBL_BOX(pOldObject));

    return GUM_IItemModel_emitStructureChanged(GUM_IITEMMODEL(pSelf));
}

GBL_EXPORT GblBool GUM_PropertyModel_propertyVisible(const GUM_PropertyModel* pSelf,
                                                     const char* pName) {
    if (!pSelf || !pName || !pName[0])
        return GBL_FALSE;
    return !GUM_PropertyModel_hidden_(&GUM_PROPERTYMODEL_(pSelf)->hidden,
                                      GblQuark_fromString(pName));
}

GBL_EXPORT GBL_RESULT GUM_PropertyModel_setPropertyVisible(GUM_PropertyModel* pSelf,
                                                           const char* pName,
                                                           GblBool visible) {
    if (!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (!pName || !pName[0])
        return GBL_RESULT_ERROR_INVALID_ARG;

    GUM_PropertyModel_* pSelf_ = GUM_PROPERTYMODEL_(pSelf);
    const GblQuark name = GblQuark_fromString(pName);
    const size_t hiddenIndex = GUM_PropertyModel_hiddenIndex_(&pSelf_->hidden, name);
    const GblBool currentlyVisible = hiddenIndex == SIZE_MAX;
    if (!!visible == !!currentlyVisible)
        return GBL_RESULT_SUCCESS;

    GblArrayList hidden;
    GBL_RESULT result = GblArrayList_construct(&hidden,
                                               sizeof(GblQuark),
                                               GblArrayList_size(&pSelf_->hidden),
                                               GblArrayList_data(&pSelf_->hidden));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    if (visible)
        result = GblArrayList_erase(&hidden, hiddenIndex, 1);
    else
        result = GblArrayList_pushBack(&hidden, &name);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GblArrayList_destruct(&hidden);
        return result;
    }

    GblArrayList properties;
    result = GUM_PropertyModel_buildProperties_(pSelf_->pObject,
                                                &hidden,
                                                &properties);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GblArrayList_destruct(&hidden);
        return result;
    }

    const size_t oldCount = GblArrayList_size(&pSelf_->properties);
    const size_t newCount = GblArrayList_size(&properties);
    GblArrayList oldHidden = pSelf_->hidden;
    GblArrayList oldProperties = pSelf_->properties;
    pSelf_->hidden = hidden;
    pSelf_->properties = properties;
    GblArrayList_destruct(&oldHidden);
    GblArrayList_destruct(&oldProperties);

    return oldCount != newCount
         ? GUM_IItemModel_emitStructureChanged(GUM_IITEMMODEL(pSelf))
         : GBL_RESULT_SUCCESS;
}

GBL_EXPORT GUM_PropertyModel* GUM_PropertyModel_create(GblObject* pObject) {
    GUM_PropertyModel* pSelf = GUM_PROPERTYMODEL(
        GblBox_create(GUM_PROPERTYMODEL_TYPE,
                      sizeof(GUM_PropertyModel),
                      nullptr,
                      nullptr,
                      nullptr));
    if (!pSelf)
        return nullptr;

    if (pObject && !GBL_RESULT_SUCCESS(GUM_PropertyModel_setObject(pSelf, pObject))) {
        GblBox_unref(GBL_BOX(pSelf));
        return nullptr;
    }
    return pSelf;
}

GblType GUM_PropertyModel_type(void) {
    static GblType type = GBL_INVALID_TYPE;
    static GblInterfaceImpl interfaces[1];

    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        interfaces[0] = (GblInterfaceImpl){
            .interfaceType = GUM_IITEMMODEL_TYPE,
            .classOffset = offsetof(GUM_PropertyModelClass, GUM_IItemModelImpl)
        };
        type = GblType_register(GblQuark_internStatic("GUM_PropertyModel"),
                                GBL_BOX_TYPE,
                                &(static GblTypeInfo){
                                    .classSize           = sizeof(GUM_PropertyModelClass),
                                    .pFnClassInit        = GUM_PropertyModelClass_init_,
                                    .instanceSize        = sizeof(GUM_PropertyModel),
                                    .instancePrivateSize = sizeof(GUM_PropertyModel_),
                                    .pFnInstanceInit     = GUM_PropertyModel_init_,
                                    .pInterfaceImpls     = interfaces,
                                    .interfaceCount      = GBL_COUNT_OF(interfaces)
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}

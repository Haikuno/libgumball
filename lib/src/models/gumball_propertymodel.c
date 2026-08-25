#include <gumball/models/gumball_propertymodel.h>

#define GUM_PROPERTYMODEL_(self) (GBL_PRIVATE(GUM_PropertyModel, self))

typedef struct GUM_PropertyModel_ {
    GblObject* pObject;
} GUM_PropertyModel_;

static const GblProperty* GUM_PropertyModel_effectiveProperty_(
    const GUM_PropertyModel* pSelf,
    const GblProperty* pDeclared) {
    const GblObject* pObject = GUM_PROPERTYMODEL_(pSelf)->pObject;
    if (!pObject || !pDeclared)
        return nullptr;

    const GblProperty* pProperty = GblProperty_findQuark(GBL_TYPEOF(pObject),
                                                          pDeclared->name);
    return pProperty && (pProperty->flags & GBL_PROPERTY_FLAG_READ) ? pProperty
                                                                    : nullptr;
}

static const GblProperty* GUM_PropertyModel_propertyAt_(const GUM_PropertyModel* pSelf,
                                                        size_t row) {
    const GblObject* pObject = GUM_PROPERTYMODEL_(pSelf)->pObject;
    if (!pObject)
        return nullptr;

    const GblType objectType = GBL_TYPEOF(pObject);
    const size_t depth = GblType_depth(objectType);
    size_t current = 0;

    for (size_t d = 0; d <= depth; ++d) {
        const GblType type = GblType_base(objectType, d);
        const GblProperty* pDeclared = nullptr;
        while ((pDeclared = GblProperty_next(type,
                                             pDeclared,
                                             GBL_PROPERTY_FLAG_READ))) {
            const GblProperty* pProperty = GUM_PropertyModel_effectiveProperty_(pSelf,
                                                                                pDeclared);
            if (!pProperty)
                continue;
            if (current++ == row)
                return pProperty;
        }
    }
    return nullptr;
}

static size_t GUM_PropertyModel_readableCount_(const GUM_PropertyModel* pSelf) {
    const GblObject* pObject = GUM_PROPERTYMODEL_(pSelf)->pObject;
    if (!pObject)
        return 0;

    const GblType objectType = GBL_TYPEOF(pObject);
    const size_t depth = GblType_depth(objectType);
    size_t count = 0;

    for (size_t d = 0; d <= depth; ++d) {
        const GblType type = GblType_base(objectType, d);
        const GblProperty* pDeclared = nullptr;
        while ((pDeclared = GblProperty_next(type,
                                             pDeclared,
                                             GBL_PROPERTY_FLAG_READ))) {
            if (GUM_PropertyModel_effectiveProperty_(pSelf, pDeclared))
                ++count;
        }
    }
    return count;
}

static size_t GUM_PropertyModel_columnCount_(const GUM_IItemModel* pModel,
                                             GUM_ModelIndex parent) {
    GBL_UNUSED(pModel);
    return GUM_ModelIndex_valid(parent) ? 0 : 2;
}

static size_t GUM_PropertyModel_rowCount_(const GUM_IItemModel* pModel,
                                          GUM_ModelIndex parent) {
    return GUM_ModelIndex_valid(parent)
         ? 0
         : GUM_PropertyModel_readableCount_(GUM_PROPERTYMODEL(pModel));
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
    const GblProperty* pProperty = GUM_PropertyModel_effectiveProperty_(
        pSelf,
        (const GblProperty*)pPropertyBox);
    if (!pProperty)
        return;

    const GUM_ModelIndex index = {
        .pModel = GUM_IITEMMODEL(pSelf),
        .pHandle = pProperty,
        .column = 1
    };
    GUM_IItemModel_emitDataChanged(GUM_IITEMMODEL(pSelf), index);
}

static void GUM_PropertyModel_disconnect_(GUM_PropertyModel* pSelf, GblObject* pObject) {
    if (pObject)
        GblSignal_disconnect(GBL_INSTANCE(pObject),
                             "propertyChange",
                             GBL_INSTANCE(pSelf),
                             nullptr);
}

static GBL_RESULT GUM_PropertyModel_GblBox_destructor_(GblBox* pBox) {
    GUM_PropertyModel* pSelf = GUM_PROPERTYMODEL(pBox);
    GUM_PropertyModel_* pSelf_ = GUM_PROPERTYMODEL_(pSelf);

    if (pSelf_->pObject) {
        GUM_PropertyModel_disconnect_(pSelf, pSelf_->pObject);
        GblBox_unref(GBL_BOX(pSelf_->pObject));
        pSelf_->pObject = nullptr;
    }

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

    if (pObject) {
        GblBox_ref(GBL_BOX(pObject));
        const GBL_RESULT result = GblSignal_connect(GBL_INSTANCE(pObject),
                                                    "propertyChange",
                                                    GBL_INSTANCE(pSelf),
                                                    (GblFnPtr)GUM_PropertyModel_propertyChange_,
                                                    nullptr);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            GblBox_unref(GBL_BOX(pObject));
            return result;
        }
    }

    GblObject* pOldObject = pSelf_->pObject;
    if (pOldObject)
        GUM_PropertyModel_disconnect_(pSelf, pOldObject);
    pSelf_->pObject = pObject;
    if (pOldObject)
        GblBox_unref(GBL_BOX(pOldObject));

    return GUM_IItemModel_emitStructureChanged(GUM_IITEMMODEL(pSelf));
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
                                    .pInterfaceImpls     = interfaces,
                                    .interfaceCount      = GBL_COUNT_OF(interfaces)
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}

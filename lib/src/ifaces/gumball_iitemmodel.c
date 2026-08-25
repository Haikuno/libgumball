#include <gumball/ifaces/gumball_iitemmodel.h>

#include "../core/gumball_marshal_.h"

static bool GUM_IItemModel_parentAccepted_(const GUM_IItemModel* pSelf,
                                           GUM_ModelIndex parent) {
    return !GUM_ModelIndex_valid(parent) || GUM_ModelIndex_belongs(parent, pSelf);
}

GBL_EXPORT GUM_IItemModel* GUM_IItemModel_ref(GUM_IItemModel* pSelf) {
    return pSelf ? GUM_IITEMMODEL(GBL_REF(pSelf)) : nullptr;
}

GBL_EXPORT GblRefCount GUM_IItemModel_unref(GUM_IItemModel* pSelf) {
    return pSelf ? GBL_UNREF(pSelf) : 0;
}

GBL_EXPORT GblBool GUM_ModelIndex_valid(GUM_ModelIndex index) {
    return index.pModel && index.pHandle;
}

GBL_EXPORT GblBool GUM_ModelIndex_equal(GUM_ModelIndex lhs, GUM_ModelIndex rhs) {
    return lhs.pModel == rhs.pModel &&
           lhs.pHandle == rhs.pHandle &&
           lhs.column == rhs.column;
}

GBL_EXPORT GblBool GUM_ModelIndex_belongs(GUM_ModelIndex index,
                                          const GUM_IItemModel* pSelf) {
    return GUM_ModelIndex_valid(index) && index.pModel == pSelf;
}

GBL_EXPORT size_t GUM_IItemModel_columnCount(const GUM_IItemModel* pSelf,
                                             GUM_ModelIndex parent) {
    if (!pSelf || !GUM_IItemModel_parentAccepted_(pSelf, parent))
        return 0;
    return GUM_IITEMMODEL_CLASSOF(pSelf)->pFnColumnCount(pSelf, parent);
}

GBL_EXPORT size_t GUM_IItemModel_rowCount(const GUM_IItemModel* pSelf,
                                          GUM_ModelIndex parent) {
    if (!pSelf || !GUM_IItemModel_parentAccepted_(pSelf, parent))
        return 0;
    return GUM_IITEMMODEL_CLASSOF(pSelf)->pFnRowCount(pSelf, parent);
}

GBL_EXPORT GUM_ModelIndex GUM_IItemModel_index(const GUM_IItemModel* pSelf,
                                               size_t row,
                                               size_t column,
                                               GUM_ModelIndex parent) {
    if (!pSelf || !GUM_IItemModel_parentAccepted_(pSelf, parent))
        return GUM_MODEL_INDEX_INVALID;
    return GUM_IITEMMODEL_CLASSOF(pSelf)->pFnIndex(pSelf, row, column, parent);
}

GBL_EXPORT GUM_ModelIndex GUM_IItemModel_parent(const GUM_IItemModel* pSelf,
                                                GUM_ModelIndex index) {
    if (!pSelf || !GUM_ModelIndex_belongs(index, pSelf))
        return GUM_MODEL_INDEX_INVALID;
    return GUM_IITEMMODEL_CLASSOF(pSelf)->pFnParent(pSelf, index);
}

GBL_EXPORT GBL_RESULT GUM_IItemModel_data(const GUM_IItemModel* pSelf,
                                          GUM_ModelIndex index,
                                          GblVariant* pValue) {
    if (!pSelf || !pValue)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (!GUM_ModelIndex_belongs(index, pSelf))
        return GBL_RESULT_ERROR_INVALID_ARG;
    return GUM_IITEMMODEL_CLASSOF(pSelf)->pFnData(pSelf, index, pValue);
}

GBL_EXPORT GBL_RESULT GUM_IItemModel_displayData(const GUM_IItemModel* pSelf,
                                                 GUM_ModelIndex index,
                                                 GblVariant* pValue) {
    if (!pSelf || !pValue)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (!GUM_ModelIndex_belongs(index, pSelf))
        return GBL_RESULT_ERROR_INVALID_ARG;

    GUM_IItemModelClass* pClass = GUM_IITEMMODEL_CLASSOF(pSelf);
    return pClass->pFnDisplayData ? pClass->pFnDisplayData(pSelf, index, pValue)
                                  : pClass->pFnData(pSelf, index, pValue);
}

GBL_EXPORT GBL_RESULT GUM_IItemModel_setData(GUM_IItemModel* pSelf,
                                             GUM_ModelIndex index,
                                             GblVariant* pValue) {
    if (!pSelf || !pValue)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (!GUM_ModelIndex_belongs(index, pSelf))
        return GBL_RESULT_ERROR_INVALID_ARG;
    return GUM_IITEMMODEL_CLASSOF(pSelf)->pFnSetData(pSelf, index, pValue);
}

GBL_EXPORT GblFlags GUM_IItemModel_flags(const GUM_IItemModel* pSelf,
                                         GUM_ModelIndex index) {
    if (!pSelf || !GUM_ModelIndex_belongs(index, pSelf))
        return GUM_ITEM_NONE;
    return GUM_IITEMMODEL_CLASSOF(pSelf)->pFnFlags(pSelf, index);
}

GBL_EXPORT GBL_RESULT GUM_IItemModel_emitDataChanged(GUM_IItemModel* pSelf,
                                                     GUM_ModelIndex index) {
    if (!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (!GUM_ModelIndex_belongs(index, pSelf))
        return GBL_RESULT_ERROR_INVALID_ARG;
    return GBL_EMIT(pSelf, "dataChanged", (void*)index.pHandle, index.column);
}

GBL_EXPORT GBL_RESULT GUM_IItemModel_emitStructureChanged(GUM_IItemModel* pSelf) {
    return pSelf ? GBL_EMIT(pSelf, "structureChanged")
                 : GBL_RESULT_ERROR_INVALID_POINTER;
}

static size_t GUM_IItemModel_columnCount_(const GUM_IItemModel* pSelf,
                                          GUM_ModelIndex parent) {
    GBL_UNUSED(pSelf, parent);
    return 0;
}

static size_t GUM_IItemModel_rowCount_(const GUM_IItemModel* pSelf,
                                       GUM_ModelIndex parent) {
    GBL_UNUSED(pSelf, parent);
    return 0;
}

static GUM_ModelIndex GUM_IItemModel_index_(const GUM_IItemModel* pSelf,
                                            size_t row,
                                            size_t column,
                                            GUM_ModelIndex parent) {
    GBL_UNUSED(pSelf, row, column, parent);
    return GUM_MODEL_INDEX_INVALID;
}

static GUM_ModelIndex GUM_IItemModel_parent_(const GUM_IItemModel* pSelf,
                                             GUM_ModelIndex index) {
    GBL_UNUSED(pSelf, index);
    return GUM_MODEL_INDEX_INVALID;
}

static GBL_RESULT GUM_IItemModel_data_(const GUM_IItemModel* pSelf,
                                       GUM_ModelIndex index,
                                       GblVariant* pValue) {
    GBL_UNUSED(pSelf, index, pValue);
    return GBL_RESULT_UNIMPLEMENTED;
}

static GBL_RESULT GUM_IItemModel_displayData_(const GUM_IItemModel* pSelf,
                                              GUM_ModelIndex index,
                                              GblVariant* pValue) {
    return GUM_IITEMMODEL_CLASSOF(pSelf)->pFnData(pSelf, index, pValue);
}

static GBL_RESULT GUM_IItemModel_setData_(GUM_IItemModel* pSelf,
                                          GUM_ModelIndex index,
                                          GblVariant* pValue) {
    GBL_UNUSED(pSelf, index, pValue);
    return GBL_RESULT_UNSUPPORTED;
}

static GblFlags GUM_IItemModel_flags_(const GUM_IItemModel* pSelf,
                                      GUM_ModelIndex index) {
    GBL_UNUSED(pSelf, index);
    return GUM_ITEM_NONE;
}

static GBL_RESULT GUM_IItemModelClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_IITEMMODEL_TYPE)) {
        GblSignal_install(GUM_IITEMMODEL_TYPE, "dataChanged",
                          GUM_Marshal_CClosure_VOID__INSTANCE_POINTER_SIZE_, 2,
                          GBL_POINTER_TYPE, GBL_SIZE_TYPE);
        GblSignal_install(GUM_IITEMMODEL_TYPE, "structureChanged",
                          GblMarshal_CClosure_VOID__INSTANCE, 0);
    }

    GUM_IItemModelClass* pSelf = GUM_IITEMMODEL_CLASS(pClass);
    pSelf->pFnColumnCount = GUM_IItemModel_columnCount_;
    pSelf->pFnRowCount    = GUM_IItemModel_rowCount_;
    pSelf->pFnIndex       = GUM_IItemModel_index_;
    pSelf->pFnParent      = GUM_IItemModel_parent_;
    pSelf->pFnData        = GUM_IItemModel_data_;
    pSelf->pFnDisplayData = GUM_IItemModel_displayData_;
    pSelf->pFnSetData     = GUM_IItemModel_setData_;
    pSelf->pFnFlags       = GUM_IItemModel_flags_;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_IItemModelClass_final_(GblClass* pClass, const void* pClassData) {
    GBL_UNUSED(pClassData);

    if (!GblType_classRefCount(GUM_IITEMMODEL_TYPE)) {
        GblSignal_uninstall(GUM_IITEMMODEL_TYPE, "dataChanged");
        GblSignal_uninstall(GUM_IITEMMODEL_TYPE, "structureChanged");
    }

    return GBL_RESULT_SUCCESS;
}

GblType GUM_IItemModel_type(void) {
    static GblType type = GBL_INVALID_TYPE;
    static GblType dependencies[] = { GBL_INVALID_TYPE };

    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        dependencies[0] = GBL_BOX_TYPE;
        type = GblType_register(GblQuark_internStatic("GUM_IItemModel"),
                                GBL_INTERFACE_TYPE,
                                &(static GblTypeInfo){
                                    .classSize        = sizeof(GUM_IItemModelClass),
                                    .pFnClassInit     = GUM_IItemModelClass_init_,
                                    .pFnClassFinal    = GUM_IItemModelClass_final_,
                                    .pDependencies    = dependencies,
                                    .dependencyCount = 1
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

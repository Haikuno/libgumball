#include <gumball/models/gumball_objecttreemodel.h>

#define GUM_OBJECTTREEMODEL_(self) (GBL_PRIVATE(GUM_ObjectTreeModel, self))

typedef struct GUM_ObjectTreeModel_ {
    GblObject* pRoot;
} GUM_ObjectTreeModel_;

static GUM_ModelIndex GUM_ObjectTreeModel_indexFor_(const GUM_IItemModel* pModel,
                                                    const GblObject* pObject) {
    return pObject ? (GUM_ModelIndex){ .pModel = pModel, .pHandle = pObject }
                   : GUM_MODEL_INDEX_INVALID;
}

static size_t GUM_ObjectTreeModel_columnCount_(const GUM_IItemModel* pModel,
                                               GUM_ModelIndex parent) {
    GBL_UNUSED(pModel, parent);
    return 1;
}

static size_t GUM_ObjectTreeModel_rowCount_(const GUM_IItemModel* pModel,
                                            GUM_ModelIndex parent) {
    const GUM_ObjectTreeModel* pSelf = GUM_OBJECTTREEMODEL(pModel);
    const GblObject* pRoot = GUM_OBJECTTREEMODEL_(pSelf)->pRoot;
    if (!pRoot)
        return 0;

    if (!GUM_ModelIndex_valid(parent))
        return 1;

    size_t count = 0;
    GblObject_foreachChild((GblObject*)parent.pHandle, pChild) {
        GBL_UNUSED(pChild);
        ++count;
    }
    return count;
}

static GUM_ModelIndex GUM_ObjectTreeModel_index_(const GUM_IItemModel* pModel,
                                                 size_t row,
                                                 size_t column,
                                                 GUM_ModelIndex parent) {
    if (column)
        return GUM_MODEL_INDEX_INVALID;

    const GUM_ObjectTreeModel* pSelf = GUM_OBJECTTREEMODEL(pModel);
    GblObject* pRoot = GUM_OBJECTTREEMODEL_(pSelf)->pRoot;
    if (!pRoot)
        return GUM_MODEL_INDEX_INVALID;

    if (!GUM_ModelIndex_valid(parent))
        return row == 0 ? GUM_ObjectTreeModel_indexFor_(pModel, pRoot)
                        : GUM_MODEL_INDEX_INVALID;

    size_t current = 0;
    GblObject_foreachChild((GblObject*)parent.pHandle, pChild) {
        if (current++ == row)
            return GUM_ObjectTreeModel_indexFor_(pModel, pChild);
    }
    return GUM_MODEL_INDEX_INVALID;
}

static GUM_ModelIndex GUM_ObjectTreeModel_parent_(const GUM_IItemModel* pModel,
                                                  GUM_ModelIndex index) {
    const GUM_ObjectTreeModel* pSelf = GUM_OBJECTTREEMODEL(pModel);
    GblObject* pRoot = GUM_OBJECTTREEMODEL_(pSelf)->pRoot;
    GblObject* pObject = (GblObject*)index.pHandle;
    if (!pRoot || pObject == pRoot)
        return GUM_MODEL_INDEX_INVALID;

    GblObject* pParent = GblObject_parent(pObject);
    return GUM_ObjectTreeModel_indexFor_(pModel, pParent);
}

static GBL_RESULT GUM_ObjectTreeModel_data_(const GUM_IItemModel* pModel,
                                            GUM_ModelIndex index,
                                            GblVariant* pValue) {
    GBL_UNUSED(pModel);
    GblStringRef* pName = GblObject_name((const GblObject*)index.pHandle);
    return GblVariant_setString(pValue, pName ? pName : "");
}

static GBL_RESULT GUM_ObjectTreeModel_setData_(GUM_IItemModel* pModel,
                                               GUM_ModelIndex index,
                                               GblVariant* pValue) {
    GblObject* pObject = (GblObject*)index.pHandle;
    const GBL_RESULT result = GblObject_setPropertyVariant(pObject, "name", pValue);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;
    return GUM_IItemModel_emitDataChanged(pModel, index);
}

static GblFlags GUM_ObjectTreeModel_flags_(const GUM_IItemModel* pModel,
                                           GUM_ModelIndex index) {
    GBL_UNUSED(pModel, index);
    return GUM_ITEM_SELECTABLE | GUM_ITEM_EDITABLE;
}

static GBL_RESULT GUM_ObjectTreeModel_GblBox_destructor_(GblBox* pBox) {
    GUM_ObjectTreeModel_* pSelf_ = GUM_OBJECTTREEMODEL_(GUM_OBJECTTREEMODEL(pBox));
    if (pSelf_->pRoot)
        GblBox_unref(GBL_BOX(pSelf_->pRoot));
    pSelf_->pRoot = nullptr;
    return GBL_BOX_CLASS(GblClass_weakRefDefault(GBL_BOX_TYPE))->pFnDestructor(pBox);
}

static GBL_RESULT GUM_ObjectTreeModelClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GBL_BOX_CLASS(pClass)->pFnDestructor = GUM_ObjectTreeModel_GblBox_destructor_;

    GUM_IItemModelClass* pModel = GUM_IITEMMODEL_CLASS(pClass);
    pModel->pFnColumnCount = GUM_ObjectTreeModel_columnCount_;
    pModel->pFnRowCount    = GUM_ObjectTreeModel_rowCount_;
    pModel->pFnIndex       = GUM_ObjectTreeModel_index_;
    pModel->pFnParent      = GUM_ObjectTreeModel_parent_;
    pModel->pFnData        = GUM_ObjectTreeModel_data_;
    pModel->pFnSetData     = GUM_ObjectTreeModel_setData_;
    pModel->pFnFlags       = GUM_ObjectTreeModel_flags_;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblObject* GUM_ObjectTreeModel_root(const GUM_ObjectTreeModel* pSelf) {
    return pSelf ? GUM_OBJECTTREEMODEL_(pSelf)->pRoot : nullptr;
}

GBL_EXPORT GBL_RESULT GUM_ObjectTreeModel_setRoot(GUM_ObjectTreeModel* pSelf,
                                                  GblObject* pRoot) {
    if (!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_ObjectTreeModel_* pSelf_ = GUM_OBJECTTREEMODEL_(pSelf);
    if (pSelf_->pRoot == pRoot)
        return GBL_RESULT_SUCCESS;

    if (pRoot)
        GblBox_ref(GBL_BOX(pRoot));
    GblObject* pOldRoot = pSelf_->pRoot;
    pSelf_->pRoot = pRoot;
    if (pOldRoot)
        GblBox_unref(GBL_BOX(pOldRoot));

    return GUM_IItemModel_emitStructureChanged(GUM_IITEMMODEL(pSelf));
}

GBL_EXPORT GBL_RESULT GUM_ObjectTreeModel_refresh(GUM_ObjectTreeModel* pSelf) {
    return pSelf ? GUM_IItemModel_emitStructureChanged(GUM_IITEMMODEL(pSelf))
                 : GBL_RESULT_ERROR_INVALID_POINTER;
}

GBL_EXPORT GUM_ObjectTreeModel* GUM_ObjectTreeModel_create(GblObject* pRoot) {
    GUM_ObjectTreeModel* pSelf = GUM_OBJECTTREEMODEL(
        GblBox_create(GUM_OBJECTTREEMODEL_TYPE,
                      sizeof(GUM_ObjectTreeModel),
                      nullptr,
                      nullptr,
                      nullptr));
    if (!pSelf)
        return nullptr;

    if (pRoot && !GBL_RESULT_SUCCESS(GUM_ObjectTreeModel_setRoot(pSelf, pRoot))) {
        GblBox_unref(GBL_BOX(pSelf));
        return nullptr;
    }
    return pSelf;
}

GblType GUM_ObjectTreeModel_type(void) {
    static GblType type = GBL_INVALID_TYPE;
    static GblInterfaceImpl interfaces[1];

    if GBL_UNLIKELY (type == GBL_INVALID_TYPE) {
        interfaces[0] = (GblInterfaceImpl){
            .interfaceType = GUM_IITEMMODEL_TYPE,
            .classOffset = offsetof(GUM_ObjectTreeModelClass, GUM_IItemModelImpl)
        };
        type = GblType_register(GblQuark_internStatic("GUM_ObjectTreeModel"),
                                GBL_BOX_TYPE,
                                &(static GblTypeInfo){
                                    .classSize           = sizeof(GUM_ObjectTreeModelClass),
                                    .pFnClassInit        = GUM_ObjectTreeModelClass_init_,
                                    .instanceSize        = sizeof(GUM_ObjectTreeModel),
                                    .instancePrivateSize = sizeof(GUM_ObjectTreeModel_),
                                    .pInterfaceImpls     = interfaces,
                                    .interfaceCount      = GBL_COUNT_OF(interfaces)
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }
    return type;
}

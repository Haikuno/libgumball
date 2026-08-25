#ifndef GUM_IITEMMODEL_H
#define GUM_IITEMMODEL_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__iitemmodel_8h.html

/*!  \file
 *   \ref     GUM_IItemModel "GUM_IItemModel interface"
 *   \ingroup ifaces
 *
 *   Generic item model used by Tree and Table.
 *   Model indices are borrowed and become invalid after structureChanged.
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gimbal/gimbal_meta.h>

#define GUM_IITEMMODEL_TYPE              (GBL_TYPEID     (GUM_IItemModel))
#define GUM_IITEMMODEL(instance)         (GBL_CAST       (GUM_IItemModel, instance))
#define GUM_IITEMMODEL_CLASS(klass)      (GBL_CLASS_CAST (GUM_IItemModel, klass))
#define GUM_IITEMMODEL_CLASSOF(instance) (GBL_CLASSOF    (GUM_IItemModel, instance))

#define GBL_SELF_TYPE GUM_IItemModel

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_IItemModel);

typedef struct GUM_ModelIndex {
    const GUM_IItemModel* pModel;  //!< Model this index belongs to.
    const void*           pHandle; //!< Model-owned row handle.
    size_t                column;  //!< Column inside the row.
} GUM_ModelIndex;

#define GUM_MODEL_INDEX_INVALID ((GUM_ModelIndex){ 0 })

typedef enum GUM_ItemFlag {
    GUM_ITEM_NONE       = 0,
    GUM_ITEM_SELECTABLE = 1u << 0,
    GUM_ITEM_EDITABLE   = 1u << 1
} GUM_ItemFlag;

GBL_INTERFACE_DERIVE(GUM_IItemModel)
    size_t         (*pFnColumnCount)(GBL_CSELF, GUM_ModelIndex parent);
    size_t         (*pFnRowCount)   (GBL_CSELF, GUM_ModelIndex parent);
    GUM_ModelIndex (*pFnIndex)      (GBL_CSELF, size_t row, size_t column, GUM_ModelIndex parent);
    GUM_ModelIndex (*pFnParent)     (GBL_CSELF, GUM_ModelIndex index);
    GBL_RESULT     (*pFnData)       (GBL_CSELF, GUM_ModelIndex index, GblVariant* pValue);
    GBL_RESULT     (*pFnSetData)    (GBL_SELF, GUM_ModelIndex index, GblVariant* pValue);
    GblFlags       (*pFnFlags)      (GBL_CSELF, GUM_ModelIndex index);
GBL_INTERFACE_END

GBL_SIGNALS(GUM_IItemModel,
    (dataChanged,      (GBL_INSTANCE_TYPE, pReceiver), (GBL_POINTER_TYPE, pHandle), (GBL_SIZE_TYPE, column)),
    (structureChanged, (GBL_INSTANCE_TYPE, pReceiver))
)

GBL_EXPORT GblType         GUM_IItemModel_type                 (void) GBL_NOEXCEPT;
GBL_EXPORT GUM_IItemModel* GUM_IItemModel_ref                  (GBL_SELF) GBL_NOEXCEPT;
GBL_EXPORT GblRefCount     GUM_IItemModel_unref                (GBL_SELF) GBL_NOEXCEPT;

GBL_EXPORT GblBool         GUM_ModelIndex_valid                (GUM_ModelIndex index) GBL_NOEXCEPT;
GBL_EXPORT GblBool         GUM_ModelIndex_equal                (GUM_ModelIndex lhs, GUM_ModelIndex rhs) GBL_NOEXCEPT;
GBL_EXPORT GblBool         GUM_ModelIndex_belongs              (GUM_ModelIndex index, GBL_CSELF) GBL_NOEXCEPT;

GBL_EXPORT size_t          GUM_IItemModel_columnCount          (GBL_CSELF, GUM_ModelIndex parent) GBL_NOEXCEPT;
GBL_EXPORT size_t          GUM_IItemModel_rowCount             (GBL_CSELF, GUM_ModelIndex parent) GBL_NOEXCEPT;
GBL_EXPORT GUM_ModelIndex  GUM_IItemModel_index                (GBL_CSELF, size_t row, size_t column, GUM_ModelIndex parent) GBL_NOEXCEPT;
GBL_EXPORT GUM_ModelIndex  GUM_IItemModel_parent               (GBL_CSELF, GUM_ModelIndex index) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT      GUM_IItemModel_data                 (GBL_CSELF, GUM_ModelIndex index, GblVariant* pValue) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT      GUM_IItemModel_setData              (GBL_SELF, GUM_ModelIndex index, GblVariant* pValue) GBL_NOEXCEPT;
GBL_EXPORT GblFlags        GUM_IItemModel_flags                (GBL_CSELF, GUM_ModelIndex index) GBL_NOEXCEPT;

//! Emits a data change without invalidating indices.
GBL_EXPORT GBL_RESULT      GUM_IItemModel_emitDataChanged      (GBL_SELF, GUM_ModelIndex index) GBL_NOEXCEPT;
//! Emits a structure change and invalidates existing indices.
GBL_EXPORT GBL_RESULT      GUM_IItemModel_emitStructureChanged (GBL_SELF) GBL_NOEXCEPT;

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_IITEMMODEL_H

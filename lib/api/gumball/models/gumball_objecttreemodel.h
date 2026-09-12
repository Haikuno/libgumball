#ifndef GUM_OBJECTTREEMODEL_H
#define GUM_OBJECTTREEMODEL_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__objecttreemodel_8h.html

/*!  \file
 *   \ref     GUM_ObjectTreeModel "GUM_ObjectTreeModel data structure and hierarchy graph"
 *   \ingroup models
 *
 *   GUM_ObjectTreeModel exposes a GblObject hierarchy through GUM_IItemModel.
 *   Call GUM_ObjectTreeModel_refresh() after changing the hierarchy externally.
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gumball/ifaces/gumball_iitemmodel.h>

#define GUM_OBJECTTREEMODEL_TYPE          (GBL_TYPEID     (GUM_ObjectTreeModel))
#define GUM_OBJECTTREEMODEL(self)         (GBL_CAST       (GUM_ObjectTreeModel, self))
#define GUM_OBJECTTREEMODEL_CLASS(klass)  (GBL_CLASS_CAST (GUM_ObjectTreeModel, klass))
#define GUM_OBJECTTREEMODEL_CLASSOF(self) (GBL_CLASSOF    (GUM_ObjectTreeModel, self))

#define GBL_SELF_TYPE GUM_ObjectTreeModel

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_ObjectTreeModel);

GBL_CLASS_DERIVE_EMPTY(GUM_ObjectTreeModel, GblBox, GUM_IItemModel)
GBL_INSTANCE_DERIVE_EMPTY(GUM_ObjectTreeModel, GblBox)

GBL_EXPORT GblType              GUM_ObjectTreeModel_type    (void) GBL_NOEXCEPT;
GBL_EXPORT GUM_ObjectTreeModel* GUM_ObjectTreeModel_create  (GblObject* pRoot) GBL_NOEXCEPT;
GBL_EXPORT GblObject*           GUM_ObjectTreeModel_root    (const GUM_ObjectTreeModel* pSelf) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           GUM_ObjectTreeModel_setRoot (GUM_ObjectTreeModel* pSelf, GblObject* pRoot) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT           GUM_ObjectTreeModel_refresh (GUM_ObjectTreeModel* pSelf) GBL_NOEXCEPT;

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_OBJECTTREEMODEL_H

#ifndef GUM_PROPERTYMODEL_H
#define GUM_PROPERTYMODEL_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__propertymodel_8h.html

/*!  \file
 *   \ref     GUM_PropertyModel "GUM_PropertyModel data structure and hierarchy graph"
 *   \ingroup models
 *
 *   GUM_PropertyModel exposes a GblObject's properties through GUM_IItemModel.
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gumball/ifaces/gumball_iitemmodel.h>

#define GUM_PROPERTYMODEL_TYPE          (GBL_TYPEID     (GUM_PropertyModel))
#define GUM_PROPERTYMODEL(self)         (GBL_CAST       (GUM_PropertyModel, self))
#define GUM_PROPERTYMODEL_CLASS(klass)  (GBL_CLASS_CAST (GUM_PropertyModel, klass))
#define GUM_PROPERTYMODEL_CLASSOF(self) (GBL_CLASSOF    (GUM_PropertyModel, self))

#define GBL_SELF_TYPE GUM_PropertyModel

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_PropertyModel);

GBL_CLASS_DERIVE_EMPTY(GUM_PropertyModel, GblBox, GUM_IItemModel)
GBL_INSTANCE_DERIVE_EMPTY(GUM_PropertyModel, GblBox)

GBL_EXPORT GblType            GUM_PropertyModel_type      (void) GBL_NOEXCEPT;
GBL_EXPORT GUM_PropertyModel* GUM_PropertyModel_create    (GblObject* pObject) GBL_NOEXCEPT;
GBL_EXPORT GblObject*         GUM_PropertyModel_object    (const GUM_PropertyModel* pSelf) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT         GUM_PropertyModel_setObject (GUM_PropertyModel* pSelf, GblObject* pObject) GBL_NOEXCEPT;

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_PROPERTYMODEL_H

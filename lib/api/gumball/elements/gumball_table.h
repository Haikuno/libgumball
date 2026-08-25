#ifndef GUM_TABLE_H
#define GUM_TABLE_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__table_8h.html

/*!  \file
 *   \ref     GUM_Table "GUM_Table data structure and hierarchy graph"
 *   \ingroup elements
 *
 *   GUM_Table displays the rows and columns of a GUM_IItemModel.
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gumball/elements/gumball_widget.h>
#include <gumball/ifaces/gumball_iitemmodel.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_TABLE_TYPE          (GBL_TYPEID     (GUM_Table))
#define GUM_TABLE(self)         (GBL_CAST       (GUM_Table, self))
#define GUM_TABLE_CLASS(klass)  (GBL_CLASS_CAST (GUM_Table, klass))
#define GUM_TABLE_CLASSOF(self) (GBL_CLASSOF    (GUM_Table, self))
//! @}

#define GBL_SELF_TYPE GUM_Table

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Table);

GBL_CLASS_DERIVE_EMPTY(GUM_Table, GUM_Widget)

GBL_INSTANCE_DERIVE(GUM_Table, GUM_Widget)
    float rowHeight;
GBL_INSTANCE_END

GBL_PROPERTIES(GUM_Table,
    (rowHeight, GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE)
)

GBL_SIGNALS(GUM_Table,
    (selectionChanged, (GBL_INSTANCE_TYPE, pReceiver),
                       (GBL_POINTER_TYPE,  pHandle),
                       (GBL_SIZE_TYPE,     column))
)

GBL_EXPORT GblType         GUM_Table_type           (void) GBL_NOEXCEPT;
GBL_EXPORT GUM_IItemModel* GUM_Table_model          (const GUM_Table* pSelf) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT      GUM_Table_setModel       (GUM_Table* pSelf, GUM_IItemModel* pModel) GBL_NOEXCEPT;
GBL_EXPORT GUM_ModelIndex  GUM_Table_selection      (const GUM_Table* pSelf) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT      GUM_Table_select         (GUM_Table* pSelf, GUM_ModelIndex index) GBL_NOEXCEPT;
GBL_EXPORT float           GUM_Table_scrollPosition (const GUM_Table* pSelf) GBL_NOEXCEPT;
GBL_EXPORT float           GUM_Table_scrollRange    (const GUM_Table* pSelf) GBL_NOEXCEPT;
GBL_EXPORT void            GUM_Table_scrollTo       (GUM_Table* pSelf, float position) GBL_NOEXCEPT;
GBL_EXPORT void            GUM_Table_scrollBy       (GUM_Table* pSelf, float delta) GBL_NOEXCEPT;

//! Returns a new GUM_Table. Optionally takes a list of Name/Value pairs for properties.
#define GUM_Table_create(/* propertyName, propertyValue */ ...) GBL_NEW(GUM_Table __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_TABLE_H

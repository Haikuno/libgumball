#ifndef GUM_TREE_H
#define GUM_TREE_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__tree_8h.html

/*!  \file
 *   \ref     GUM_Tree "GUM_Tree data structure and hierarchy graph"
 *   \ingroup elements
 *
 *   GUM_Tree displays a hierarchical GUM_IItemModel.
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
#define GUM_TREE_TYPE          (GBL_TYPEID     (GUM_Tree))
#define GUM_TREE(self)         (GBL_CAST       (GUM_Tree, self))
#define GUM_TREE_CLASS(klass)  (GBL_CLASS_CAST (GUM_Tree, klass))
#define GUM_TREE_CLASSOF(self) (GBL_CLASSOF    (GUM_Tree, self))
//! @}

#define GBL_SELF_TYPE GUM_Tree

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Tree);

GBL_CLASS_DERIVE_EMPTY(GUM_Tree, GUM_Widget)

GBL_INSTANCE_DERIVE(GUM_Tree, GUM_Widget)
    float rowHeight;
    float indentWidth;
GBL_INSTANCE_END

GBL_PROPERTIES(GUM_Tree,
    (rowHeight,   GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE),
    (indentWidth, GBL_GENERIC, (READ, WRITE), GBL_FLOAT_TYPE)
)

GBL_SIGNALS(GUM_Tree,
    (selectionChanged, (GBL_INSTANCE_TYPE, pReceiver),
                       (GBL_POINTER_TYPE,  pHandle),
                       (GBL_SIZE_TYPE,     column))
)

GBL_EXPORT GblType         GUM_Tree_type           (void) GBL_NOEXCEPT;
GBL_EXPORT GUM_IItemModel* GUM_Tree_model          (const GUM_Tree* pSelf) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT      GUM_Tree_setModel       (GUM_Tree* pSelf, GUM_IItemModel* pModel) GBL_NOEXCEPT;
GBL_EXPORT GUM_ModelIndex  GUM_Tree_selection      (const GUM_Tree* pSelf) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT      GUM_Tree_select         (GUM_Tree* pSelf, GUM_ModelIndex index) GBL_NOEXCEPT;
GBL_EXPORT GblBool         GUM_Tree_expanded       (const GUM_Tree* pSelf, GUM_ModelIndex index) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT      GUM_Tree_setExpanded    (GUM_Tree* pSelf, GUM_ModelIndex index, GblBool expanded) GBL_NOEXCEPT;
GBL_EXPORT float           GUM_Tree_scrollPosition (const GUM_Tree* pSelf) GBL_NOEXCEPT;
GBL_EXPORT float           GUM_Tree_scrollRange    (const GUM_Tree* pSelf) GBL_NOEXCEPT;
GBL_EXPORT void            GUM_Tree_scrollTo       (GUM_Tree* pSelf, float position) GBL_NOEXCEPT;
GBL_EXPORT void            GUM_Tree_scrollBy       (GUM_Tree* pSelf, float delta) GBL_NOEXCEPT;

//! Returns a new GUM_Tree. Optionally takes a list of Name/Value pairs for properties.
#define GUM_Tree_create(/* propertyName, propertyValue */ ...) GBL_NEW(GUM_Tree __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_TREE_H

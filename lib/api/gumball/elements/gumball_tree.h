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
#define GUM_TREE_TYPE          (GBL_TYPEID     (GUM_Tree))           //!< Returns the GUM_Tree Type UUID
#define GUM_TREE(self)         (GBL_CAST       (GUM_Tree, self))     //!< Casts an instance of a compatible element to a GUM_Tree
#define GUM_TREE_CLASS(klass)  (GBL_CLASS_CAST (GUM_Tree, klass))    //!< Casts a class     of a compatible element to a GUM_TreeClass
#define GUM_TREE_CLASSOF(self) (GBL_CLASSOF    (GUM_Tree, self))     //!< Casts an instance of a compatible element to a GUM_TreeClass
//! @}

#define GBL_SELF_TYPE                GUM_Tree

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
    (selectionChanged, (GBL_POINTER_TYPE, pHandle),
                       (GBL_SIZE_TYPE,    column))
)

GBL_EXPORT GblType GUM_Tree_type(void) GBL_NOEXCEPT;

//! Returns the current model, or nullptr. Do not unref it.
GBL_EXPORT GUM_IItemModel* GUM_Tree_model         (const GUM_Tree* pSelf)                                   GBL_NOEXCEPT;
//! Sets the model and resets selection, expansion, and scrolling. Keeps a reference; nullptr clears it.
GBL_EXPORT GBL_RESULT      GUM_Tree_setModel      (GUM_Tree* pSelf, GUM_IItemModel* pModel)                 GBL_NOEXCEPT;
//! Returns the selected index, or GUM_MODEL_INDEX_INVALID if nothing is selected.
GBL_EXPORT GUM_ModelIndex  GUM_Tree_selection     (const GUM_Tree* pSelf)                                   GBL_NOEXCEPT;
//! Selects a selectable index in column zero. GUM_MODEL_INDEX_INVALID clears selection.
GBL_EXPORT GBL_RESULT      GUM_Tree_select        (GUM_Tree* pSelf, GUM_ModelIndex index)                   GBL_NOEXCEPT;
//! Returns true if index has children and is expanded. Leaves return false.
GBL_EXPORT GblBool         GUM_Tree_expanded      (const GUM_Tree* pSelf, GUM_ModelIndex index)             GBL_NOEXCEPT;
//! Expands or collapses a row in column zero. Leaves are unchanged.
GBL_EXPORT GBL_RESULT      GUM_Tree_setExpanded   (GUM_Tree* pSelf, GUM_ModelIndex index, GblBool expanded) GBL_NOEXCEPT;
//! Returns the current vertical scroll offset, in pixels.
GBL_EXPORT float           GUM_Tree_scrollPosition(const GUM_Tree* pSelf)                                   GBL_NOEXCEPT;
//! Returns the maximum vertical scroll offset, in pixels.
GBL_EXPORT float           GUM_Tree_scrollRange   (const GUM_Tree* pSelf)                                   GBL_NOEXCEPT;
//! Eases toward a vertical offset in pixels, clamped to the scroll range.
GBL_EXPORT void            GUM_Tree_scrollTo      (GUM_Tree* pSelf, float position)                         GBL_NOEXCEPT;
//! Adds delta pixels to the scroll target, clamped to the scroll range.
GBL_EXPORT void            GUM_Tree_scrollBy      (GUM_Tree* pSelf, float delta)                            GBL_NOEXCEPT;

//! Returns a new GUM_Tree. Optionally takes a list of Name/Value pairs for properties.
#define GUM_Tree_create(/* propertyName, propertyValue */ ...) GBL_NEW(GUM_Tree __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_TREE_H

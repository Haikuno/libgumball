#ifndef GUM_ROOT_H
#define GUM_ROOT_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__root_8h.html

/*!  \file
 *   \ref        GUM_Root "GUM_Root data structure and hierarchy graph"
 *   \ingroup    elements
 *
 *   GUM_Root is the element responsible for holding all other elements in the scene.
*/

#include <gimbal/gimbal_meta.h>
#include <gimbal/core/gimbal_module.h>

/*!  \name  Type System
 *   \brief Type UUID and cast operators
 *   @{
*/
#define GUM_ROOT_TYPE            (GBL_TYPEID     (GUM_Root))           //!< Returns the GUM_Root Type UUID
#define GUM_ROOT(self)           (GBL_CAST       (GUM_Root, self))     //!< Casts an instance of a compatible element to a GUM_Root
#define GUM_ROOT_CLASS(klass)    (GBL_CLASS_CAST (GUM_Root, klass))    //!< Casts a class of a compatible element to a GUM_RootClass
#define GUM_ROOT_CLASSOF(self)   (GBL_CLASSOF    (GUM_Root, self))     //!< Casts an instance of a compatible element to a GUM_RootClass
//! @}

#define GBL_SELF_TYPE            GUM_Root

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Root);

/*!  \struct  GUM_RootClass
 *   \extends GblModuleClass
 *   \brief   GUM_Root structure
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Root, GblModule)

/*!  \class   GUM_Root
 *   \extends GblModule
 *   \brief   Root element
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_Root, GblModule)

//! Returns a new GUM_Root. Only one Root can be active at a time.
GBL_EXPORT GUM_Root* GUM_Root_create(void) GBL_NOEXCEPT;

GblType GUM_Root_type(void) GBL_NOEXCEPT;

//! Updates the Root, input system, backend and layout.
GBL_RESULT GUM_Root_update(GBL_SELF) GBL_NOEXCEPT;

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_ROOT_H

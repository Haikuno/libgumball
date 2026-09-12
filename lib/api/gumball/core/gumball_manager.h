#ifndef GUM_MANAGER_H
#define GUM_MANAGER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__manager_8h.html

/*!  \file
 *   \ingroup core
 *
 *   GUM_Manager is libGumball's resource manager.
 *   Loaded resources are cached by their absolute path.
 *
 *   \todo
 *       - Add support for audio
 *       - Back resources with a GblArena
 *
 *   \author     2025, 2026 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include <gimbal/core/gimbal_module.h>
#include <gumball/ifaces/gumball_iresource.h>

#define GUM_MANAGER_TYPE            (GBL_TYPEID     (GUM_Manager))
#define GUM_MANAGER(self)           (GBL_CAST       (GUM_Manager, self))
#define GUM_MANAGER_CLASS(klass)    (GBL_CLASS_CAST (GUM_Manager, klass))
#define GUM_MANAGER_CLASSOF(self)   (GBL_CLASSOF    (GUM_Manager, self))
#define GBL_SELF_TYPE               GUM_Manager

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Manager);

/*!  \struct  GUM_ManagerClass
 *   \extends GblModuleClass
 *   \brief   Resource manager class
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Manager, GblModule)

/*!  \struct  GUM_Manager
 *   \extends GblModule
 *   \brief   Resource manager
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_Manager, GblModule)

GblType GUM_Manager_type(void) GBL_NOEXCEPT;

//! Loads a resource and stores a new reference in ppResource.
GBL_EXPORT GBL_RESULT      GUM_Manager_loadEx      (GblStringRef* pPath,
                                                    GUM_IResource** ppResource) GBL_NOEXCEPT;

//! Loads a resource, returning nullptr on failure.
GBL_EXPORT GUM_IResource*  GUM_Manager_load        (GblStringRef* pPath) GBL_NOEXCEPT;

//! Removes a resource from the cache. Existing references remain valid.
GBL_EXPORT GBL_RESULT      GUM_Manager_evict       (GUM_IResource* pResource) GBL_NOEXCEPT;

//! Sets the base path used for relative resource paths.
GBL_EXPORT GBL_RESULT      GUM_Manager_setBasePath (GblStringRef* pPath) GBL_NOEXCEPT;

//! Returns the current base path.
GBL_EXPORT GblStringRef*   GUM_Manager_basePath    (void) GBL_NOEXCEPT;

//! Deinitializes the resource manager and clears its cache. Existing references remain valid.
GBL_EXPORT void            GUM_Manager_deinit      (void) GBL_NOEXCEPT;

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_MANAGER_H

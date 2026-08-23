#ifndef GUM_MANAGER_H
#define GUM_MANAGER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__manager_8h.html

/*!  \file
 *   \ingroup core
 *
 *   GUM_Manager is libGumball's resource manager. It starts when first used and
 *   caches loaded resources by normalized absolute path.
 *
 *   The cache owns one reference to each resource and load returns another
 *   caller-owned reference. Removing a resource from the cache does not
 *   invalidate references held by the caller.
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

/*! \struct GUM_ManagerClass
 *  \extends GblModuleClass
 *  \brief Resource manager class.
 */
GBL_CLASS_DERIVE_EMPTY(GUM_Manager, GblModule)

/*! \struct GUM_Manager
 *  \extends GblModule
 *  \brief Resource manager instance.
 */
GBL_INSTANCE_DERIVE_EMPTY(GUM_Manager, GblModule)

GblType GUM_Manager_type(void) GBL_NOEXCEPT;

/*! Loads a resource into \p ppResource.
 *  On success the returned resource is caller-owned. On failure \p ppResource
 *  is set to nullptr. Returns GBL_RESULT_NOT_READY while Manager is shutting down.
 */
GBL_EXPORT GBL_RESULT      GUM_Manager_loadEx      (GblStringRef* pPath,
                                                    GUM_IResource** ppResource) GBL_NOEXCEPT;

//! Loads a resource, returning nullptr on failure.
GBL_EXPORT GUM_IResource*  GUM_Manager_load        (GblStringRef* pPath) GBL_NOEXCEPT;

//! Removes a resource from the cache without invalidating caller-owned references.
GBL_EXPORT GBL_RESULT      GUM_Manager_evict       (GUM_IResource* pResource) GBL_NOEXCEPT;

//! Sets the base path used for future relative resource loads.
GBL_EXPORT GBL_RESULT      GUM_Manager_setBasePath (GblStringRef* pPath) GBL_NOEXCEPT;

//! Returns the current normalized absolute base path as a borrowed reference.
GBL_EXPORT GblStringRef*   GUM_Manager_basePath    (void) GBL_NOEXCEPT;

//! Stops Manager and releases its cache. Safe to call repeatedly; a later load starts it again.
GBL_EXPORT void            GUM_Manager_deinit      (void) GBL_NOEXCEPT;

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_MANAGER_H

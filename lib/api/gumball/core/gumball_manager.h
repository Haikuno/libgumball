#ifndef GUM_MANAGER_H
#define GUM_MANAGER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__manager_8h.html

/*!  \file
 *   \ingroup core
 *
 *   GUM_Manager is a resource manager for libGumball.
 *   It is used to load resources, returning refcounted pointers to them.
 *
 *   \todo
 *       - Add support for audio
 *       - Back resources with a GblArena
 *
 *   \author     2025 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include <gimbal/gimbal_meta.h>
#include <gumball/ifaces/gumball_iresource.h>


#define GUM_MANAGER_TYPE    (GBL_TYPEID(GUM_Manager)) //!< Returns the GUM_Manager Type UUID
#define GBL_SELF_TYPE       GUM_Manager

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Manager);

/*!  \struct GUM_ManagerClass
 *   \extends GblStaticClass
 *   \brief GUM_Manager structure
 *
 *   GUM_ManagerClass derives from GblStaticClass, adding nothing new
*/
//!  \cond
GBL_STATIC_CLASS_DERIVE_EMPTY(GUM_Manager)

GblType GUM_Manager_type(void);
//!  \endcond

//!  Loads a resource from the given path, and stores it in the internal hashset. If the resource has already been loaded, returns a reference to the existing resource.
GBL_EXPORT GUM_IResource* GUM_Manager_load(GblStringRef* path)         GBL_NOEXCEPT;

//!  Unloads a resource from the manager, removing it from the internal hashset.
GBL_EXPORT void           GUM_Manager_unload(GUM_IResource* pResource) GBL_NOEXCEPT;

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_MANAGER_H

#ifndef GUM_IRESOURCE_H
#define GUM_IRESOURCE_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__iresource_8h.html

/*!  \file    gumball_iresource.h
 *   \ref     GUM_IResource "GUM_IResource data structure and hierarchy graph"
 *   \ingroup ifaces
 *
 *   GUM_IResource is the base interface for all resources in libGumball, such as textures and fonts.
*/

#include <gimbal/gimbal_meta.h>

/*!  \name  Type system
 *   \brief Type UUID and cast operators
 *   @{
*/
#define GUM_IRESOURCE_TYPE                (GBL_TYPEID     (GUM_IResource))              //!< Returns the GUM_IResource Type UUID
#define GUM_IRESOURCE(instance)           (GBL_CAST       (GUM_IResource, instance))    //!< Casts an instance of a compatible type to GUM_IResource
#define GUM_IRESOURCE_CLASS(klass)        (GBL_CLASS_CAST (GUM_IResource, klass))       //!< Casts a compatible class to GUM_IResourceClass
#define GUM_IRESOURCE_CLASSOF(instance)   (GBL_CLASSOF    (GUM_IResource, instance))    //!< Returns the GUM_IResourceClass implementation for an instance
//! @}

#define GBL_SELF_TYPE GUM_IResource

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_IResource);

/*!  \struct  GUM_IResource
 *   \extends GblInterface
 *   \brief   Resource loading/unloading interface
*/
//! \cond
GBL_INTERFACE_DERIVE(GUM_IResource)
    GBL_RESULT (*pFnLoad)  (GBL_SELF, GblStringRef* pPath);
    GBL_RESULT (*pFnUnload)(GBL_SELF);
GBL_INTERFACE_END

GblType GUM_IResource_type(void) GBL_NOEXCEPT;
//! \endcond

//! Returns a new reference to a GUM_IResource.
GBL_EXPORT GUM_IResource* GUM_IResource_ref   (GBL_SELF) GBL_NOEXCEPT;
//! Releases a resource reference. The final reference unloads backend data and must be released before the backend runtime shuts down.
GBL_EXPORT GblRefCount    GUM_IResource_unref (GBL_SELF) GBL_NOEXCEPT;

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_IRESOURCE_H

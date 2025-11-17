#ifndef GUM_IRESOURCE_H
#define GUM_IRESOURCE_H

/*!
 * \file    gumball_iresource.h
 * \ref     GUM_IResource "GUM_IResource data structure and hierarchy graph"
 * \ingroup ifaces
 *
 * GUM_IResource is the base interface for all resources in libGumball, such as textures and fonts.
*/

#include <gimbal/gimbal_meta.h>
#include <gimbal/utils/gimbal_byte_array.h>

/*!
 * \name  Type system
 * \brief Type UUID and cast operators
 * @{
*/
#define GUM_IRESOURCE_TYPE				(GBL_TYPEID		(GUM_IResource)) 			//!< Returns the GUM_IResource Type UUID
#define GUM_IRESOURCE(instance)	        (GBL_CAST		(GUM_IResource, instance)) 	//!< Casts an instance of a compatible element to a GUM_IResource
#define GUM_IRESOURCE_CLASS(klass)		(GBL_CLASS_CAST (GUM_IResource, klass)) 	//!< Casts an class of a compatible element to a GUM_IResource
#define GUM_IRESOURCE_CLASSOF(instance)	(GBL_CLASSOF	(GUM_IResource, instance))	//!< Casts an instance of a compatible element to a GUM_IResource
//! @}

#define GBL_SELF_TYPE 					 GUM_IResource

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_IResource);

/*!
 * \struct  GUM_IResource
 * \extends GblInterface
 * \brief   GUM_IResource interface
 *
 * GUM_IResource is the base interface for all resources in libGumball.
*/
//! \cond
GBL_INTERFACE_DERIVE(GUM_IResource)
	GUM_IResource*  (*pFnRef)		(GBL_SELF);
	GblRefCount 	(*pFnUnref)		(GBL_SELF);
	GBL_RESULT 		(*pFnCreate)	(GBL_SELF, GblByteArray **ppByteArray, GblQuark quark, GblStringRef *extension);
GBL_INTERFACE_END

GblType GUM_IResource_type(void);
//! \endcond

GUM_IResource *GUM_IResource_ref(GUM_IResource *pResource);  //!< Returns a reference to a GUM_IResource, increasing the reference count.
GblRefCount   GUM_IResource_unref(GUM_IResource *pResource); //!< Decrements the reference count for the passed GUM_IResource. \note The GUM_Manager holds a reference to the resource, so to free the resource, call GUM_Manager_unload()

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_IRESOURCE_H

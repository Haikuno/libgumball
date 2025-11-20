#ifndef GUM_IRESOURCE_H
#define GUM_IRESOURCE_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__iresource_8h.html

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
	GBL_RESULT		(*pFnLoad)			(GBL_SELF, GblStringRef *path);
	GBL_RESULT		(*pFnUnload)		(GBL_SELF);
	void*			(*pFnValue)			(GBL_CSELF);
	GBL_RESULT		(*pFnSetValue)		(GBL_SELF, void *pValue);
	GBL_RESULT		(*pFnQuark)			(GBL_CSELF, GblQuark *quark);
	GBL_RESULT		(*pFnSetQuark)		(GBL_SELF,  GblQuark quark);
GBL_INTERFACE_END

GblType GUM_IResource_type(void);
//! \endcond

//! Returns a new reference to a GUM_IResource, increasing the reference count.
GBL_EXPORT GUM_IResource  *GUM_IResource_ref   	  (GBL_SELF)   				GBL_NOEXCEPT;
//! Decrements the reference count for the passed GUM_IResource. \note The GUM_Manager holds a reference to the resource, so to free the resource, call GUM_Manager_unload()
GBL_EXPORT GblRefCount     GUM_IResource_unref 	  (GBL_SELF) 				GBL_NOEXCEPT;
//! Returns the backend-specific data of the resource as a void*
GBL_EXPORT void		   	  *GUM_IResource_data  	  (GBL_CSELF) 				GBL_NOEXCEPT;
//! Sets the backend-specific data of the resource to the passed void* \note Don't use this function unless you know what you're doing!
GBL_EXPORT void		   	   GUM_IResource_setData  (GBL_SELF, void *pValue) 	GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_IRESOURCE_H
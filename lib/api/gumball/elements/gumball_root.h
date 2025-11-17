#ifndef GUM_ROOT_H
#define GUM_ROOT_H

/*! \file
 *	\ref	GUM_Root "GUM_Root data structure and hierarchy graph"
 *  \ingroup elements
 *
 *  GUM_Root is the element responsible for holding all other elements in the scene.
 *  It also internally holds the draw queue, which is a list of all drawable elements in the scene,
 *  sorted by z-index.
 *
 *  \todo
 * 	 	- Make the draw queue private.
*/

#include <gimbal/gimbal_meta.h>
#include <gimbal/core/gimbal_module.h>
#include <gimbal/gimbal_containers.h>

/*! \name  Type System
 * 	\brief Type UUID and cast operators
 * 	@{
*/
#define GUM_ROOT_TYPE			(GBL_TYPEID		(GUM_Root))			//!< Returns the GUM_Root Type UUID
#define GUM_ROOT(self)			(GBL_CAST		(GUM_Root, self))	//!< Casts an instance of a compatible element to a GUM_Root
#define GUM_ROOT_CLASS(klass)	(GBL_CLASS_CAST	(GUM_Root, klass)) 	//!< Casts an class of a compatible element to a GUM_Root
#define GUM_ROOT_CLASSOF(self)	(GBL_CLASSOF	(GUM_Root, self)) 	//!< Casts an instance of a compatible element to a GUM_Root
//! @}

#define GBL_SELF_TYPE 			 GUM_Root

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Root);

/*!
 *  \struct  GUM_RootClass
 *  \extends GblModuleClass
 *  \brief   GUM_Root structure
 *
 * GUM_RootClass derives from GblModuleClass,
 * adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Root, GblModule)

/*!
 *  \class   GUM_Root
 *  \extends GblModule
 *  \brief   Root element
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_Root, GblModule)

GUM_Root* GUM_Root_create(void); //!< Returns a new GUM_Root. \note Currently there is no support for having multiple roots at once.
//! \cond
GblType GUM_Root_type(void);

void 			GUM_drawQueue_init(void);
void 			GUM_drawQueue_push(GblObject *pObj);
void 			GUM_drawQueue_remove(GblObject *pObj);
GblArrayList 	*GUM_drawQueue_get(void);
//!\endcond

GBL_DECLS_END
#undef GBL_SELF_TYPE

#endif // GUM_ROOT_H

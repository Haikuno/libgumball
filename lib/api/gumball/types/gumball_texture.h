#ifndef GUM_TEXTURE_H
#define GUM_TEXTURE_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__texture_8h.html

/*! \file
 *  \ref     GUM_Texture "GUM_Texture data structure and hierarchy graph"
 *  \ingroup types
 *
 *  GUM_Texture is the backend-agnostic type that represents a texture
 *
 *  \author 	2025 Agustín Bellagamba
 *  \copyright 	MIT License
*/

#include <gimbal/gimbal_meta.h>
#include <gumball/types/gumball_vector2.h>
#include <gumball/ifaces/gumball_iresource.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_TEXTURE_TYPE 		  (GBL_TYPEID	  (GUM_Texture))
#define GUM_TEXTURE(self) 		  (GBL_CAST		  (GUM_Texture, self))
#define GUM_TEXTURE_CLASS(klass)  (GBL_CLASS_CAST (GUM_Texture, klass))
#define GUM_TEXTURE_CLASSOF(self) (GBL_CLASSOF	  (GUM_Texture, self))
//! @}

#define GBL_SELF_TYPE GUM_Texture

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Texture);

/*!
 *	\struct  	GUM_TextureClass
 *	\extends 	GblBoxClass
 *	\implements GUM_IResource
 *	\brief   	GUM_Texture structure
 *
 *	GUM_TextureClass derives from GblBoxClass, adding nothing new.
 *  It also implements the GUM_IResource interface.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_Texture, GblBox, GUM_IResource)

/*!
 *	\class   GUM_Texture
 *	\extends GblBox
 *  \brief   Backend agnostic texture type
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_Texture, GblBox)

//! \cond
GblType GUM_Texture_type(void);
//! \endcond

//! Returns the size of the texture as a GUM_Vector2
GUM_Vector2 GUM_Texture_size(GBL_SELF);

//! Returns a void pointer to the underlying backend specific texture
void *GUM_Texture_getTexture(GBL_SELF);

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_TEXTURE_H

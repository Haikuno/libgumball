#ifndef GUM_RENDERER_H
#define GUM_RENDERER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__renderer_8h.html

/*! \file
 *  \ingroup types
 *
 *  GUM_Renderer is the renderer type used in libGumball
 *  It exists for backends that require one, such as SDL.
 *
 * 	\author 	2025 Agustín Bellagamba
 *  \copyright 	MIT License
 *
*/

#include <gimbal/gimbal_meta.h>

/*! \struct GUM_Renderer
 *  \brief  GUM_Renderer structure
*/
typedef struct GUM_Renderer GUM_Renderer;
GBL_DECLS_BEGIN

//! Creates a new GUM_Renderer
GUM_Renderer *GUM_Renderer_create(void *pRenderer);

#define GUM_RENDERER_TYPE (GBL_TYPEID(GUM_Renderer)) //!< Returns the GUM_Renderer Type UUID

//! \cond
GblType GUM_Renderer_type(void);
//! \endcond
GBL_DECLS_END

#endif // GUM_Renderer_H

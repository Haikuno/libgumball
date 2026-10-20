#ifndef GUM_RENDERER_H
#define GUM_RENDERER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__renderer_8h.html

/*!  \file
 *   \ingroup types
 *
 *   GUM_Renderer wraps a backend renderer.
 *
 *   \author    2025, 2026 Agustín Bellagamba
 *   \copyright MIT License
 *
*/

#include <gimbal/gimbal_meta.h>

#define GUM_RENDERER_TYPE (GBL_TYPEID(GUM_Renderer)) //!< Returns the GUM_Renderer Type UUID

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Renderer);

//! \cond
GblType GUM_Renderer_type(void) GBL_NOEXCEPT;
//! \endcond

//! Returns a new GUM_Renderer. pRenderer must remain valid until the wrapper is destroyed.
GUM_Renderer* GUM_Renderer_create (void* pRenderer);
//! Destroys a GUM_Renderer.
void          GUM_Renderer_destroy(GUM_Renderer* pSelf);

GBL_DECLS_END

#endif // GUM_RENDERER_H

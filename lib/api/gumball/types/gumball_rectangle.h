#ifndef GUM_RECTANGLE_H
#define GUM_RECTANGLE_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__rectangle_8h.html

/*!  \file
 *   \ingroup types
 *
 *   GUM_Rectangle is a simple rectangle structure
 *
 *   \author     2025, 2026 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include <gimbal/gimbal_meta.h>

GBL_DECLS_BEGIN

/*!  \struct GUM_Rectangle
 *   \brief  GUM_Rectangle structure
*/
typedef struct {
    float x;
    float y;
    float width;
    float height;
} GUM_Rectangle;

#define GUM_RECTANGLE_TYPE (GBL_TYPEID(GUM_Rectangle)) //!< Returns the GUM_Rectangle Type UUID

//! \cond
GblType GUM_Rectangle_type(void) GBL_NOEXCEPT;
//! \endcond

//! Returns the intersection of two rectangles.
GUM_Rectangle GUM_Rectangle_intersect(GUM_Rectangle a, GUM_Rectangle b) GBL_NOEXCEPT;

constexpr GUM_Rectangle GUM_CLIP_RECT_NONE_ = ((GUM_Rectangle){ -1e30f, -1e30f, 2e30f, 2e30f });

GBL_DECLS_END

#endif // GUM_RECTANGLE_H

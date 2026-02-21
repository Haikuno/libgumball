#ifndef GUM_RECTANGLE_H
#define GUM_RECTANGLE_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__rectangle_8h.html

/*!  \file
 *   \ingroup types
 *
 *   GUM_Rectangle is a simple rectangle structure
 *
 *   \author     2025 Agustín Bellagamba
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
GblType GUM_Rectangle_type(void);
//! \endcond

GBL_DECLS_END

#endif // GUM_RECTANGLE_H

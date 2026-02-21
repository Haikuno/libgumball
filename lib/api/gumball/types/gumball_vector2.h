#ifndef GUM_VECTOR2_H
#define GUM_VECTOR2_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__vector2_8h.html

/*!  \file
 *   \ingroup types
 *
 *   GUM_Vector2 is a simple 2D vector structure
 *
 *   \author     2025 Agustín Bellagamba
 *   \copyright  MIT License
*/

#include <gimbal/gimbal_meta.h>

GBL_DECLS_BEGIN

/*!  \struct GUM_Vector2
 *   \brief  GUM_Vector2 structure
*/
typedef struct {
    float x;
    float y;
} GUM_Vector2;

#define GUM_VECTOR2_TYPE (GBL_TYPEID(GUM_Vector2)) //!< Returns the GUM_Vector2 Type UUID

//! \cond
GblType GUM_Vector2_type(void);
//! \endcond

GBL_EXPORT GUM_Vector2 GUM_Vector2_add          (GUM_Vector2 a, GUM_Vector2 b)                      GBL_NOEXCEPT; //!< Returns the sum of two vectors
GBL_EXPORT GUM_Vector2 GUM_Vector2_subtract     (GUM_Vector2 a, GUM_Vector2 b)                      GBL_NOEXCEPT; //!< Returns the difference of two vectors
GBL_EXPORT GUM_Vector2 GUM_Vector2_scale        (GUM_Vector2 a, float scalar )                      GBL_NOEXCEPT; //!< Returns the product of a vector and a scalar
GBL_EXPORT float       GUM_Vector2_distance     (GUM_Vector2 a, GUM_Vector2 b)                      GBL_NOEXCEPT; //!< Returns the distance between two vectors
GBL_EXPORT float       GUM_Vector2_length       (GUM_Vector2 a)                                     GBL_NOEXCEPT; //!< Returns the length of a vector
GBL_EXPORT float       GUM_Vector2_angle        (GUM_Vector2 a, GUM_Vector2 b)                      GBL_NOEXCEPT; //!< Returns the angle of (b - a). Range (-pi, pi)
GBL_EXPORT float       GUM_Vector2_angleTo      (GUM_Vector2 from, GUM_Vector2 to, GUM_Vector2 dir) GBL_NOEXCEPT; //!< Returns the unsigned angle between dir and (to - from). Range (0, pi)
GBL_EXPORT float       GUM_Vector2_signedAngleTo(GUM_Vector2 from, GUM_Vector2 to, GUM_Vector2 dir) GBL_NOEXCEPT; //!< Returns the signed angle from dir to (to - from). Range (-pi, pi)
GBL_EXPORT float       GUM_Vector2_dot          (GUM_Vector2 a, GUM_Vector2 b)                      GBL_NOEXCEPT; //!< Returns the dot product of two vectors
GBL_EXPORT float       GUM_Vector2_cross        (GUM_Vector2 a, GUM_Vector2 b)                      GBL_NOEXCEPT; //!< Returns the cross product of two vectors

GBL_DECLS_END

#endif // GUM_VECTOR2_H

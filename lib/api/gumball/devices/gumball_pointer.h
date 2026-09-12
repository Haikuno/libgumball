#ifndef GUM_POINTER_H
#define GUM_POINTER_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__pointer_8h.html

/*!  \file
 *   \ref     GUM_Pointer "GUM_Pointer data structure and hierarchy graph"
 *   \ingroup devices
 *
 *   Base input device for a positional pointer.
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include "gumball_inputdevice.h"
#include <gumball/types/gumball_vector2.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_POINTER_TYPE            (GBL_TYPEID(GUM_Pointer))
#define GUM_POINTER(self)           (GBL_CAST(GUM_Pointer, self))
#define GUM_POINTER_CLASS(klass)    (GBL_CLASS_CAST(GUM_Pointer, klass))
#define GUM_POINTER_CLASSOF(self)   (GBL_CLASSOF(GUM_Pointer, self))
//! @}

#define GBL_SELF_TYPE GUM_Pointer

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Pointer);

GBL_CLASS_DERIVE_EMPTY(GUM_Pointer, GUM_InputDevice)

GBL_INSTANCE_DERIVE(GUM_Pointer, GUM_InputDevice)
    GUM_Vector2 position; //!< Current pointer position.
    GUM_Vector2 delta;    //!< Motion delta from the previous update.
GBL_INSTANCE_END

GblType GUM_Pointer_type(void) GBL_NOEXCEPT;

//! Returns a new GUM_Pointer. Optionally takes Name/Value property pairs.
#define GUM_Pointer_create(/* property_name, property_value */...) GBL_NEW(GUM_Pointer __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_POINTER_H

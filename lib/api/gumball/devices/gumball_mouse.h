#ifndef GUM_MOUSE_H
#define GUM_MOUSE_H

// View this file's documentation online: TODO: add link

/*!  \file
 *   \ref     GUM_Mouse "GUM_Mouse data structure and hierarchy graph"
 *   \ingroup devices
 *
 *   Mouse pointing device. Common pointer position and motion state live in
 *   GUM_Pointer; GUM_Mouse adds only mouse-specific wheel state.
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include "gumball_pointer.h"

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_MOUSE_TYPE            (GBL_TYPEID(GUM_Mouse))
#define GUM_MOUSE(self)           (GBL_CAST(GUM_Mouse, self))
#define GUM_MOUSE_CLASS(klass)    (GBL_CLASS_CAST(GUM_Mouse, klass))
#define GUM_MOUSE_CLASSOF(self)   (GBL_CLASSOF(GUM_Mouse, self))
//! @}

#define GBL_SELF_TYPE GUM_Mouse

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Mouse);

GBL_CLASS_DERIVE_EMPTY(GUM_Mouse, GUM_Pointer)

GBL_FLAGS(GUM_MOUSE_FLAGS,
    (GUM_MOUSE_BUTTON_LEFT,   "GUM_mouse_left",   0b00000001),
    (GUM_MOUSE_BUTTON_RIGHT,  "GUM_mouse_right",  0b00000010),
    (GUM_MOUSE_BUTTON_MIDDLE, "GUM_mouse_middle", 0b00000100)
)

GBL_INSTANCE_DERIVE(GUM_Mouse, GUM_Pointer)
    GUM_Vector2 wheel; //!< Scroll wheel delta.
GBL_INSTANCE_END

GblType GUM_Mouse_type(void) GBL_NOEXCEPT;

//! Returns a new GUM_Mouse. Optionally takes in a list of Name/Value pairs for properties.
#define GUM_Mouse_create(/* property_name, property_value */...) GBL_NEW(GUM_Mouse __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_MOUSE_H

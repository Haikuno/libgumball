#ifndef GUM_MOUSE_H
#define GUM_MOUSE_H

// View this file's documentation online: TODO: add link

/*!  \file
 *   \ref     GUM_Mouse "GUM_Mouse data structure and hierarchy graph"
 *   \ingroup devices
 *
 *   TODO: Brief description
 *
 *   TODO: Extended
 *         Description.
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
#define GUM_MOUSE_TYPE            (GBL_TYPEID(GUM_Mouse))
#define GUM_MOUSE(self)           (GBL_CAST(GUM_Mouse, self))
#define GUM_MOUSE_CLASS(klass)    (GBL_CLASS_CAST(GUM_Mouse, klass))
#define GUM_MOUSE_CLASSOF(self)   (GBL_CLASSOF(GUM_Mouse, self))
//! @}

#define GBL_SELF_TYPE GUM_Mouse

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Mouse);

/*!
 *    \struct  GUM_MouseClass
 *    \extends GUM_InputDeviceClass
 *    \brief   GUM_Mouse structure
 *
 *    GUM_MouseClass derives from GUM_InputDeviceClass,
 *    TODO: description
*/
//! \cond
GBL_CLASS_DERIVE_EMPTY(GUM_Mouse, GUM_InputDevice)
//! \endcond

GBL_FLAGS(GUM_MOUSE_FLAGS,
    (GUM_MOUSE_BUTTON_LEFT,   "GUM_mouse_left",   0x1),
    (GUM_MOUSE_BUTTON_RIGHT,  "GUM_mouse_right",  0x2),
    (GUM_MOUSE_BUTTON_MIDDLE, "GUM_mouse_middle", 0x4)
)

/*!
 *    \class   GUM_Mouse
 *    \extends GUM_InputDevice
 *    \brief   TODO: brief description
*/

GBL_INSTANCE_DERIVE(GUM_Mouse, GUM_InputDevice)
    GUM_Vector2 position; //!< Current mouse position
    GUM_Vector2 delta;    //!< Delta bewteen last frame's position and the current frame's position
GBL_INSTANCE_END

GblType GUM_Mouse_type(void) GBL_NOEXCEPT;

//! Returns a new GUM_Mouse. Optionally takes in a list of Name/Value pairs for properties
#define GUM_Mouse_create(/* property_name, property_value */...) GBL_NEW(GUM_Mouse __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_MOUSE_H
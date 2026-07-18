#ifndef GUM_KEYBOARD_H
#define GUM_KEYBOARD_H

// View this file's documentation online: TODO: add link

/*!  \file
 *   \ref     GUM_Keyboard "GUM_Keyboard data structure and hierarchy graph"
 *   \ingroup devices
 *
 *   GUM_Keyboard is the input device representing the system keyboard.
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include "gumball_inputdevice.h"

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_KEYBOARD_TYPE            (GBL_TYPEID    (GUM_Keyboard))
#define GUM_KEYBOARD(self)           (GBL_CAST      (GUM_Keyboard, self))
#define GUM_KEYBOARD_CLASS(klass)    (GBL_CLASS_CAST(GUM_Keyboard, klass))
#define GUM_KEYBOARD_CLASSOF(self)   (GBL_CLASSOF   (GUM_Keyboard, self))
//! @}

#define GBL_SELF_TYPE GUM_Keyboard

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Keyboard);

/*!
 *    \struct  GUM_KeyboardClass
 *    \extends GUM_InputDeviceClass
 *    \brief   GUM_Keyboard structure
 *
 *    GUM_KeyboardClass derives from GUM_InputDeviceClass, adding nothing new.
*/
//! \cond
GBL_CLASS_DERIVE_EMPTY(GUM_Keyboard, GUM_InputDevice)
//! \endcond

//! Bitmask of the subset of keys libGumball cares about (nav + confirm/cancel).
GBL_FLAGS(GUM_KEYBOARD_FLAGS,
    (GUM_KEYBOARD_KEY_UP,     "GUM_key_up",     0b00000000'00000001),
    (GUM_KEYBOARD_KEY_DOWN,   "GUM_key_down",   0b00000000'00000010),
    (GUM_KEYBOARD_KEY_LEFT,   "GUM_key_left",   0b00000000'00000100),
    (GUM_KEYBOARD_KEY_RIGHT,  "GUM_key_right",  0b00000000'00001000),
    (GUM_KEYBOARD_KEY_W,      "GUM_key_w",      0b00000000'00010000),
    (GUM_KEYBOARD_KEY_A,      "GUM_key_a",      0b00000000'00100000),
    (GUM_KEYBOARD_KEY_S,      "GUM_key_s",      0b00000000'01000000),
    (GUM_KEYBOARD_KEY_D,      "GUM_key_d",      0b00000000'10000000),
    (GUM_KEYBOARD_KEY_ENTER,  "GUM_key_enter",  0b00000001'00000000),
    (GUM_KEYBOARD_KEY_ESCAPE, "GUM_key_escape", 0b00000010'00000000)
)

/*!
 *    \class   GUM_Keyboard
 *    \extends GUM_InputDevice
 *    \brief   The system keyboard
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_Keyboard, GUM_InputDevice)

GblType GUM_Keyboard_type(void) GBL_NOEXCEPT;

//! Returns a new GUM_Keyboard. Optionally takes in a list of Name/Value pairs for properties
#define GUM_Keyboard_create(/* property_name, property_value */...) GBL_NEW(GUM_Keyboard __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_KEYBOARD_H

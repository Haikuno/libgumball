#ifndef GUM_GAMEPAD_H
#define GUM_GAMEPAD_H

// View this file's documentation online: TODO: add link

/*!  \file
 *   \ref     GUM_Gamepad "GUM_Gamepad data structure and hierarchy graph"
 *   \ingroup elements
 *
 *   Brief description
 *
 *   Extended
 *   Description.
 *
 *   \author       2026 Agustín Bellagamba
 *   \copyright    MIT License
*/

#include <gumball/devices/gumball_inputdevice.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_GAMEPAD_TYPE            (GBL_TYPEID     (GUM_Gamepad))
#define GUM_GAMEPAD(self)           (GBL_CAST       (GUM_Gamepad, self))
#define GUM_GAMEPAD_CLASS(klass)    (GBL_CLASS_CAST (GUM_Gamepad, klass))
#define GUM_GAMEPAD_CLASSOF(self)   (GBL_CLASSOF    (GUM_Gamepad, self))
//! @}

#define GBL_SELF_TYPE GUM_Gamepad

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_Gamepad);

/*!
 *    \struct  GUM_GamepadClass
 *    \extends GUM_InputDeviceClass
 *    \brief   GUM_Gamepad structure
 *
 *    GUM_GamepadClass derives from GUM_InputDeviceClass,
 *    TODO: description
*/
//! \cond
GBL_CLASS_DERIVE_EMPTY(GUM_Gamepad, GUM_InputDevice)
//! \endcond

GBL_FLAGS(GUM_GAMEPAD_FLAGS,
    (GUM_GAMEPAD_BUTTON_A,      "GUM_gamepad_a",      0x0001),
    (GUM_GAMEPAD_BUTTON_B,      "GUM_gamepad_b",      0x0002),
    (GUM_GAMEPAD_BUTTON_X,      "GUM_gamepad_x",      0x0004),
    (GUM_GAMEPAD_BUTTON_Y,      "GUM_gamepad_y",      0x0008),
    (GUM_GAMEPAD_BUTTON_LB,     "GUM_gamepad_lb",     0x0010),
    (GUM_GAMEPAD_BUTTON_RB,     "GUM_gamepad_rb",     0x0020),
    (GUM_GAMEPAD_BUTTON_START,  "GUM_gamepad_start",  0x0040),
    (GUM_GAMEPAD_BUTTON_SELECT, "GUM_gamepad_select", 0x0080),
    (GUM_GAMEPAD_DPAD_UP,       "GUM_gamepad_up",     0x0100),
    (GUM_GAMEPAD_DPAD_DOWN,     "GUM_gamepad_down",   0x0200),
    (GUM_GAMEPAD_DPAD_LEFT,     "GUM_gamepad_left",   0x0400),
    (GUM_GAMEPAD_DPAD_RIGHT,    "GUM_gamepad_right",  0x0800)
)

/*!
 *    \class   GUM_Gamepad
 *    \extends GUM_InputDevice
 *    \brief   TODO: brief description
*/
GBL_INSTANCE_DERIVE(GUM_Gamepad, GUM_InputDevice)
    uint8_t index;    //!> User-facing controller index.                             Default value is 0
    uint8_t rawIndex; //!> Physical slot this gamepad occupies, used in the backend. Default value is 0
GBL_INSTANCE_END

GBL_PROPERTIES(GUM_Gamepad,
    (index,    GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE),
    (rawIndex, GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE)
)

GblType GUM_Gamepad_type(void) GBL_NOEXCEPT;

//! Returns a new GUM_Gamepad. Optionally takes in a list of Name/Value pairs for properties
#define GUM_Gamepad_create(/* propertyName, propertyValue */ ...) GBL_NEW(GUM_Gamepad __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_GAMEPAD_H
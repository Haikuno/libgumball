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

#include <gimbal/gimbal.h>
#include <gumball/gumball.h>

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

/*!
 *    \class   GUM_Gamepad
 *    \extends GUM_InputDevice
 *    \brief   TODO: brief description
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_Gamepad, GUM_InputDevice)

GblType GUM_Gamepad_type(void) GBL_NOEXCEPT;

//! Returns a new GUM_Gamepad. Optionally takes in a list of Name/Value pairs for properties
#define GUM_Gamepad_create(/* propertyName, propertyValue */ ...) GBL_NEW(GUM_Gamepad __VA_OPT__(,) __VA_ARGS__)

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_GAMEPAD_H
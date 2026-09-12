#ifndef GUM_INPUTDEVICE_H
#define GUM_INPUTDEVICE_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__inputdevice_8h.html

/*!  \file
 *   \ref     GUM_InputDevice "GUM_InputDevice data structure and hierarchy graph"
 *   \ingroup devices
 *
 *   Base class for libGumball input devices.
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gimbal/gimbal_meta.h>

/*! \name  Type System
 *  \brief Type UUID and cast operators
 *  @{
*/
#define GUM_INPUTDEVICE_TYPE            (GBL_TYPEID     (GUM_InputDevice))
#define GUM_INPUTDEVICE(self)           (GBL_CAST       (GUM_InputDevice, self))
#define GUM_INPUTDEVICE_CLASS(klass)    (GBL_CLASS_CAST (GUM_InputDevice, klass))
#define GUM_INPUTDEVICE_CLASSOF(self)   (GBL_CLASSOF    (GUM_InputDevice, self))
//! @}

#define GBL_SELF_TYPE GUM_InputDevice

GBL_DECLS_BEGIN
GBL_FORWARD_DECLARE_STRUCT(GUM_InputDevice);
GBL_FORWARD_DECLARE_STRUCT(GUM_Widget);

/*!
 *    \struct  GUM_InputDeviceClass
 *    \extends GblObjectClass
 *    \brief   Input-device class structure
*/
GBL_CLASS_DERIVE_EMPTY(GUM_InputDevice, GblObject)

/*!
 *    \class   GUM_InputDevice
 *    \extends GblObject
 *    \brief   Base input device state shared by keyboard, pointer, and gamepad devices
*/
GBL_INSTANCE_DERIVE(GUM_InputDevice, GblObject)
    GblFlags buttons;     //!< Current device button state.
    GblFlags buttonsPrev; //!< Previous update's device button state.
    uint8_t  highlight_r; //!< Red   component of this device's focus-ring color. Default value is 255
    uint8_t  highlight_g; //!< Green component of this device's focus-ring color. Default value is 255
    uint8_t  highlight_b; //!< Blue  component of this device's focus-ring color. Default value is 255
    uint8_t  highlight_a; //!< Alpha component of this device's focus-ring color. Default value is 255
GBL_INSTANCE_END

GBL_PROPERTIES(GUM_InputDevice,
    (highlight_color, GBL_GENERIC, (READ, WRITE), GBL_UINT32_TYPE),
    (highlight_r,     GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE ),
    (highlight_g,     GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE ),
    (highlight_b,     GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE ),
    (highlight_a,     GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE )
)

GblType GUM_InputDevice_type(void) GBL_NOEXCEPT;

//! Returns the device name.
GBL_EXPORT GblStringRef* GUM_InputDevice_name(const GUM_InputDevice* pSelf) GBL_NOEXCEPT;

//! Returns the Widget currently focused by this device, or nullptr.
GBL_EXPORT GUM_Widget* GUM_InputDevice_focusedWidget(const GUM_InputDevice* pSelf) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_INPUTDEVICE_H

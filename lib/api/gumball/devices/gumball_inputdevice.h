#ifndef GUM_INPUTDEVICE_H
#define GUM_INPUTDEVICE_H

// View this file's documentation online: TODO: add link

/*!  \file
 *   \ref     GUM_InputDevice "GUM_InputDevice data structure and hierarchy graph"
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
 *    \brief   GUM_InputDevice structure
 *
 *    GUM_InputDeviceClass derives from GblObjectClass,
 *    adding nothing new.
*/
GBL_CLASS_DERIVE_EMPTY(GUM_InputDevice, GblObject)

/*!
 *    \class   GUM_InputDevice
 *    \extends GblObject
 *    \brief   GUM_InputDevice derives from GblObject, adding TODO: doc
*/
GBL_INSTANCE_DERIVE(GUM_InputDevice, GblObject)
    GblFlags      buttons;        //!< Current device buttons state
    GblFlags      buttonsPrev;    //!< Previous frame's device buttons state
    GblStringRef* deviceName;     //!< The name of the device, if it can be detected.
    GUM_Widget*   pFocusedWidget; //!< The widget this device currently has navigation focus on, or nullptr.
    uint8_t       highlight_r;    //!< Red   component of this device's focus-ring color. Default value is 255
    uint8_t       highlight_g;    //!< Green component of this device's focus-ring color. Default value is 255
    uint8_t       highlight_b;    //!< Blue  component of this device's focus-ring color. Default value is 255
    uint8_t       highlight_a;    //!< Alpha component of this device's focus-ring color. Default value is 255
GBL_INSTANCE_END

GBL_PROPERTIES(GUM_InputDevice,
    (highlight_color, GBL_GENERIC, (READ, WRITE), GBL_UINT32_TYPE),
    (highlight_r,     GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE ),
    (highlight_g,     GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE ),
    (highlight_b,     GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE ),
    (highlight_a,     GBL_GENERIC, (READ, WRITE), GBL_UINT8_TYPE )
)

GblType GUM_InputDevice_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_INPUTDEVICE_H
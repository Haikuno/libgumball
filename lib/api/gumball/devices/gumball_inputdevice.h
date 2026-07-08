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
 *    \brief   GUM_InputDevice derives from GblObject, adding nothing new.
*/
GBL_INSTANCE_DERIVE_EMPTY(GUM_InputDevice, GblObject)

GblType GUM_InputDevice_type(void) GBL_NOEXCEPT;

GBL_DECLS_END

#undef GBL_SELF_TYPE

#endif // GUM_INPUTDEVICE_H
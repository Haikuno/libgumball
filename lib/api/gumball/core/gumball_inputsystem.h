#ifndef GUM_INPUTSYSTEM_H
#define GUM_INPUTSYSTEM_H

// View this file's documentation online: https://libgumball.psyops.studio/gumball__inputsystem_8h.html

/*!  \file
 *   \brief     Input system
 *   \ingroup   core
 *
 *   \author    2026 Agustín Bellagamba
 *   \copyright MIT License
*/

#include <gimbal/preprocessor/gimbal_macro_utils.h>
#include <gumball/events/gumball_event_input.h>

typedef struct GUM_InputBinding {
    GblType  deviceType;
    GblFlags button;
} GUM_InputBinding;

GBL_DECLS_BEGIN

// Initializes the input system
GBL_RESULT GUM_InputSystem_init   (void);

// Deinitializes the input system
void       GUM_InputSystem_deinit (void);

// Updates the input system
void       GUM_InputSystem_update (void);

// Given a device type, binds an action to the passed button.
GBL_RESULT GUM_InputSystem_bind   (GblType deviceType, GUM_InputAction action, GblFlags button);

// Unbinds an action previously associated with the passed device/button pair.
GBL_RESULT GUM_InputSystem_unbind (GblType deviceType, GUM_InputAction action, GblFlags button);

//! Returns true if input is currently enabled for pDevice.
GBL_EXPORT GblBool GUM_InputSystem_deviceEnabled        (const GUM_InputDevice* pDevice) GBL_NOEXCEPT;

/*! Returns true if input is enabled for deviceType after its type gates are applied.
 *  Per-device overrides are not included.
 */
GBL_EXPORT GblBool GUM_InputSystem_deviceTypeEnabled    (GblType deviceType) GBL_NOEXCEPT;

//! Returns true if pDevice's focus ring is currently enabled.
GBL_EXPORT GblBool GUM_InputSystem_focusRingEnabled     (const GUM_InputDevice* pDevice) GBL_NOEXCEPT;

//! Returns true if focus rings are enabled for deviceType after its type gates are applied.
GBL_EXPORT GblBool GUM_InputSystem_focusRingTypeEnabled (GblType deviceType) GBL_NOEXCEPT;

////////// Implementation details, Grugs please ignore
//! \cond GRUGLESS
GBL_EXPORT GBL_RESULT GUM_InputSystem_enableDeviceInstance     (GUM_InputDevice* pDevice) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GUM_InputSystem_disableDeviceInstance    (GUM_InputDevice* pDevice) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GUM_InputSystem_enableDeviceType         (GblType deviceType)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GUM_InputSystem_disableDeviceType        (GblType deviceType)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GUM_InputSystem_enableFocusRingInstance  (GUM_InputDevice* pDevice) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GUM_InputSystem_disableFocusRingInstance (GUM_InputDevice* pDevice) GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GUM_InputSystem_enableFocusRingType      (GblType deviceType)       GBL_NOEXCEPT;
GBL_EXPORT GBL_RESULT GUM_InputSystem_disableFocusRingType     (GblType deviceType)       GBL_NOEXCEPT;

GBL_DECLS_END

GBL_INLINE GBL_RESULT GUM_InputSystem_enableDevicePtr_(void* pDevice) {
    return GUM_InputSystem_enableDeviceInstance(pDevice);
}

GBL_INLINE GBL_RESULT GUM_InputSystem_disableDevicePtr_(void* pDevice) {
    return GUM_InputSystem_disableDeviceInstance(pDevice);
}

GBL_INLINE GBL_RESULT GUM_InputSystem_enableFocusRingPtr_(void* pDevice) {
    return GUM_InputSystem_enableFocusRingInstance(pDevice);
}

GBL_INLINE GBL_RESULT GUM_InputSystem_disableFocusRingPtr_(void* pDevice) {
    return GUM_InputSystem_disableFocusRingInstance(pDevice);
}

#define GUM_INPUTSYSTEM_ENABLE_DEVICE_TRAITS_ (              \
        GUM_InputSystem_enableDevicePtr_,                    \
        (                                                     \
            (GblType, GUM_InputSystem_enableDeviceType)      \
        )                                                     \
    )

#define GUM_INPUTSYSTEM_DISABLE_DEVICE_TRAITS_ (             \
        GUM_InputSystem_disableDevicePtr_,                   \
        (                                                     \
            (GblType, GUM_InputSystem_disableDeviceType)     \
        )                                                     \
    )

#define GUM_INPUTSYSTEM_ENABLE_FOCUS_RING_TRAITS_ (          \
        GUM_InputSystem_enableFocusRingPtr_,                 \
        (                                                     \
            (GblType, GUM_InputSystem_enableFocusRingType)   \
        )                                                     \
    )

#define GUM_INPUTSYSTEM_DISABLE_FOCUS_RING_TRAITS_ (         \
        GUM_InputSystem_disableFocusRingPtr_,                \
        (                                                     \
            (GblType, GUM_InputSystem_disableFocusRingType)  \
        )                                                     \
    )
//! \endcond

/*! Enables input for one device or a whole InputDevice-derived type.
 *  Re-enabled devices establish a fresh input baseline before dispatching events.
 */
#define GUM_InputSystem_enableDevice(device) \
    GBL_META_GENERIC_MACRO_GENERATE(GUM_INPUTSYSTEM_ENABLE_DEVICE_TRAITS_, device)(device)

//! Disables input for one device or a whole InputDevice-derived type.
#define GUM_InputSystem_disableDevice(device) \
    GBL_META_GENERIC_MACRO_GENERATE(GUM_INPUTSYSTEM_DISABLE_DEVICE_TRAITS_, device)(device)

//! Enables focus-ring drawing for one device or a whole InputDevice-derived type.
#define GUM_InputSystem_enableFocusRing(device) \
    GBL_META_GENERIC_MACRO_GENERATE(GUM_INPUTSYSTEM_ENABLE_FOCUS_RING_TRAITS_, device)(device)

//! Disables focus-ring drawing for one device or a whole InputDevice-derived type.
#define GUM_InputSystem_disableFocusRing(device) \
    GBL_META_GENERIC_MACRO_GENERATE(GUM_INPUTSYSTEM_DISABLE_FOCUS_RING_TRAITS_, device)(device)

#endif // GUM_INPUTSYSTEM_H

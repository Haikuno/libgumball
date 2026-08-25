#ifndef GUMBALL_INPUTDEVICE_PRIVATE_H
#define GUMBALL_INPUTDEVICE_PRIVATE_H

#include <stdbool.h>

#include <gumball/devices/gumball_inputdevice.h>

typedef void (*GUM_InputDeviceVisitFn_)(GUM_InputDevice* pDevice, void* pClosure);

bool       GUM_InputDevice_enabled_              (const GUM_InputDevice* pDevice) GBL_NOEXCEPT;
bool       GUM_InputDevice_focusRingEnabled_     (const GUM_InputDevice* pDevice) GBL_NOEXCEPT;
bool       GUM_InputDevice_baselinePending_      (const GUM_InputDevice* pDevice) GBL_NOEXCEPT;
bool       GUM_InputDevice_takeBaseline_         (GUM_InputDevice* pDevice) GBL_NOEXCEPT;
void       GUM_InputDevice_setEnabled_           (GUM_InputDevice* pDevice, bool enabled) GBL_NOEXCEPT;
void       GUM_InputDevice_setFocusRingEnabled_  (GUM_InputDevice* pDevice, bool enabled) GBL_NOEXCEPT;
void       GUM_InputDevice_requestBaseline_      (GUM_InputDevice* pDevice) GBL_NOEXCEPT;
void       GUM_InputDevice_resetTransient_       (GUM_InputDevice* pDevice) GBL_NOEXCEPT;
GBL_RESULT GUM_InputDevice_foreach_              (GblType deviceType,
                                                  GUM_InputDeviceVisitFn_ pFnVisit,
                                                  void* pClosure) GBL_NOEXCEPT;
bool       GUM_InputDevice_focusTransitionBegin_ (GUM_InputDevice* pDevice) GBL_NOEXCEPT;
void       GUM_InputDevice_focusTransitionEnd_   (GUM_InputDevice* pDevice) GBL_NOEXCEPT;
void       GUM_InputDevice_focusRelease_         (GUM_InputDevice* pDevice) GBL_NOEXCEPT;
void       GUM_InputDevice_focusSet_             (GUM_InputDevice* pDevice, GUM_Widget* pWidget) GBL_NOEXCEPT;
GBL_RESULT GUM_InputDevice_setName_              (GUM_InputDevice* pDevice, const char* pName) GBL_NOEXCEPT;
void       GUM_InputDevice_widgetDestroyed_      (GUM_Widget* pWidget) GBL_NOEXCEPT;

#endif // GUMBALL_INPUTDEVICE_PRIVATE_H

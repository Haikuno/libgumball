#include <gumball/core/gumball_inputsystem.h>
#include <gumball/devices/gumball_gamepad.h>

void GUM_InputSystem_compileC_(GUM_InputDevice* pDevice, GUM_Gamepad* pGamepad) {
    GblType       deviceType      = GUM_GAMEPAD_TYPE;
    const GblType constDeviceType = GUM_GAMEPAD_TYPE;

    (void)GUM_InputSystem_enableDevice(pDevice);
    (void)GUM_InputSystem_enableDevice(pGamepad);
    (void)GUM_InputSystem_enableDevice(deviceType);
    (void)GUM_InputSystem_enableDevice(constDeviceType);
    (void)GUM_InputSystem_enableDevice(GUM_GAMEPAD_TYPE);
    (void)GUM_InputSystem_disableDevice(pDevice);
    (void)GUM_InputSystem_disableDevice(pGamepad);
    (void)GUM_InputSystem_disableDevice(deviceType);
    (void)GUM_InputSystem_disableDevice(constDeviceType);
    (void)GUM_InputSystem_disableDevice(GUM_GAMEPAD_TYPE);

    (void)GUM_InputSystem_enableFocusRing(pDevice);
    (void)GUM_InputSystem_enableFocusRing(pGamepad);
    (void)GUM_InputSystem_enableFocusRing(deviceType);
    (void)GUM_InputSystem_enableFocusRing(constDeviceType);
    (void)GUM_InputSystem_enableFocusRing(GUM_GAMEPAD_TYPE);
    (void)GUM_InputSystem_disableFocusRing(pDevice);
    (void)GUM_InputSystem_disableFocusRing(pGamepad);
    (void)GUM_InputSystem_disableFocusRing(deviceType);
    (void)GUM_InputSystem_disableFocusRing(constDeviceType);
    (void)GUM_InputSystem_disableFocusRing(GUM_GAMEPAD_TYPE);

    (void)GUM_InputSystem_enableDevice(nullptr);
    (void)GUM_InputSystem_disableDevice(nullptr);
    (void)GUM_InputSystem_enableFocusRing(nullptr);
    (void)GUM_InputSystem_disableFocusRing(nullptr);
}

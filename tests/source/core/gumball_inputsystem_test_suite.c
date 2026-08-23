#include "core/gumball_inputsystem_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GBL_SELF_TYPE GUM_InputSystemTestSuite

GBL_TEST_FIXTURE {
    GUM_Root*     pRoot;
    GUM_Keyboard* pKeyboard;
};

static GBL_RESULT callbackResult_     = GBL_RESULT_SUCCESS;
static GBL_RESULT restartResult_      = GBL_RESULT_SUCCESS;
static size_t     focusGainCount_     = 0;
static size_t     focusLossCount_     = 0;
static size_t     requestedGainCount_ = 0;

static void resetState_(void) {
    callbackResult_     = GBL_RESULT_SUCCESS;
    restartResult_      = GBL_RESULT_SUCCESS;
    focusGainCount_     = 0;
    focusLossCount_     = 0;
    requestedGainCount_ = 0;
}

static void countFocusLost_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pWidget, pDevice);
    ++focusLossCount_;
}

static void countFocusGained_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pWidget, pDevice);
    ++requestedGainCount_;
}

static void disableDeviceOnFocusGained_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pWidget);
    ++focusGainCount_;
    callbackResult_ = GUM_InputSystem_disableDevice(pDevice);
}

static void disableTypeOnFocusGained_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pWidget, pDevice);
    ++focusGainCount_;
    callbackResult_ = GUM_InputSystem_disableDevice(GUM_KEYBOARD_TYPE);
}

static void disableDeviceOnFocusLost_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pWidget);
    ++focusLossCount_;
    callbackResult_ = GUM_InputSystem_disableDevice(pDevice);
}

static void releaseDeviceOnFocusLost_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pWidget);
    ++focusLossCount_;
    GUM_unref(pDevice);
}

static void enableTypeOnFocusLost_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pWidget, pDevice);
    ++focusLossCount_;
    callbackResult_ = GUM_InputSystem_enableDevice(GUM_KEYBOARD_TYPE);
}

static void restartOnFocusLost_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pWidget, pDevice);
    ++focusLossCount_;
    GUM_InputSystem_deinit();
    restartResult_ = GUM_InputSystem_init();
}

GBL_TEST_INIT()
    resetState_();
    pFixture->pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pFixture->pRoot);

    pFixture->pKeyboard = GUM_Keyboard_create();
    GBL_TEST_VERIFY(pFixture->pKeyboard);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GUM_InputSystem_enableFocusRing(GUM_INPUTDEVICE_TYPE);
    GUM_InputSystem_enableFocusRing(GUM_KEYBOARD_TYPE);
    GUM_InputSystem_enableDevice(GUM_INPUTDEVICE_TYPE);
    GUM_InputSystem_enableDevice(GUM_KEYBOARD_TYPE);
    GUM_InputSystem_enableDevice(GUM_MOUSE_TYPE);
    GUM_InputSystem_enableDevice(GUM_INPUTDEVICE(pFixture->pKeyboard));
    GUM_unref(pFixture->pKeyboard);
    GUM_unref(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(deviceControls)
    GBL_TEST_COMPARE(GUM_InputSystem_enableDevice(nullptr), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_TEST_COMPARE(GUM_InputSystem_disableDevice(nullptr), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_TEST_COMPARE(GUM_InputSystem_enableFocusRing(nullptr), GBL_RESULT_ERROR_INVALID_POINTER);
    GBL_TEST_COMPARE(GUM_InputSystem_disableFocusRing(nullptr), GBL_RESULT_ERROR_INVALID_POINTER);

    GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pFixture->pKeyboard);
    GUM_Keyboard* pOtherKeyboard = GUM_Keyboard_create();
    GUM_InputDevice* pOther = GUM_INPUTDEVICE(pOtherKeyboard);
    GUM_Button* pButton = GUM_Button_create();
    GUM_Button* pOtherButton = GUM_Button_create();
    GBL_TEST_VERIFY(pOtherKeyboard && pButton && pOtherButton);

    GUM_Nav_focus(pDevice, GUM_WIDGET(pButton));
    GUM_Nav_focus(pOther, GUM_WIDGET(pOtherButton));
    pDevice->buttons     = GUM_KEYBOARD_KEY_ENTER;
    pDevice->buttonsPrev = GUM_KEYBOARD_KEY_ESCAPE;

    GBL_TEST_COMPARE(GUM_InputSystem_disableDevice(pDevice), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GUM_InputSystem_disableDevice(pDevice), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceEnabled(pDevice));
    GBL_TEST_VERIFY(GUM_InputSystem_deviceEnabled(pOther));
    GBL_TEST_VERIFY(!GUM_InputDevice_focusedWidget(pDevice));
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pOther), GUM_WIDGET(pOtherButton));
    GBL_TEST_COMPARE(pDevice->buttons, 0u);
    GBL_TEST_COMPARE(pDevice->buttonsPrev, 0u);

    GBL_TEST_COMPARE(GUM_InputSystem_enableDevice(pDevice), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GUM_InputSystem_enableDevice(pDevice), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(GUM_InputSystem_deviceEnabled(pDevice));

    GUM_Nav_focus(pOther, nullptr);
    GUM_unref(pOtherButton);
    GUM_unref(pButton);
    GUM_unref(pOtherKeyboard);
GBL_TEST_CASE_END

GBL_TEST_CASE(typeControls)
    GUM_InputDevice* pKeyboard = GUM_INPUTDEVICE(pFixture->pKeyboard);
    GUM_Keyboard* pOtherKeyboard = GUM_Keyboard_create();
    GUM_Mouse* pMouseObject = GUM_Mouse_create();
    GUM_InputDevice* pOther = GUM_INPUTDEVICE(pOtherKeyboard);
    GUM_InputDevice* pMouse = GUM_INPUTDEVICE(pMouseObject);
    GUM_Button* pKeyboardButton = GUM_Button_create();
    GUM_Button* pOtherButton = GUM_Button_create();
    GUM_Button* pMouseButton = GUM_Button_create();
    GBL_TEST_VERIFY(pOtherKeyboard && pMouseObject && pKeyboardButton && pOtherButton && pMouseButton);

    GUM_Nav_focus(pKeyboard, GUM_WIDGET(pKeyboardButton));
    GUM_Nav_focus(pOther, GUM_WIDGET(pOtherButton));
    GUM_Nav_focus(pMouse, GUM_WIDGET(pMouseButton));

    GBL_TEST_COMPARE(GUM_InputSystem_disableDevice(GUM_KEYBOARD_TYPE), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceTypeEnabled(GUM_KEYBOARD_TYPE));
    GBL_TEST_VERIFY(GUM_InputSystem_deviceTypeEnabled(GUM_MOUSE_TYPE));
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceEnabled(pKeyboard));
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceEnabled(pOther));
    GBL_TEST_VERIFY(GUM_InputSystem_deviceEnabled(pMouse));
    GBL_TEST_VERIFY(!GUM_InputDevice_focusedWidget(pKeyboard));
    GBL_TEST_VERIFY(!GUM_InputDevice_focusedWidget(pOther));
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pMouse), GUM_WIDGET(pMouseButton));

    GBL_TEST_COMPARE(GUM_InputSystem_disableDevice(GUM_INPUTDEVICE_TYPE), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceTypeEnabled(GUM_MOUSE_TYPE));
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceEnabled(pMouse));

    GBL_TEST_COMPARE(GUM_InputSystem_enableDevice(GUM_INPUTDEVICE_TYPE), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(GUM_InputSystem_deviceTypeEnabled(GUM_MOUSE_TYPE));
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceTypeEnabled(GUM_KEYBOARD_TYPE));
    GBL_TEST_VERIFY(GUM_InputSystem_deviceEnabled(pMouse));
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceEnabled(pKeyboard));

    GBL_TEST_COMPARE(GUM_InputSystem_enableDevice(GUM_KEYBOARD_TYPE), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(GUM_InputSystem_deviceTypeEnabled(GUM_KEYBOARD_TYPE));
    GBL_TEST_VERIFY(GUM_InputSystem_deviceEnabled(pKeyboard));
    GBL_TEST_VERIFY(GUM_InputSystem_deviceEnabled(pOther));

    GUM_unref(pMouseButton);
    GUM_unref(pOtherButton);
    GUM_unref(pKeyboardButton);
    GUM_unref(pMouseObject);
    GUM_unref(pOtherKeyboard);
GBL_TEST_CASE_END

GBL_TEST_CASE(focusRingControls)
    GUM_InputDevice* pKeyboard = GUM_INPUTDEVICE(pFixture->pKeyboard);
    GUM_Keyboard* pOtherKeyboard = GUM_Keyboard_create();
    GUM_Mouse* pMouseObject = GUM_Mouse_create();
    GUM_InputDevice* pOther = GUM_INPUTDEVICE(pOtherKeyboard);
    GUM_InputDevice* pMouse = GUM_INPUTDEVICE(pMouseObject);
    GUM_Button* pKeyboardButton = GUM_Button_create();
    GUM_Button* pOtherButton = GUM_Button_create();
    GUM_Button* pMouseButton = GUM_Button_create();
    GBL_TEST_VERIFY(pOtherKeyboard && pMouseObject && pKeyboardButton && pOtherButton && pMouseButton);

    GUM_Nav_focus(pKeyboard, GUM_WIDGET(pKeyboardButton));
    GUM_Nav_focus(pOther, GUM_WIDGET(pOtherButton));
    GUM_Nav_focus(pMouse, GUM_WIDGET(pMouseButton));

    GBL_TEST_COMPARE(GUM_InputSystem_disableFocusRing(pKeyboard), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_InputSystem_focusRingEnabled(pKeyboard));
    GBL_TEST_VERIFY(GUM_InputSystem_deviceEnabled(pKeyboard));
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pKeyboard), GUM_WIDGET(pKeyboardButton));
    GBL_TEST_COMPARE(GUM_InputSystem_enableFocusRing(pKeyboard), GBL_RESULT_SUCCESS);

    GBL_TEST_COMPARE(GUM_InputSystem_disableFocusRing(GUM_KEYBOARD_TYPE), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_InputSystem_focusRingTypeEnabled(GUM_KEYBOARD_TYPE));
    GBL_TEST_VERIFY(!GUM_InputSystem_focusRingEnabled(pKeyboard));
    GBL_TEST_VERIFY(!GUM_InputSystem_focusRingEnabled(pOther));
    GBL_TEST_VERIFY(GUM_InputSystem_focusRingEnabled(pMouse));
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pKeyboard), GUM_WIDGET(pKeyboardButton));

    GBL_TEST_COMPARE(GUM_InputSystem_disableFocusRing(GUM_INPUTDEVICE_TYPE), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_InputSystem_focusRingEnabled(pMouse));
    GBL_TEST_COMPARE(GUM_InputSystem_enableFocusRing(GUM_INPUTDEVICE_TYPE), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(GUM_InputSystem_focusRingEnabled(pMouse));
    GBL_TEST_VERIFY(!GUM_InputSystem_focusRingEnabled(pKeyboard));

    GBL_TEST_COMPARE(GUM_InputSystem_enableFocusRing(GUM_KEYBOARD_TYPE), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(GUM_InputSystem_focusRingEnabled(pKeyboard));
    GBL_TEST_VERIFY(GUM_InputSystem_focusRingEnabled(pOther));

    GUM_Nav_focus(pKeyboard, nullptr);
    GUM_Nav_focus(pOther, nullptr);
    GUM_Nav_focus(pMouse, nullptr);
    GUM_unref(pMouseButton);
    GUM_unref(pOtherButton);
    GUM_unref(pKeyboardButton);
    GUM_unref(pMouseObject);
    GUM_unref(pOtherKeyboard);
GBL_TEST_CASE_END

GBL_TEST_CASE(focusDisable)
    resetState_();
    GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pFixture->pKeyboard);
    GUM_Button* pInstance = GUM_Button_create();
    GUM_Button* pType = GUM_Button_create();
    GBL_TEST_VERIFY(pInstance && pType);

    GUM_connect(pInstance,
                "onFocusGained", disableDeviceOnFocusGained_,
                "onFocusLost",   countFocusLost_);
    GUM_Nav_focus(pDevice, GUM_WIDGET(pInstance));

    GBL_TEST_COMPARE(focusGainCount_, 1u);
    GBL_TEST_COMPARE(focusLossCount_, 1u);
    GBL_TEST_COMPARE(callbackResult_, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceEnabled(pDevice));
    GBL_TEST_VERIFY(!GUM_InputDevice_focusedWidget(pDevice));

    GBL_TEST_COMPARE(GUM_InputSystem_enableDevice(pDevice), GBL_RESULT_SUCCESS);
    resetState_();

    GUM_connect(pType,
                "onFocusGained", disableTypeOnFocusGained_,
                "onFocusLost",   countFocusLost_);
    GUM_Nav_focus(pDevice, GUM_WIDGET(pType));

    GBL_TEST_COMPARE(focusGainCount_, 1u);
    GBL_TEST_COMPARE(focusLossCount_, 1u);
    GBL_TEST_COMPARE(callbackResult_, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceTypeEnabled(GUM_KEYBOARD_TYPE));
    GBL_TEST_VERIFY(!GUM_InputDevice_focusedWidget(pDevice));
    GBL_TEST_COMPARE(GUM_InputSystem_enableDevice(GUM_KEYBOARD_TYPE), GBL_RESULT_SUCCESS);

    GUM_unref(pType);
    GUM_unref(pInstance);
GBL_TEST_CASE_END

GBL_TEST_CASE(focusLoss)
    resetState_();
    GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pFixture->pKeyboard);
    GUM_Button* pOld = GUM_Button_create();
    GUM_Button* pRequested = GUM_Button_create();
    GBL_TEST_VERIFY(pOld && pRequested);

    GUM_Nav_focus(pDevice, GUM_WIDGET(pOld));
    GUM_connect(pOld, "onFocusLost", disableDeviceOnFocusLost_);
    GUM_connect(pRequested, "onFocusGained", countFocusGained_);
    GUM_Nav_focus(pDevice, GUM_WIDGET(pRequested));

    GBL_TEST_COMPARE(focusLossCount_, 1u);
    GBL_TEST_COMPARE(requestedGainCount_, 0u);
    GBL_TEST_COMPARE(callbackResult_, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceEnabled(pDevice));
    GBL_TEST_VERIFY(!GUM_InputDevice_focusedWidget(pDevice));

    GBL_TEST_COMPARE(GUM_InputSystem_enableDevice(pDevice), GBL_RESULT_SUCCESS);
    GUM_unref(pRequested);
    GUM_unref(pOld);

    resetState_();
    GUM_Keyboard* pVictim = GUM_Keyboard_create();
    pOld = GUM_Button_create();
    pRequested = GUM_Button_create();
    GBL_TEST_VERIFY(pVictim && pOld && pRequested);

    GUM_Nav_focus(GUM_INPUTDEVICE(pVictim), GUM_WIDGET(pOld));
    GUM_connect(pOld, "onFocusLost", releaseDeviceOnFocusLost_);
    GUM_connect(pRequested, "onFocusGained", countFocusGained_);
    GUM_Nav_focus(GUM_INPUTDEVICE(pVictim), GUM_WIDGET(pRequested));

    GBL_TEST_COMPARE(focusLossCount_, 1u);
    GBL_TEST_COMPARE(requestedGainCount_, 0u);
    GBL_TEST_VERIFY(!GUM_Widget_isFocused(GUM_WIDGET(pOld)));
    GBL_TEST_VERIFY(!GUM_Widget_isFocused(GUM_WIDGET(pRequested)));

    GUM_unref(pRequested);
    GUM_unref(pOld);
GBL_TEST_CASE_END

GBL_TEST_CASE(typeReentry)
    resetState_();
    GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pFixture->pKeyboard);
    GUM_Button* pButton = GUM_Button_create();
    GBL_TEST_VERIFY(pButton);

    GUM_Nav_focus(pDevice, GUM_WIDGET(pButton));
    GUM_connect(pButton, "onFocusLost", enableTypeOnFocusLost_);

    GBL_TEST_COMPARE(GUM_InputSystem_disableDevice(GUM_KEYBOARD_TYPE), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(focusLossCount_, 1u);
    GBL_TEST_COMPARE(callbackResult_, GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_TEST_VERIFY(!GUM_InputSystem_deviceTypeEnabled(GUM_KEYBOARD_TYPE));
    GBL_TEST_VERIFY(!GUM_InputDevice_focusedWidget(pDevice));
    GBL_TEST_COMPARE(GUM_InputSystem_enableDevice(GUM_KEYBOARD_TYPE), GBL_RESULT_SUCCESS);

    GUM_unref(pButton);
GBL_TEST_CASE_END

GBL_TEST_CASE(restart)
    resetState_();
    GUM_InputSystem_deinit();
    GUM_InputSystem_deinit();
    GBL_TEST_COMPARE(GUM_InputSystem_bind(GUM_KEYBOARD_TYPE,
                                          GUM_INPUTACTION_CONFIRM,
                                          GUM_KEYBOARD_KEY_ENTER),
                     GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_TEST_COMPARE(GUM_InputSystem_init(), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GUM_InputSystem_init(), GBL_RESULT_SUCCESS);

    GUM_Button* pTrigger = GUM_Button_create();
    GBL_TEST_VERIFY(pTrigger);

    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_WIDGET(pTrigger));
    GUM_connect(pTrigger, "onFocusLost", restartOnFocusLost_);

    GBL_TEST_COMPARE(GUM_InputSystem_disableDevice(GUM_KEYBOARD_TYPE),
                     GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_TEST_COMPARE(restartResult_, GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(focusLossCount_, 1u);
    GBL_TEST_VERIFY(GUM_InputSystem_deviceTypeEnabled(GUM_KEYBOARD_TYPE));
    GBL_TEST_VERIFY(!GUM_InputDevice_focusedWidget(GUM_INPUTDEVICE(pFixture->pKeyboard)));

    GUM_unref(pTrigger);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(deviceControls,
                  typeControls,
                  focusRingControls,
                  focusDisable,
                  focusLoss,
                  typeReentry,
                  restart)

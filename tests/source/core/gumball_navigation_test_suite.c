#include "core/gumball_navigation_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GBL_SELF_TYPE GUM_NavigationTestSuite

GBL_TEST_FIXTURE {
    GUM_Root* pRoot;
    GUM_Keyboard* pKeyboard;
    GUM_Button* pFirst;
    GUM_Button* pSecond;
    GUM_Button* pThird;
};

static unsigned inputSignalCount_ = 0;

static void GUM_NavigationTestSuite_inputSignal_(GUM_Widget* pWidget) {
    GBL_UNUSED(pWidget);
    ++inputSignalCount_;
}

GBL_TEST_INIT()
    pFixture->pRoot = GUM_Root_create();
    pFixture->pKeyboard = GUM_Keyboard_create();

    GUM_Container* pContainer = GUM_Container_create("w", 300.0f,
                                                     "h", 100.0f,
                                                     "padding", 0.0f,
                                                     "margin", 0.0f,
                                                     "direction", GUM_DIRECTION_HORIZONTAL);

    pFixture->pFirst  = GUM_Button_create("parent", pContainer);
    pFixture->pSecond = GUM_Button_create("parent", pContainer,
                                          "isSelectedByDefault", true);
    pFixture->pThird  = GUM_Button_create("parent", pContainer);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GUM_unref(pFixture->pKeyboard);
    GUM_unref(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(defaultFocus)
    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), nullptr);
    GUM_Nav_move(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_INPUTACTION_MOVE_RIGHT);
    GBL_TEST_COMPARE(GUM_INPUTDEVICE(pFixture->pKeyboard)->pFocusedWidget,
                     GUM_WIDGET(pFixture->pSecond));
GBL_TEST_CASE_END

GBL_TEST_CASE(moveRight)
    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_WIDGET(pFixture->pSecond));
    GUM_Nav_move(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_INPUTACTION_MOVE_RIGHT);
    GBL_TEST_COMPARE(GUM_INPUTDEVICE(pFixture->pKeyboard)->pFocusedWidget,
                     GUM_WIDGET(pFixture->pThird));
GBL_TEST_CASE_END

GBL_TEST_CASE(moveLeft)
    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_WIDGET(pFixture->pThird));
    GUM_Nav_move(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_INPUTACTION_MOVE_LEFT);
    GBL_TEST_COMPARE(GUM_INPUTDEVICE(pFixture->pKeyboard)->pFocusedWidget,
                     GUM_WIDGET(pFixture->pSecond));
GBL_TEST_CASE_END

GBL_TEST_CASE(explicitFocus)
    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), nullptr);
    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_WIDGET(pFixture->pFirst));
    GBL_TEST_COMPARE(GUM_INPUTDEVICE(pFixture->pKeyboard)->pFocusedWidget,
                     GUM_WIDGET(pFixture->pFirst));
GBL_TEST_CASE_END

GBL_TEST_CASE(nonContainerAncestor)
    GUM_Widget* pWrapper = GUM_Widget_create("w", 200.0f, "h", 100.0f);
    GUM_Container* pContainer = GUM_Container_create("parent", pWrapper,
                                                     "w", 200.0f,
                                                     "h", 100.0f,
                                                     "padding", 0.0f,
                                                     "margin", 0.0f,
                                                     "direction", GUM_DIRECTION_HORIZONTAL);
    GUM_Button* pLeft = GUM_Button_create("parent", pContainer);
    GUM_Button* pRight = GUM_Button_create("parent", pContainer);

    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_WIDGET(pLeft));
    GUM_Nav_move(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_INPUTACTION_MOVE_RIGHT);
    GBL_TEST_COMPARE(GUM_INPUTDEVICE(pFixture->pKeyboard)->pFocusedWidget,
                     GUM_WIDGET(pRight));
GBL_TEST_CASE_END

GBL_TEST_CASE(pointerTargetContract)
    const GUM_Vector2 point = { 20.0f, 120.0f };

    GUM_Widget* pUnderlay = GUM_Widget_create("x", 10.0f,
                                              "y", 110.0f,
                                              "w", 40.0f,
                                              "h", 40.0f,
                                              "z_index", (uint8_t)60,
                                              "isActive", true,
                                              "isSelectable", true,
                                              "isInteractive", true);
    GUM_Widget* pOverlay = GUM_Widget_create("x", 10.0f,
                                             "y", 110.0f,
                                             "w", 40.0f,
                                             "h", 40.0f,
                                             "z_index", (uint8_t)70,
                                             "isActive", true,
                                             "isSelectable", false,
                                             "isInteractive", false);

    GBL_TEST_COMPARE(GUM_InputSystem_pointerTargetAt_(point), pUnderlay);

    pOverlay->isInteractive = true;
    GBL_TEST_COMPARE(GUM_InputSystem_pointerTargetAt_(point), pOverlay);
    GBL_TEST_VERIFY(!pOverlay->isSelectable);

    inputSignalCount_ = 0;
    GUM_connect(pOverlay, "onPressConfirm", GUM_NavigationTestSuite_inputSignal_);

    GUM_Event_Input* pEvent = GUM_EVENT_INPUT(GblEvent_create(GUM_EVENT_INPUT_TYPE));
    GBL_TEST_VERIFY(pEvent);
    pEvent->action = GUM_INPUTACTION_CONFIRM;
    pEvent->state = GUM_INPUTSTATE_PRESS;

    GUM_Widget* pTarget = GUM_InputSystem_pointerTargetAt_(point);
    GBL_TEST_COMPARE(GUM_WIDGET_CLASSOF(pTarget)->pFnInputEvent(pTarget, pEvent),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(inputSignalCount_, 1u);

    GBL_UNREF(pEvent);
GBL_TEST_CASE_END

GBL_TEST_CASE(mouseEventSnapshot)
    GUM_Mouse* pMouse = GUM_Mouse_create();
    GBL_TEST_VERIFY(pMouse);

    pMouse->position = (GUM_Vector2){ 12.5f, 24.0f };
    pMouse->delta    = (GUM_Vector2){ -3.0f, 4.5f };
    pMouse->wheel    = (GUM_Vector2){ 1.0f, -2.0f };

    GUM_Event_Mouse* pEvent = GUM_Event_Mouse_createFrom(pMouse);
    GBL_TEST_VERIFY(pEvent);
    GBL_TEST_VERIFY(GblType_check(GBL_TYPEOF(pEvent), GUM_EVENT_POINTER_TYPE));
    GBL_TEST_COMPARE(GUM_EVENT_INPUT(pEvent)->pInputDevice, GUM_INPUTDEVICE(pMouse));
    GBL_TEST_COMPARE(GUM_EVENT_POINTER(pEvent)->position.x, pMouse->position.x);
    GBL_TEST_COMPARE(GUM_EVENT_POINTER(pEvent)->position.y, pMouse->position.y);
    GBL_TEST_COMPARE(pEvent->delta.x, pMouse->delta.x);
    GBL_TEST_COMPARE(pEvent->delta.y, pMouse->delta.y);
    GBL_TEST_COMPARE(pEvent->wheel.x, pMouse->wheel.x);
    GBL_TEST_COMPARE(pEvent->wheel.y, pMouse->wheel.y);

    pMouse->position = (GUM_Vector2){ 100.0f, 200.0f };
    GBL_TEST_COMPARE(GUM_EVENT_POINTER(pEvent)->position.x, 12.5f);
    GBL_TEST_COMPARE(GUM_EVENT_POINTER(pEvent)->position.y, 24.0f);

    GBL_UNREF(pEvent);
    GUM_unref(pMouse);
GBL_TEST_CASE_END

GBL_TEST_CASE(inputSignalContract)
    GUM_Widget* pWidget = GUM_WIDGET(pFixture->pSecond);
    pWidget->isActive = true;
    inputSignalCount_ = 0;

    GUM_connect(pWidget,
                "onPressMoveUp",      GUM_NavigationTestSuite_inputSignal_,
                "onPressMoveDown",    GUM_NavigationTestSuite_inputSignal_,
                "onPressMoveLeft",    GUM_NavigationTestSuite_inputSignal_,
                "onPressMoveRight",   GUM_NavigationTestSuite_inputSignal_,
                "onReleaseMoveUp",    GUM_NavigationTestSuite_inputSignal_,
                "onReleaseMoveDown",  GUM_NavigationTestSuite_inputSignal_,
                "onReleaseMoveLeft",  GUM_NavigationTestSuite_inputSignal_,
                "onReleaseMoveRight", GUM_NavigationTestSuite_inputSignal_);

    GUM_Event_Input* pEvent = GUM_EVENT_INPUT(GblEvent_create(GUM_EVENT_INPUT_TYPE));
    GBL_TEST_VERIFY(pEvent);

    for (GUM_InputAction action = GUM_INPUTACTION_MOVE_UP;
         action <= GUM_INPUTACTION_MOVE_RIGHT;
         ++action) {
        pEvent->action = action;
        pEvent->state = GUM_INPUTSTATE_PRESS;
        GBL_TEST_COMPARE(GUM_WIDGET_CLASSOF(pWidget)->pFnInputEvent(pWidget, pEvent),
                         GBL_RESULT_SUCCESS);

        pEvent->state = GUM_INPUTSTATE_RELEASE;
        GBL_TEST_COMPARE(GUM_WIDGET_CLASSOF(pWidget)->pFnInputEvent(pWidget, pEvent),
                         GBL_RESULT_SUCCESS);
    }

    GBL_TEST_COMPARE(inputSignalCount_, 8u);

    pWidget->isInteractive = false;
    pEvent->action = GUM_INPUTACTION_MOVE_RIGHT;
    pEvent->state = GUM_INPUTSTATE_PRESS;
    GBL_TEST_COMPARE(GUM_WIDGET_CLASSOF(pWidget)->pFnInputEvent(pWidget, pEvent),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(inputSignalCount_, 8u);

    pWidget->isInteractive = true;
    pEvent->action = GUM_INPUTACTION_COUNT;
    GBL_TEST_COMPARE(GUM_WIDGET_CLASSOF(pWidget)->pFnInputEvent(pWidget, pEvent),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(inputSignalCount_, 8u);

    GBL_UNREF(pEvent);
GBL_TEST_CASE_END

GBL_TEST_CASE(deviceDestructionClearsFocus)
    GUM_Widget* pWidget = GUM_WIDGET(pFixture->pSecond);
    const uint8_t initialFocusCount = pWidget->focusCount;

    GUM_Keyboard* pKeyboard = GUM_Keyboard_create();
    GBL_TEST_VERIFY(pKeyboard);

    GUM_Nav_focus(GUM_INPUTDEVICE(pKeyboard), pWidget);
    GBL_TEST_COMPARE(pWidget->focusCount, initialFocusCount + 1);

    GUM_unref(pKeyboard);
    GBL_TEST_COMPARE(pWidget->focusCount, initialFocusCount);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(defaultFocus,
                  moveRight,
                  moveLeft,
                  explicitFocus,
                  nonContainerAncestor,
                  pointerTargetContract,
                  mouseEventSnapshot,
                  inputSignalContract,
                  deviceDestructionClearsFocus)

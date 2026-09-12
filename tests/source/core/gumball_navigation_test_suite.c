#include "core/gumball_navigation_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GBL_SELF_TYPE GUM_NavigationTestSuite

GBL_TEST_FIXTURE {
    GUM_Root*      pRoot;
    GUM_Keyboard*  pKeyboard;
    GUM_Container* pContainer;
    GUM_Button*    pFirst;
    GUM_Button*    pSecond;
    GUM_Button*    pThird;
};

static unsigned   inputSignalCount_       = 0;
static unsigned   nestedFocusCount_       = 0;
static unsigned   widgetUnrefCount_       = 0;
static unsigned   deviceUnrefCount_       = 0;
static GUM_Widget* pNestedFocusTarget_     = nullptr;

static void onInput_(GUM_Widget* pWidget) {
    GBL_UNUSED(pWidget);
    ++inputSignalCount_;
}

static void onNestedFocus_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pWidget);
    ++nestedFocusCount_;
    GUM_Nav_focus(pDevice, pNestedFocusTarget_);
}

static void onWidgetUnref_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pDevice);
    ++widgetUnrefCount_;
    GUM_unref(pWidget);
}

static void onDeviceUnref_(GUM_Widget* pWidget, GUM_InputDevice* pDevice) {
    GBL_UNUSED(pWidget);
    ++deviceUnrefCount_;
    GUM_unref(pDevice);
}

static void resetFocus_(void) {
    pNestedFocusTarget_ = nullptr;
    nestedFocusCount_   = 0;
    widgetUnrefCount_   = 0;
    deviceUnrefCount_   = 0;
}

GBL_TEST_INIT()
    resetFocus_();
    pFixture->pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pFixture->pRoot);

    pFixture->pKeyboard = GUM_Keyboard_create();
    GBL_TEST_VERIFY(pFixture->pKeyboard);

    pFixture->pContainer = GUM_Container_create("w", 300.0f,
                                                "h", 100.0f,
                                                "padding", 0.0f,
                                                "margin", 0.0f,
                                                "direction", GUM_DIRECTION_HORIZONTAL);
    GBL_TEST_VERIFY(pFixture->pContainer);

    pFixture->pFirst  = GUM_Button_create("parent", pFixture->pContainer);
    pFixture->pSecond = GUM_Button_create("parent", pFixture->pContainer,
                                          "isSelectedByDefault", true);
    pFixture->pThird  = GUM_Button_create("parent", pFixture->pContainer);
    GBL_TEST_VERIFY(pFixture->pFirst && pFixture->pSecond && pFixture->pThird);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    pNestedFocusTarget_ = nullptr;
    GUM_unref(pFixture->pKeyboard);
    GUM_unref(pFixture->pContainer);
    GUM_unref(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(move)
    GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pFixture->pKeyboard);

    GUM_Nav_focus(pDevice, nullptr);
    GUM_Nav_move(pDevice, GUM_INPUTACTION_MOVE_RIGHT);
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pDevice), GUM_WIDGET(pFixture->pSecond));

    GUM_Nav_move(pDevice, GUM_INPUTACTION_MOVE_RIGHT);
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pDevice), GUM_WIDGET(pFixture->pThird));

    GUM_Nav_move(pDevice, GUM_INPUTACTION_MOVE_LEFT);
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pDevice), GUM_WIDGET(pFixture->pSecond));
GBL_TEST_CASE_END

GBL_TEST_CASE(focus)
    GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pFixture->pKeyboard);

    GUM_Nav_focus(pDevice, GUM_WIDGET(pFixture->pFirst));
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pDevice), GUM_WIDGET(pFixture->pFirst));
    GBL_TEST_VERIFY(GUM_Widget_isFocused(GUM_WIDGET(pFixture->pFirst)));

    GUM_Nav_focus(pDevice, nullptr);
    GBL_TEST_VERIFY(!GUM_InputDevice_focusedWidget(pDevice));
    GBL_TEST_VERIFY(!GUM_Widget_isFocused(GUM_WIDGET(pFixture->pFirst)));
GBL_TEST_CASE_END

GBL_TEST_CASE(focusReentry)
    GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pFixture->pKeyboard);
    GUM_Widget* pFirst  = GUM_WIDGET(pFixture->pFirst);
    GUM_Widget* pSecond = GUM_WIDGET(pFixture->pSecond);
    GUM_Widget* pThird  = GUM_WIDGET(pFixture->pThird);

    GUM_Nav_focus(pDevice, pFirst);
    pNestedFocusTarget_ = pThird;
    GUM_connect(pFirst, "onFocusLost", onNestedFocus_);
    GUM_Nav_focus(pDevice, pSecond);

    GBL_TEST_COMPARE(nestedFocusCount_, 1u);
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pDevice), pSecond);
    GBL_TEST_VERIFY(!GUM_Widget_isFocused(pFirst));
    GBL_TEST_VERIFY(!GUM_Widget_isFocused(pThird));

    GblSignal_disconnect(GBL_INSTANCE(pFirst), "onFocusLost", GBL_INSTANCE(pFirst), nullptr);
    resetFocus_();
    GUM_Nav_focus(pDevice, pFirst);
    pNestedFocusTarget_ = pThird;
    GUM_connect(pSecond, "onFocusGained", onNestedFocus_);
    GUM_Nav_focus(pDevice, pSecond);

    GBL_TEST_COMPARE(nestedFocusCount_, 1u);
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pDevice), pSecond);
    GBL_TEST_VERIFY(!GUM_Widget_isFocused(pThird));

    GblSignal_disconnect(GBL_INSTANCE(pSecond), "onFocusGained", GBL_INSTANCE(pSecond), nullptr);
    resetFocus_();
    GUM_Nav_focus(pDevice, nullptr);
    pNestedFocusTarget_ = pSecond;
    GUM_connect(pSecond, "onFocusGained", onNestedFocus_);
    GUM_Nav_focus(pDevice, pSecond);

    GBL_TEST_COMPARE(nestedFocusCount_, 1u);
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pDevice), pSecond);
    GBL_TEST_VERIFY(GUM_Widget_isFocused(pSecond));
GBL_TEST_CASE_END

GBL_TEST_CASE(focusLifetime)
    GUM_InputDevice* pDevice = GUM_INPUTDEVICE(pFixture->pKeyboard);
    GUM_Widget* pFirst = GUM_WIDGET(pFixture->pFirst);

    GUM_Button* pVictim = GUM_Button_create();
    GBL_TEST_VERIFY(pVictim);
    GblObject_setParent(GBL_OBJECT(pVictim), nullptr);
    GUM_Nav_focus(pDevice, GUM_WIDGET(pVictim));
    GUM_connect(pVictim, "onFocusLost", onWidgetUnref_);
    GUM_Nav_focus(pDevice, pFirst);

    GBL_TEST_COMPARE(widgetUnrefCount_, 1u);
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(pDevice), pFirst);

    widgetUnrefCount_ = 0;
    pVictim = GUM_Button_create();
    GBL_TEST_VERIFY(pVictim);
    GblObject_setParent(GBL_OBJECT(pVictim), nullptr);
    GUM_connect(pVictim, "onFocusGained", onWidgetUnref_);
    GUM_Nav_focus(pDevice, GUM_WIDGET(pVictim));

    GBL_TEST_COMPARE(widgetUnrefCount_, 1u);
    GBL_TEST_VERIFY(!GUM_InputDevice_focusedWidget(pDevice));

    GUM_Keyboard* pKeyboard = GUM_Keyboard_create();
    GBL_TEST_VERIFY(pKeyboard);
    pDevice = GUM_INPUTDEVICE(pKeyboard);
    GUM_Nav_focus(pDevice, pFirst);
    GUM_connect(pFirst, "onFocusLost", onDeviceUnref_);
    GUM_Nav_focus(pDevice, GUM_WIDGET(pFixture->pSecond));

    GBL_TEST_COMPARE(deviceUnrefCount_, 1u);
    GBL_TEST_VERIFY(!GUM_Widget_isFocused(pFirst));
    GBL_TEST_VERIFY(!GUM_Widget_isFocused(GUM_WIDGET(pFixture->pSecond)));

    GblSignal_disconnect(GBL_INSTANCE(pFirst), "onFocusLost", GBL_INSTANCE(pFirst), nullptr);
    pKeyboard = GUM_Keyboard_create();
    GBL_TEST_VERIFY(pKeyboard);
    pDevice = GUM_INPUTDEVICE(pKeyboard);
    GUM_Nav_focus(pDevice, GUM_WIDGET(pFixture->pSecond));
    GUM_unref(pKeyboard);
    GBL_TEST_VERIFY(!GUM_Widget_isFocused(GUM_WIDGET(pFixture->pSecond)));
GBL_TEST_CASE_END

GBL_TEST_CASE(focusCount)
    GUM_Widget* pWidget = GUM_WIDGET(pFixture->pSecond);
    GUM_Keyboard* pOther = GUM_Keyboard_create();
    GBL_TEST_VERIFY(pOther);

    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), pWidget);
    GUM_Nav_focus(GUM_INPUTDEVICE(pOther), pWidget);
    GBL_TEST_VERIFY(GUM_Widget_isFocused(pWidget));

    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), nullptr);
    GBL_TEST_VERIFY(GUM_Widget_isFocused(pWidget));

    GUM_Nav_focus(GUM_INPUTDEVICE(pOther), nullptr);
    GBL_TEST_VERIFY(!GUM_Widget_isFocused(pWidget));
    GUM_unref(pOther);
GBL_TEST_CASE_END

GBL_TEST_CASE(nonContainerAncestor)
    GUM_Widget* pWrapper = GUM_Widget_create("w", 200.0f, "h", 100.0f);
    GUM_Container* pContainer = GUM_Container_create("parent", pWrapper,
                                                     "w", 200.0f,
                                                     "h", 100.0f,
                                                     "padding", 0.0f,
                                                     "margin", 0.0f,
                                                     "direction", GUM_DIRECTION_HORIZONTAL);
    GUM_Button* pLeft  = GUM_Button_create("parent", pContainer);
    GUM_Button* pRight = GUM_Button_create("parent", pContainer);
    GBL_TEST_VERIFY(pWrapper && pContainer && pLeft && pRight);

    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_WIDGET(pLeft));
    GUM_Nav_move(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_INPUTACTION_MOVE_RIGHT);
    GBL_TEST_COMPARE(GUM_InputDevice_focusedWidget(GUM_INPUTDEVICE(pFixture->pKeyboard)),
                     GUM_WIDGET(pRight));

    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), nullptr);

    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), nullptr);
    GUM_unref(pWrapper);
GBL_TEST_CASE_END

GBL_TEST_CASE(mouseEvent)
    GUM_Mouse* pMouse = GUM_Mouse_create();
    GBL_TEST_VERIFY(pMouse);

    GUM_Pointer* pPointer = GUM_POINTER(pMouse);
    pPointer->position = (GUM_Vector2){ 12.5f, 24.0f };
    pPointer->delta    = (GUM_Vector2){ -3.0f, 4.5f };
    pMouse->wheel      = (GUM_Vector2){ 1.0f, -2.0f };

    GUM_Event_Mouse* pEvent = GUM_Event_Mouse_createFrom(pMouse);
    GBL_TEST_VERIFY(pEvent);
    GBL_TEST_COMPARE(GUM_EVENT_INPUT(pEvent)->pInputDevice, GUM_INPUTDEVICE(pMouse));
    GBL_TEST_COMPARE(GUM_EVENT_POINTER(pEvent)->position.x, 12.5f);
    GBL_TEST_COMPARE(GUM_EVENT_POINTER(pEvent)->position.y, 24.0f);
    GBL_TEST_COMPARE(GUM_EVENT_POINTER(pEvent)->delta.x, -3.0f);
    GBL_TEST_COMPARE(GUM_EVENT_POINTER(pEvent)->delta.y, 4.5f);
    GBL_TEST_COMPARE(pEvent->wheel.x, 1.0f);
    GBL_TEST_COMPARE(pEvent->wheel.y, -2.0f);

    pPointer->position = (GUM_Vector2){ 100.0f, 200.0f };
    pPointer->delta    = (GUM_Vector2){ 50.0f, 60.0f };
    GBL_TEST_COMPARE(GUM_EVENT_POINTER(pEvent)->position.x, 12.5f);
    GBL_TEST_COMPARE(GUM_EVENT_POINTER(pEvent)->delta.x, -3.0f);

    GBL_UNREF(pEvent);
    GUM_unref(pMouse);
GBL_TEST_CASE_END

GBL_TEST_CASE(inputSignals)
    GUM_Widget* pWidget = GUM_WIDGET(pFixture->pSecond);
    GBL_TEST_COMPARE(GUM_setProperty(pWidget, "isActive", true), GBL_RESULT_SUCCESS);
    inputSignalCount_ = 0;

    GUM_connect(pWidget,
                "onPressMoveUp",      onInput_,
                "onPressMoveDown",    onInput_,
                "onPressMoveLeft",    onInput_,
                "onPressMoveRight",   onInput_,
                "onReleaseMoveUp",    onInput_,
                "onReleaseMoveDown",  onInput_,
                "onReleaseMoveLeft",  onInput_,
                "onReleaseMoveRight", onInput_);

    GUM_Event_Input* pEvent = GUM_EVENT_INPUT(GblEvent_create(GUM_EVENT_INPUT_TYPE));
    GBL_TEST_VERIFY(pEvent);

    for (GUM_InputAction action = GUM_INPUTACTION_MOVE_UP;
         action <= GUM_INPUTACTION_MOVE_RIGHT;
         ++action) {
        pEvent->action = action;
        pEvent->state  = GUM_INPUTSTATE_PRESS;
        GBL_TEST_COMPARE(GUM_WIDGET_CLASSOF(pWidget)->pFnInputEvent(pWidget, pEvent),
                         GBL_RESULT_SUCCESS);

        pEvent->state = GUM_INPUTSTATE_RELEASE;
        GBL_TEST_COMPARE(GUM_WIDGET_CLASSOF(pWidget)->pFnInputEvent(pWidget, pEvent),
                         GBL_RESULT_SUCCESS);
    }

    GBL_TEST_COMPARE(inputSignalCount_, 8u);

    pWidget->isInteractive = false;
    pEvent->action = GUM_INPUTACTION_MOVE_RIGHT;
    pEvent->state  = GUM_INPUTSTATE_PRESS;
    GBL_TEST_COMPARE(GUM_WIDGET_CLASSOF(pWidget)->pFnInputEvent(pWidget, pEvent),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(inputSignalCount_, 8u);

    GBL_UNREF(pEvent);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(move,
                  focus,
                  focusReentry,
                  focusLifetime,
                  focusCount,
                  nonContainerAncestor,
                  mouseEvent,
                  inputSignals)

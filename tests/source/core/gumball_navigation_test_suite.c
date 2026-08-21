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
    GUM_Nav_move(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_INPUTACTION_MOVE_RIGHT);
    GBL_TEST_COMPARE(GUM_INPUTDEVICE(pFixture->pKeyboard)->pFocusedWidget,
                     GUM_WIDGET(pFixture->pSecond));
GBL_TEST_CASE_END

GBL_TEST_CASE(moveRight)
    GUM_Nav_move(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_INPUTACTION_MOVE_RIGHT);
    GBL_TEST_COMPARE(GUM_INPUTDEVICE(pFixture->pKeyboard)->pFocusedWidget,
                     GUM_WIDGET(pFixture->pThird));
GBL_TEST_CASE_END

GBL_TEST_CASE(moveLeft)
    GUM_Nav_move(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_INPUTACTION_MOVE_LEFT);
    GBL_TEST_COMPARE(GUM_INPUTDEVICE(pFixture->pKeyboard)->pFocusedWidget,
                     GUM_WIDGET(pFixture->pSecond));
GBL_TEST_CASE_END

GBL_TEST_CASE(explicitFocus)
    GUM_Nav_focus(GUM_INPUTDEVICE(pFixture->pKeyboard), GUM_WIDGET(pFixture->pFirst));
    GBL_TEST_COMPARE(GUM_INPUTDEVICE(pFixture->pKeyboard)->pFocusedWidget,
                     GUM_WIDGET(pFixture->pFirst));
GBL_TEST_CASE_END

GBL_TEST_REGISTER(defaultFocus,
                  moveRight,
                  moveLeft,
                  explicitFocus)

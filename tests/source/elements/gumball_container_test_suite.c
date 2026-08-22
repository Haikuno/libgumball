#include "elements/gumball_container_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GBL_SELF_TYPE GUM_ContainerTestSuite

GBL_TEST_FIXTURE {
    GUM_Root* pRoot;
};

GBL_TEST_INIT()
    pFixture->pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GUM_unref(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(verticalLayout)
    GUM_Container* pContainer = GUM_Container_create("w", 100.0f,
                                                     "h", 100.0f,
                                                     "padding", 10.0f,
                                                     "margin", 5.0f);
    GUM_Widget* pFirst  = GUM_Widget_create("parent", pContainer);
    GUM_Widget* pSecond = GUM_Widget_create("parent", pContainer);

    GBL_TEST_COMPARE(pFirst->x, 10.0f);
    GBL_TEST_COMPARE(pFirst->y, 10.0f);
    GBL_TEST_COMPARE(pFirst->w, 80.0f);
    GBL_TEST_COMPARE(pFirst->h, 35.0f);

    GBL_TEST_COMPARE(pSecond->x, 10.0f);
    GBL_TEST_COMPARE(pSecond->y, 55.0f);
    GBL_TEST_COMPARE(pSecond->w, 80.0f);
    GBL_TEST_COMPARE(pSecond->h, 35.0f);

    GUM_unref(pContainer);
GBL_TEST_CASE_END

GBL_TEST_CASE(horizontalLayout)
    GUM_Container* pContainer = GUM_Container_create("w", 100.0f,
                                                     "h", 60.0f,
                                                     "padding", 10.0f,
                                                     "margin", 5.0f,
                                                     "direction", GUM_DIRECTION_HORIZONTAL);
    GUM_Widget* pFirst  = GUM_Widget_create("parent", pContainer);
    GUM_Widget* pSecond = GUM_Widget_create("parent", pContainer);

    GBL_TEST_COMPARE(pFirst->x, 10.0f);
    GBL_TEST_COMPARE(pFirst->y, 10.0f);
    GBL_TEST_COMPARE(pFirst->w, 35.0f);
    GBL_TEST_COMPARE(pFirst->h, 40.0f);

    GBL_TEST_COMPARE(pSecond->x, 55.0f);
    GBL_TEST_COMPARE(pSecond->y, 10.0f);
    GBL_TEST_COMPARE(pSecond->w, 35.0f);
    GBL_TEST_COMPARE(pSecond->h, 40.0f);

    GUM_unref(pContainer);
GBL_TEST_CASE_END

GBL_TEST_CASE(minimumChildSizeOverflow)
    GUM_Container* pContainer = GUM_Container_create("w", 100.0f,
                                                     "h", 100.0f,
                                                     "padding", 0.0f,
                                                     "margin", 0.0f,
                                                     "minChildSize", 0.4f);
    GUM_Widget* pFirst  = GUM_Widget_create("parent", pContainer);
    GUM_Widget* pSecond = GUM_Widget_create("parent", pContainer);
    GUM_Widget* pThird  = GUM_Widget_create("parent", pContainer);

    GBL_TEST_COMPARE(pFirst->h, 40.0f);
    GBL_TEST_COMPARE(pSecond->h, 40.0f);
    GBL_TEST_COMPARE(pThird->h, 40.0f);
    GBL_TEST_COMPARE(pFirst->y, 0.0f);
    GBL_TEST_COMPARE(pSecond->y, 40.0f);
    GBL_TEST_COMPARE(pThird->y, 80.0f);

    GUM_unref(pContainer);
GBL_TEST_CASE_END

GBL_TEST_CASE(scrollStateClamping)
    GUM_Container* pContainer = GUM_Container_create("w", 100.0f,
                                                     "h", 100.0f,
                                                     "padding", 0.0f,
                                                     "margin", 0.0f,
                                                     "minChildSize", 0.4f);
    GUM_Widget_create("parent", pContainer);
    GUM_Widget_create("parent", pContainer);
    GUM_Widget_create("parent", pContainer);

    pContainer->scrollAnimatorY.from    = 50.0f;
    pContainer->scrollAnimatorY.current = 50.0f;
    pContainer->scrollAnimatorY.to      = 50.0f;
    GUM_CONTAINER_CLASSOF(pContainer)->pFnUpdateContent(pContainer);

    GBL_TEST_COMPARE(pContainer->scrollAnimatorY.from, 20.0f);
    GBL_TEST_COMPARE(pContainer->scrollAnimatorY.current, 20.0f);
    GBL_TEST_COMPARE(pContainer->scrollAnimatorY.to, 20.0f);

    GUM_unref(pContainer);
GBL_TEST_CASE_END

GBL_TEST_CASE(retainedChild)
    GUM_Container* pContainer = GUM_Container_create();
    GUM_Widget* pChild = GUM_Widget_create("parent", pContainer);
    GUM_ref(pChild);

    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pChild)), 2);

    GUM_unref(pContainer);

    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pChild)), 1);
    GBL_TEST_COMPARE(GblObject_parent(GBL_OBJECT(pChild)), nullptr);

    GUM_unref(pChild);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(verticalLayout,
                  horizontalLayout,
                  minimumChildSizeOverflow,
                  scrollStateClamping,
                  retainedChild)

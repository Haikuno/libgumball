#include "elements/gumball_objectviewer_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GBL_SELF_TYPE GUM_ObjectViewerTestSuite

GBL_TEST_FIXTURE {
    GUM_Root*   pRoot;
    GUM_Widget* pObserved;
};

GBL_TEST_INIT()
    pFixture->pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pFixture->pRoot);

    pFixture->pObserved = GUM_Widget_create("label", "observed");
    GBL_TEST_VERIFY(pFixture->pObserved);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GUM_unref(pFixture->pObserved);
    GUM_unref(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(object)
    GUM_ObjectViewer* pViewer = GUM_ObjectViewer_create();
    GBL_TEST_VERIFY(pViewer);

    GBL_TEST_COMPARE(GUM_ObjectViewer_setObject(pViewer, GBL_OBJECT(pFixture->pObserved)),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(pViewer->pObject, GBL_OBJECT(pFixture->pObserved));

    GUM_unref(pViewer);
GBL_TEST_CASE_END

GBL_TEST_CASE(update)
    GUM_ObjectViewer* pViewer = GUM_ObjectViewer_create();
    GBL_TEST_VERIFY(pViewer);

    GBL_TEST_COMPARE(GUM_ObjectViewer_setObject(pViewer, GBL_OBJECT(pFixture->pObserved)),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GUM_WIDGET_CLASSOF(pViewer)->pFnUpdate(GUM_WIDGET(pViewer)),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_WIDGET(pViewer)->shouldUpdate);
    GBL_TEST_COMPARE(GblObject_childCount(GBL_OBJECT(pViewer)), 0u);

    GUM_unref(pViewer);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(object,
                  update)

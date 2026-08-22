#include "elements/gumball_widget_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GUM_WIDGET_LIFECYCLE_PROBE_TYPE (GBL_TYPEID(GUM_WidgetLifecycleProbe))
#define GUM_WIDGET_LIFECYCLE_PROBE(self) (GBL_CAST(GUM_WidgetLifecycleProbe, self))

GBL_FORWARD_DECLARE_STRUCT(GUM_WidgetLifecycleProbe);
GblType GUM_WidgetLifecycleProbe_type(void) GBL_NOEXCEPT;

GBL_CLASS_DERIVE_EMPTY(GUM_WidgetLifecycleProbe, GUM_Widget)

GBL_INSTANCE_DERIVE(GUM_WidgetLifecycleProbe, GUM_Widget)
    size_t     activateCount;
    size_t     deactivateCount;
    size_t     activateSignalCount;
    size_t     deactivateSignalCount;
    bool       activateObservedActive;
    bool       deactivateObservedInactive;
    GBL_RESULT activateResult;
    GBL_RESULT deactivateResult;
GBL_INSTANCE_END

static GBL_RESULT GUM_WidgetLifecycleProbe_init_(GblInstance* pInstance) {
    GUM_WidgetLifecycleProbe* pSelf = GUM_WIDGET_LIFECYCLE_PROBE(pInstance);
    pSelf->activateResult   = GBL_RESULT_SUCCESS;
    pSelf->deactivateResult = GBL_RESULT_SUCCESS;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_WidgetLifecycleProbe_activate_(GUM_Widget* pWidget) {
    GUM_WidgetLifecycleProbe* pSelf = GUM_WIDGET_LIFECYCLE_PROBE(pWidget);
    ++pSelf->activateCount;
    pSelf->activateObservedActive = pWidget->isActive;
    return pSelf->activateResult;
}

static GBL_RESULT GUM_WidgetLifecycleProbe_deactivate_(GUM_Widget* pWidget) {
    GUM_WidgetLifecycleProbe* pSelf = GUM_WIDGET_LIFECYCLE_PROBE(pWidget);
    ++pSelf->deactivateCount;
    pSelf->deactivateObservedInactive = !pWidget->isActive;
    return pSelf->deactivateResult;
}

static GBL_RESULT GUM_WidgetLifecycleProbeClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GUM_WIDGET_CLASS(pClass)->pFnActivate   = GUM_WidgetLifecycleProbe_activate_;
    GUM_WIDGET_CLASS(pClass)->pFnDeactivate = GUM_WidgetLifecycleProbe_deactivate_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_WidgetLifecycleProbe_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_WidgetLifecycleProbe"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_WidgetLifecycleProbeClass),
                                                       .instanceSize    = sizeof(GUM_WidgetLifecycleProbe),
                                                       .pFnClassInit    = GUM_WidgetLifecycleProbeClass_init_,
                                                       .pFnInstanceInit = GUM_WidgetLifecycleProbe_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

static GUM_WidgetLifecycleProbe* lifecycleProbeCreate_(void) {
    return GBL_NEW(GUM_WidgetLifecycleProbe);
}

static void onActivate_(GUM_WidgetLifecycleProbe* pSelf) {
    ++pSelf->activateSignalCount;
}

static void onDeactivate_(GUM_WidgetLifecycleProbe* pSelf) {
    ++pSelf->deactivateSignalCount;
}

#define GBL_SELF_TYPE GUM_WidgetTestSuite

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

GBL_TEST_CASE(lifecycleTransitions)
    GUM_WidgetLifecycleProbe* pProbe = lifecycleProbeCreate_();
    GUM_connect(pProbe,
                "onActivate", onActivate_,
                "onDeactivate", onDeactivate_);

    const GBL_RESULT activateResult = GUM_setProperty(pProbe, "isActive", true);
    const bool active = GUM_WIDGET(pProbe)->isActive;
    const size_t activateCount = pProbe->activateCount;
    const size_t activateSignalCount = pProbe->activateSignalCount;
    const bool activateObservedActive = pProbe->activateObservedActive;

    const GBL_RESULT deactivateResult = GUM_setProperty(pProbe, "isActive", false);
    const bool inactive = !GUM_WIDGET(pProbe)->isActive;
    const size_t deactivateCount = pProbe->deactivateCount;
    const size_t deactivateSignalCount = pProbe->deactivateSignalCount;
    const bool deactivateObservedInactive = pProbe->deactivateObservedInactive;

    GUM_unref(pProbe);

    GBL_TEST_COMPARE(activateResult, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(active);
    GBL_TEST_COMPARE(activateCount, 1);
    GBL_TEST_COMPARE(activateSignalCount, 1);
    GBL_TEST_VERIFY(activateObservedActive);
    GBL_TEST_COMPARE(deactivateResult, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(inactive);
    GBL_TEST_COMPARE(deactivateCount, 1);
    GBL_TEST_COMPARE(deactivateSignalCount, 1);
    GBL_TEST_VERIFY(deactivateObservedInactive);
GBL_TEST_CASE_END

GBL_TEST_CASE(lifecycleIdempotence)
    GUM_WidgetLifecycleProbe* pProbe = lifecycleProbeCreate_();
    GUM_connect(pProbe,
                "onActivate", onActivate_,
                "onDeactivate", onDeactivate_);

    const GBL_RESULT firstActivate  = GUM_setProperty(pProbe, "isActive", true);
    const GBL_RESULT secondActivate = GUM_setProperty(pProbe, "isActive", true);
    const GBL_RESULT firstDeactivate  = GUM_setProperty(pProbe, "isActive", false);
    const GBL_RESULT secondDeactivate = GUM_setProperty(pProbe, "isActive", false);

    const size_t activateCount = pProbe->activateCount;
    const size_t deactivateCount = pProbe->deactivateCount;
    const size_t activateSignalCount = pProbe->activateSignalCount;
    const size_t deactivateSignalCount = pProbe->deactivateSignalCount;
    const bool inactive = !GUM_WIDGET(pProbe)->isActive;

    GUM_unref(pProbe);

    GBL_TEST_COMPARE(firstActivate, GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(secondActivate, GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(firstDeactivate, GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(secondDeactivate, GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(activateCount, 1);
    GBL_TEST_COMPARE(deactivateCount, 1);
    GBL_TEST_COMPARE(activateSignalCount, 1);
    GBL_TEST_COMPARE(deactivateSignalCount, 1);
    GBL_TEST_VERIFY(inactive);
GBL_TEST_CASE_END

GBL_TEST_CASE(activateFailureRollback)
    GUM_WidgetLifecycleProbe* pProbe = lifecycleProbeCreate_();
    pProbe->activateResult = GBL_RESULT_ERROR_INTERNAL;
    GUM_connect(pProbe, "onActivate", onActivate_);

    const GBL_RESULT result = GUM_setProperty(pProbe, "isActive", true);
    const bool rolledBack = !GUM_WIDGET(pProbe)->isActive;
    const size_t activateCount = pProbe->activateCount;
    const size_t signalCount = pProbe->activateSignalCount;
    const bool observedTarget = pProbe->activateObservedActive;

    GUM_unref(pProbe);

    GBL_TEST_COMPARE(result, GBL_RESULT_ERROR_INTERNAL);
    GBL_TEST_VERIFY(rolledBack);
    GBL_TEST_COMPARE(activateCount, 1);
    GBL_TEST_COMPARE(signalCount, 0);
    GBL_TEST_VERIFY(observedTarget);
GBL_TEST_CASE_END

GBL_TEST_CASE(deactivateFailureRollback)
    GUM_WidgetLifecycleProbe* pProbe = lifecycleProbeCreate_();
    GUM_connect(pProbe, "onDeactivate", onDeactivate_);

    const GBL_RESULT activateResult = GUM_setProperty(pProbe, "isActive", true);
    pProbe->deactivateResult = GBL_RESULT_ERROR_INTERNAL;
    const GBL_RESULT deactivateResult = GUM_setProperty(pProbe, "isActive", false);

    const bool rolledBack = GUM_WIDGET(pProbe)->isActive;
    const size_t deactivateCount = pProbe->deactivateCount;
    const size_t signalCount = pProbe->deactivateSignalCount;
    const bool observedTarget = pProbe->deactivateObservedInactive;

    GUM_unref(pProbe);

    GBL_TEST_COMPARE(activateResult, GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(deactivateResult, GBL_RESULT_ERROR_INTERNAL);
    GBL_TEST_VERIFY(rolledBack);
    GBL_TEST_COMPARE(deactivateCount, 1);
    GBL_TEST_COMPARE(signalCount, 0);
    GBL_TEST_VERIFY(observedTarget);
GBL_TEST_CASE_END

GBL_TEST_CASE(objectViewerDeactivate)
    GUM_ObjectViewer* pViewer = GUM_ObjectViewer_create();
    GUM_Widget* pObject = GUM_Widget_create();

    const GBL_RESULT setObjectResult = GUM_ObjectViewer_setObject(pViewer, GBL_OBJECT(pObject));
    const GBL_RESULT activateResult = GUM_setProperty(pViewer, "isActive", true);
    const GBL_RESULT deactivateResult = GUM_setProperty(pViewer, "isActive", false);
    const bool objectReleased = pViewer->pObject == nullptr;
    const bool inactive = !GUM_WIDGET(pViewer)->isActive;

    GUM_unref(pObject);
    GUM_unref(pViewer);

    GBL_TEST_COMPARE(setObjectResult, GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(activateResult, GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(deactivateResult, GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(objectReleased);
    GBL_TEST_VERIFY(inactive);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(lifecycleTransitions,
                  lifecycleIdempotence,
                  activateFailureRollback,
                  deactivateFailureRollback,
                  objectViewerDeactivate)

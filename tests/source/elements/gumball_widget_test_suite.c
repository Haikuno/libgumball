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

typedef enum GUM_LifecycleAction_ {
    GUM_LIFECYCLE_ACTION_NONE,
    GUM_LIFECYCLE_ACTION_SAME,
    GUM_LIFECYCLE_ACTION_OPPOSITE,
    GUM_LIFECYCLE_ACTION_UNREF
} GUM_LifecycleAction_;

static GUM_LifecycleAction_ activateHookAction_      = GUM_LIFECYCLE_ACTION_NONE;
static GUM_LifecycleAction_ deactivateHookAction_    = GUM_LIFECYCLE_ACTION_NONE;
static GUM_LifecycleAction_ activateSignalAction_    = GUM_LIFECYCLE_ACTION_NONE;
static GUM_LifecycleAction_ deactivateSignalAction_  = GUM_LIFECYCLE_ACTION_NONE;
static GBL_RESULT           nestedLifecycleResult_   = GBL_RESULT_SUCCESS;
static size_t               lifecycleUnrefCount_     = 0;
static size_t               lifecycleDestroyCount_   = 0;
static size_t               inputSignalCount_        = 0;
static size_t               inputActionSignalCount_  = 0;
static size_t               animationDoneCount_      = 0;
static size_t               animationNextDoneCount_  = 0;

static void resetState_(void) {
    activateHookAction_      = GUM_LIFECYCLE_ACTION_NONE;
    deactivateHookAction_    = GUM_LIFECYCLE_ACTION_NONE;
    activateSignalAction_    = GUM_LIFECYCLE_ACTION_NONE;
    deactivateSignalAction_  = GUM_LIFECYCLE_ACTION_NONE;
    nestedLifecycleResult_   = GBL_RESULT_SUCCESS;
    lifecycleUnrefCount_     = 0;
    lifecycleDestroyCount_   = 0;
    inputSignalCount_        = 0;
    inputActionSignalCount_  = 0;
    animationDoneCount_      = 0;
    animationNextDoneCount_  = 0;
}

static void lifecycleAction_(GUM_Widget* pWidget, GUM_LifecycleAction_ action, bool active) {
    switch (action) {
    case GUM_LIFECYCLE_ACTION_SAME:
        nestedLifecycleResult_ = GUM_setProperty(pWidget, "isActive", active);
        break;
    case GUM_LIFECYCLE_ACTION_OPPOSITE:
        nestedLifecycleResult_ = GUM_setProperty(pWidget, "isActive", !active);
        break;
    case GUM_LIFECYCLE_ACTION_UNREF:
        ++lifecycleUnrefCount_;
        GUM_unref(pWidget);
        break;
    case GUM_LIFECYCLE_ACTION_NONE:
    default:
        break;
    }
}

static GBL_RESULT GUM_WidgetLifecycleProbe_init_(GblInstance* pInstance) {
    GUM_WidgetLifecycleProbe* pSelf = GUM_WIDGET_LIFECYCLE_PROBE(pInstance);
    pSelf->activateResult   = GBL_RESULT_SUCCESS;
    pSelf->deactivateResult = GBL_RESULT_SUCCESS;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_WidgetLifecycleProbe_activate_(GUM_Widget* pWidget) {
    GUM_WidgetLifecycleProbe* pSelf = GUM_WIDGET_LIFECYCLE_PROBE(pWidget);
    ++pSelf->activateCount;
    pSelf->activateObservedActive = GUM_Widget_isActive(pWidget);
    lifecycleAction_(pWidget, activateHookAction_, true);
    return pSelf->activateResult;
}

static GBL_RESULT GUM_WidgetLifecycleProbe_deactivate_(GUM_Widget* pWidget) {
    GUM_WidgetLifecycleProbe* pSelf = GUM_WIDGET_LIFECYCLE_PROBE(pWidget);
    ++pSelf->deactivateCount;
    pSelf->deactivateObservedInactive = !GUM_Widget_isActive(pWidget);
    lifecycleAction_(pWidget, deactivateHookAction_, false);
    return pSelf->deactivateResult;
}

static GBL_RESULT GUM_WidgetLifecycleProbe_destructor_(GblBox* pBox) {
    ++lifecycleDestroyCount_;
    GblBoxClass* pWidgetClass = GBL_BOX_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE));
    return pWidgetClass->pFnDestructor(pBox);
}

static GBL_RESULT GUM_WidgetLifecycleProbeClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GBL_BOX_CLASS(pClass)->pFnDestructor      = GUM_WidgetLifecycleProbe_destructor_;
    GUM_WIDGET_CLASS(pClass)->pFnActivate    = GUM_WidgetLifecycleProbe_activate_;
    GUM_WIDGET_CLASS(pClass)->pFnDeactivate  = GUM_WidgetLifecycleProbe_deactivate_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_WidgetLifecycleProbe_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_WidgetLifecycleProbe"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){
                                    .classSize       = sizeof(GUM_WidgetLifecycleProbeClass),
                                    .instanceSize    = sizeof(GUM_WidgetLifecycleProbe),
                                    .pFnClassInit    = GUM_WidgetLifecycleProbeClass_init_,
                                    .pFnInstanceInit = GUM_WidgetLifecycleProbe_init_
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

static GUM_WidgetLifecycleProbe* lifecycleProbe_(void) {
    return GBL_NEW(GUM_WidgetLifecycleProbe);
}

static void onActivate_(GUM_WidgetLifecycleProbe* pSelf) {
    ++pSelf->activateSignalCount;
    lifecycleAction_(GUM_WIDGET(pSelf), activateSignalAction_, true);
}

static void onDeactivate_(GUM_WidgetLifecycleProbe* pSelf) {
    ++pSelf->deactivateSignalCount;
    lifecycleAction_(GUM_WIDGET(pSelf), deactivateSignalAction_, false);
}

static void onPressUnref_(GUM_WidgetLifecycleProbe* pSelf, GUM_Event_Input* pEvent) {
    GBL_UNUSED(pEvent);
    ++inputSignalCount_;
    GUM_unref(pSelf);
}

static void onPressMutate_(GUM_WidgetLifecycleProbe* pSelf, GUM_Event_Input* pEvent) {
    GBL_UNUSED(pSelf);
    ++inputSignalCount_;
    pEvent->action = GUM_INPUTACTION_COUNT;
}

static void onPressConfirm_(GUM_WidgetLifecycleProbe* pSelf) {
    GBL_UNUSED(pSelf);
    ++inputActionSignalCount_;
}

static void onNextAnimationDone_(GUM_Widget* pWidget) {
    GBL_UNUSED(pWidget);
    ++animationNextDoneCount_;
}

static void onAnimationDone_(GUM_Widget* pWidget) {
    ++animationDoneCount_;
    GUM_Widget_animate(pWidget, "x", 30.0f, 0.0f, GUM_EASE_LINEAR);
    GUM_Widget_animateOnDone(pWidget, "x", onNextAnimationDone_);
}

#define GBL_SELF_TYPE GUM_WidgetTestSuite

GBL_TEST_FIXTURE {
    GUM_Root* pRoot;
};

GBL_TEST_INIT()
    resetState_();
    pFixture->pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GUM_unref(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(lifecycle)
    resetState_();
    GUM_WidgetLifecycleProbe* pProbe = lifecycleProbe_();
    GBL_TEST_VERIFY(pProbe);
    GUM_connect(pProbe,
                "onActivate",   onActivate_,
                "onDeactivate", onDeactivate_);

    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", true), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", true), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(GUM_Widget_isActive(GUM_WIDGET(pProbe)));
    GBL_TEST_COMPARE(pProbe->activateCount, 1u);
    GBL_TEST_COMPARE(pProbe->activateSignalCount, 1u);
    GBL_TEST_VERIFY(pProbe->activateObservedActive);

    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", false), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", false), GBL_RESULT_SUCCESS);
    GBL_TEST_VERIFY(!GUM_Widget_isActive(GUM_WIDGET(pProbe)));
    GBL_TEST_COMPARE(pProbe->deactivateCount, 1u);
    GBL_TEST_COMPARE(pProbe->deactivateSignalCount, 1u);
    GBL_TEST_VERIFY(pProbe->deactivateObservedInactive);

    GUM_unref(pProbe);
GBL_TEST_CASE_END

GBL_TEST_CASE(lifecycleReentry)
    resetState_();
    GUM_WidgetLifecycleProbe* pProbe = lifecycleProbe_();
    GBL_TEST_VERIFY(pProbe);

    activateHookAction_ = GUM_LIFECYCLE_ACTION_SAME;
    GUM_connect(pProbe, "onActivate", onActivate_);
    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", true), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(nestedLifecycleResult_, GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(pProbe->activateCount, 1u);
    GBL_TEST_COMPARE(pProbe->activateSignalCount, 1u);

    nestedLifecycleResult_  = GBL_RESULT_SUCCESS;
    activateHookAction_     = GUM_LIFECYCLE_ACTION_NONE;
    deactivateSignalAction_ = GUM_LIFECYCLE_ACTION_OPPOSITE;
    GUM_connect(pProbe, "onDeactivate", onDeactivate_);
    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", false), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(nestedLifecycleResult_, GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_TEST_VERIFY(!GUM_Widget_isActive(GUM_WIDGET(pProbe)));

    GUM_unref(pProbe);
GBL_TEST_CASE_END

GBL_TEST_CASE(lifecycleFailure)
    resetState_();
    GUM_WidgetLifecycleProbe* pProbe = lifecycleProbe_();
    GBL_TEST_VERIFY(pProbe);
    GUM_connect(pProbe,
                "onActivate",   onActivate_,
                "onDeactivate", onDeactivate_);

    pProbe->activateResult = GBL_RESULT_ERROR_INTERNAL;
    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", true), GBL_RESULT_ERROR_INTERNAL);
    GBL_TEST_VERIFY(!GUM_Widget_isActive(GUM_WIDGET(pProbe)));
    GBL_TEST_COMPARE(pProbe->activateSignalCount, 0u);
    GBL_TEST_VERIFY(pProbe->activateObservedActive);

    pProbe->activateResult = GBL_RESULT_SUCCESS;
    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", true), GBL_RESULT_SUCCESS);

    pProbe->deactivateResult = GBL_RESULT_ERROR_INTERNAL;
    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", false), GBL_RESULT_ERROR_INTERNAL);
    GBL_TEST_VERIFY(GUM_Widget_isActive(GUM_WIDGET(pProbe)));
    GBL_TEST_COMPARE(pProbe->deactivateSignalCount, 0u);
    GBL_TEST_VERIFY(pProbe->deactivateObservedInactive);

    GUM_unref(pProbe);
GBL_TEST_CASE_END

GBL_TEST_CASE(lifecycleLifetime)
    resetState_();
    GUM_WidgetLifecycleProbe* pProbe = lifecycleProbe_();
    GBL_TEST_VERIFY(pProbe);
    GblObject_setParent(GBL_OBJECT(pProbe), nullptr);
    activateHookAction_ = GUM_LIFECYCLE_ACTION_UNREF;

    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", true), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(lifecycleUnrefCount_, 1u);
    GBL_TEST_COMPARE(lifecycleDestroyCount_, 1u);

    resetState_();
    pProbe = lifecycleProbe_();
    GBL_TEST_VERIFY(pProbe);
    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", true), GBL_RESULT_SUCCESS);
    GblObject_setParent(GBL_OBJECT(pProbe), nullptr);
    deactivateSignalAction_ = GUM_LIFECYCLE_ACTION_UNREF;
    GUM_connect(pProbe, "onDeactivate", onDeactivate_);

    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", false), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(lifecycleUnrefCount_, 1u);
    GBL_TEST_COMPARE(lifecycleDestroyCount_, 1u);
GBL_TEST_CASE_END

GBL_TEST_CASE(inputSignals)
    resetState_();
    GUM_WidgetLifecycleProbe* pProbe = lifecycleProbe_();
    GBL_TEST_VERIFY(pProbe);
    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", true), GBL_RESULT_SUCCESS);
    GblObject_setParent(GBL_OBJECT(pProbe), nullptr);
    GUM_connect(pProbe,
                "onPress",        onPressUnref_,
                "onPressConfirm", onPressConfirm_);

    GUM_Event_Input* pEvent = GUM_EVENT_INPUT(GblEvent_create(GUM_EVENT_INPUT_TYPE));
    GBL_TEST_VERIFY(pEvent);
    pEvent->state  = GUM_INPUTSTATE_PRESS;
    pEvent->action = GUM_INPUTACTION_CONFIRM;

    GBL_TEST_COMPARE(GblObject_notifyEvent(GBL_OBJECT(pProbe), GBL_EVENT(pEvent)),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GblEvent_state(GBL_EVENT(pEvent)), GBL_EVENT_STATE_ACCEPTED);
    GBL_TEST_COMPARE(inputSignalCount_, 1u);
    GBL_TEST_COMPARE(inputActionSignalCount_, 1u);
    GBL_TEST_COMPARE(lifecycleDestroyCount_, 1u);
    GBL_UNREF(pEvent);

    resetState_();
    pProbe = lifecycleProbe_();
    GBL_TEST_VERIFY(pProbe);
    GBL_TEST_COMPARE(GUM_setProperty(pProbe, "isActive", true), GBL_RESULT_SUCCESS);
    GUM_connect(pProbe,
                "onPress",        onPressMutate_,
                "onPressConfirm", onPressConfirm_);

    pEvent = GUM_EVENT_INPUT(GblEvent_create(GUM_EVENT_INPUT_TYPE));
    GBL_TEST_VERIFY(pEvent);
    pEvent->state  = GUM_INPUTSTATE_PRESS;
    pEvent->action = GUM_INPUTACTION_CONFIRM;

    GBL_TEST_COMPARE(GblObject_notifyEvent(GBL_OBJECT(pProbe), GBL_EVENT(pEvent)),
                     GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(inputSignalCount_, 1u);
    GBL_TEST_COMPARE(inputActionSignalCount_, 1u);

    GBL_UNREF(pEvent);
    GUM_unref(pProbe);
GBL_TEST_CASE_END

GBL_TEST_CASE(animate)
    GUM_Widget* pWidget = GUM_Widget_create("x", 4.0f);
    GBL_TEST_VERIFY(pWidget);

    GUM_Widget_animate(pWidget, "x", 18.0f, 0.0f, GUM_EASE_LINEAR);
    GBL_TEST_COMPARE(pWidget->x, 18.0f);

    GUM_Font* pFont = GUM_Widget_font(pWidget);
    GUM_Widget_animate(pWidget, "font", 1.0f, 0.0f, GUM_EASE_LINEAR);
    GBL_TEST_COMPARE(GUM_update(), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GUM_Widget_font(pWidget), pFont);

    const GblRefCount refCount = GblBox_refCount(GBL_BOX(pWidget));
    GUM_Widget_animate(pWidget, "refCount", (float)(refCount + 1), 0.0f, GUM_EASE_LINEAR);
    GBL_TEST_COMPARE(GUM_update(), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pWidget)), refCount);

    GUM_unref(pWidget);
GBL_TEST_CASE_END

GBL_TEST_CASE(animateLayout)
    GUM_Container* pContainer = GUM_Container_create("w", 100.0f,
                                                     "h", 100.0f,
                                                     "padding", 0.0f,
                                                     "margin", 5.0f,
                                                     "resizeWidgets", false);
    GUM_Widget* pFirst = GUM_Widget_create("parent", pContainer,
                                           "w", 20.0f,
                                           "h", 20.0f);
    GUM_Widget* pSecond = GUM_Widget_create("parent", pContainer,
                                            "w", 20.0f,
                                            "h", 20.0f);
    GBL_TEST_VERIFY(pContainer && pFirst && pSecond);
    GBL_TEST_COMPARE(pFirst->x, 40.0f);
    GBL_TEST_COMPARE(pSecond->y, 30.0f);

    GUM_Widget_animate(pFirst, "h", 40.0f, 0.0f, GUM_EASE_LINEAR);
    GBL_TEST_COMPARE(pFirst->h, 40.0f);
    GBL_TEST_COMPARE(pSecond->y, 50.0f);

    GUM_Widget_animate(pFirst, "w", 40.0f, 0.0f, GUM_EASE_LINEAR);
    GBL_TEST_COMPARE(pFirst->w, 40.0f);
    GBL_TEST_COMPARE(pFirst->x, 30.0f);

    GUM_Container* pResizable = GUM_Container_create("w", 100.0f,
                                                     "h", 100.0f,
                                                     "padding", 10.0f,
                                                     "margin", 0.0f);
    GUM_Widget* pChild = GUM_Widget_create("parent", pResizable);
    GBL_TEST_VERIFY(pResizable && pChild);
    GBL_TEST_COMPARE(pChild->h, 80.0f);

    GUM_Widget_animate(GUM_WIDGET(pResizable), "h", 120.0f, 0.0f, GUM_EASE_LINEAR);
    GBL_TEST_COMPARE(pChild->h, 100.0f);

    GUM_unref(pResizable);
    GUM_unref(pContainer);
GBL_TEST_CASE_END

GBL_TEST_CASE(animateOnDone)
    resetState_();
    GUM_Widget* pWidget = GUM_Widget_create("x", 1.0f);
    GBL_TEST_VERIFY(pWidget);

    GUM_Widget_animate(pWidget, "x", 10.0f, 0.0f, GUM_EASE_LINEAR);
    GUM_Widget_animateOnDone(pWidget, "x", onAnimationDone_);
    GBL_TEST_COMPARE(GUM_update(), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(animationDoneCount_, 1u);
    GBL_TEST_COMPARE(animationNextDoneCount_, 0u);
    GBL_TEST_COMPARE(pWidget->x, 30.0f);

    GBL_TEST_COMPARE(GUM_update(), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(animationDoneCount_, 1u);
    GBL_TEST_COMPARE(animationNextDoneCount_, 1u);
    GBL_TEST_COMPARE(pWidget->x, 30.0f);

    GUM_unref(pWidget);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(lifecycle,
                  lifecycleReentry,
                  lifecycleFailure,
                  lifecycleLifetime,
                  inputSignals,
                  animate,
                  animateLayout,
                  animateOnDone)

#include "elements/gumball_root_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#include "../../../lib/src/elements/gumball_root_.h"

#define GUM_ROOT_PROBE_TYPE (GBL_TYPEID(GUM_RootProbe))
#define GUM_ROOT_PROBE(self) (GBL_CAST(GUM_RootProbe, self))

GBL_FORWARD_DECLARE_STRUCT(GUM_RootProbe);
GblType GUM_RootProbe_type(void) GBL_NOEXCEPT;

GBL_CLASS_DERIVE_EMPTY(GUM_RootProbe, GUM_Widget)

typedef enum GUM_RootProbeAction_ {
    GUM_ROOT_PROBE_NONE,
    GUM_ROOT_PROBE_UNREF_SELF,
    GUM_ROOT_PROBE_UNREF_TARGET,
    GUM_ROOT_PROBE_DISABLE_TARGET,
    GUM_ROOT_PROBE_ENABLE_TARGET,
    GUM_ROOT_PROBE_REPARENT_TARGET,
    GUM_ROOT_PROBE_SET_TARGET_Z
} GUM_RootProbeAction_;

GBL_INSTANCE_DERIVE(GUM_RootProbe, GUM_Widget)
    uint8_t              id;
    uint8_t*             pOrder;
    size_t*              pDrawCount;
    size_t*              pUpdateCount;
    GBL_RESULT           drawResult;
    GBL_RESULT           updateResult;
    GUM_RootProbeAction_ action;
    GUM_Widget*          pTarget;
    GblObject*           pNewParent;
    uint8_t              targetZ;
    GUM_RootProbe**      ppSelfOwner;
    GUM_RootProbe**      ppTargetOwner;
GBL_INSTANCE_END

static GBL_RESULT GUM_RootProbe_draw_(GUM_Widget* pWidget, GUM_Renderer* pRenderer) {
    GBL_UNUSED(pRenderer);
    GUM_RootProbe* pSelf = GUM_ROOT_PROBE(pWidget);

    if (pSelf->pOrder && pSelf->pDrawCount)
        pSelf->pOrder[(*pSelf->pDrawCount)++] = pSelf->id;

    const GBL_RESULT result = pSelf->drawResult;

    switch (pSelf->action) {
    case GUM_ROOT_PROBE_UNREF_SELF:
        pSelf->action = GUM_ROOT_PROBE_NONE;
        if (pSelf->ppSelfOwner)
            *pSelf->ppSelfOwner = nullptr;
        GUM_unref(pSelf);
        return result;
    case GUM_ROOT_PROBE_UNREF_TARGET: {
        GUM_Widget* pTarget = pSelf->pTarget;
        pSelf->action  = GUM_ROOT_PROBE_NONE;
        pSelf->pTarget = nullptr;
        if (pSelf->ppTargetOwner)
            *pSelf->ppTargetOwner = nullptr;
        if (pTarget)
            GUM_unref(pTarget);
        break;
    }
    case GUM_ROOT_PROBE_DISABLE_TARGET:
        pSelf->action = GUM_ROOT_PROBE_NONE;
        if (pSelf->pTarget)
            GUM_draw_disable(pSelf->pTarget);
        break;
    case GUM_ROOT_PROBE_ENABLE_TARGET:
        pSelf->action = GUM_ROOT_PROBE_NONE;
        if (pSelf->pTarget)
            GUM_draw_enable(pSelf->pTarget);
        break;
    case GUM_ROOT_PROBE_REPARENT_TARGET:
        pSelf->action = GUM_ROOT_PROBE_NONE;
        if (pSelf->pTarget)
            GUM_setProperty(pSelf->pTarget, "parent", pSelf->pNewParent);
        break;
    case GUM_ROOT_PROBE_SET_TARGET_Z:
        pSelf->action = GUM_ROOT_PROBE_NONE;
        if (pSelf->pTarget)
            GUM_setProperty(pSelf->pTarget, "z_index", pSelf->targetZ);
        break;
    case GUM_ROOT_PROBE_NONE:
    default:
        break;
    }

    return result;
}

static GBL_RESULT GUM_RootProbe_update_(GUM_Widget* pWidget) {
    GUM_RootProbe* pSelf = GUM_ROOT_PROBE(pWidget);
    if (pSelf->pUpdateCount)
        ++*pSelf->pUpdateCount;
    return pSelf->updateResult;
}

static GBL_RESULT GUM_RootProbeClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GUM_WIDGET_CLASS(pClass)->pFnDraw   = GUM_RootProbe_draw_;
    GUM_WIDGET_CLASS(pClass)->pFnUpdate = GUM_RootProbe_update_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_RootProbe_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_RootProbe"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){
                                    .classSize    = sizeof(GUM_RootProbeClass),
                                    .instanceSize = sizeof(GUM_RootProbe),
                                    .pFnClassInit = GUM_RootProbeClass_init_
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

static GUM_RootProbe* rootProbe_(GblObject* pParent, uint8_t id, uint8_t zIndex,
                                 uint8_t* pOrder, size_t* pDrawCount) {
    GUM_RootProbe* pProbe = GBL_NEW(GUM_RootProbe,
                                    "parent", pParent,
                                    "z_index", zIndex);
    if (pProbe) {
        pProbe->id           = id;
        pProbe->pOrder       = pOrder;
        pProbe->pDrawCount   = pDrawCount;
        pProbe->drawResult   = GBL_RESULT_SUCCESS;
        pProbe->updateResult = GBL_RESULT_SUCCESS;
    }
    return pProbe;
}

#define GBL_SELF_TYPE GUM_RootTestSuite

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

GBL_TEST_CASE(singleRoot)
    GUM_Root* pSecond = GUM_Root_create();
    GBL_TEST_VERIFY(!pSecond);
    GBL_TEST_COMPARE(GblModule_find("GUM_Root"), GBL_MODULE(pFixture->pRoot));
GBL_TEST_CASE_END

GBL_TEST_CASE(drawOrder)
    uint8_t order[4] = { 0 };
    size_t count = 0;
    GUM_RootProbe* pA = rootProbe_(nullptr, 1, 100, order, &count);
    GUM_RootProbe* pB = rootProbe_(nullptr, 2,  50, order, &count);
    GUM_RootProbe* pC = rootProbe_(nullptr, 3, 100, order, &count);
    GUM_RootProbe* pChild = rootProbe_(GBL_OBJECT(pA), 4, 100, order, &count);
    GBL_TEST_VERIFY(pA && pB && pC && pChild);

    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 4u);
    GBL_TEST_COMPARE(order[0], 2u);
    GBL_TEST_COMPARE(order[1], 1u);
    GBL_TEST_COMPARE(order[2], 3u);
    GBL_TEST_COMPARE(order[3], 4u);

    count = 0;
    GBL_TEST_CALL(GUM_setProperty(pB, "z_index", (uint8_t)100));
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(order[0], 1u);
    GBL_TEST_COMPARE(order[1], 2u);
    GBL_TEST_COMPARE(order[2], 3u);
    GBL_TEST_COMPARE(order[3], 4u);

    count = 0;
    GBL_TEST_CALL(GUM_draw_disable(pA));
    GBL_TEST_CALL(GUM_draw_enable(pA));
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(order[0], 2u);
    GBL_TEST_COMPARE(order[1], 3u);
    GBL_TEST_COMPARE(order[2], 1u);
    GBL_TEST_COMPARE(order[3], 4u);

    GBL_TEST_COMPARE(GUM_Widget_zIndex(GUM_WIDGET(pA)), 100u);
    GBL_TEST_COMPARE(GUM_Widget_zIndex(GUM_WIDGET(pChild)), 100u);
    GBL_TEST_CALL(GUM_remove_child(pA, pChild));
    GBL_TEST_COMPARE(GUM_Widget_zIndex(GUM_WIDGET(pChild)), 100u);
    GBL_TEST_CALL(GUM_add_child(pA, pChild));
    GBL_TEST_COMPARE(GUM_Widget_zIndex(GUM_WIDGET(pChild)), 100u);

    GUM_unref(pChild);
    GUM_unref(pC);
    GUM_unref(pB);
    GUM_unref(pA);
GBL_TEST_CASE_END

GBL_TEST_CASE(membership)
    uint8_t order[2] = { 0 };
    size_t count = 0;
    GUM_Container* pA = GUM_Container_create();
    GUM_Container* pB = GUM_Container_create();
    GUM_Container* pNested = GUM_Container_create("parent", pA);
    GUM_RootProbe* pOne = rootProbe_(GBL_OBJECT(pNested), 1, 50, order, &count);
    GUM_RootProbe* pTwo = rootProbe_(GBL_OBJECT(pNested), 2, 50, order, &count);
    GBL_TEST_VERIFY(pA && pB && pNested && pOne && pTwo);

    GBL_TEST_CALL(GUM_draw_disable(pTwo));
    GBL_TEST_CALL(GUM_remove_child(pA, pNested));
    GBL_TEST_CALL(GUM_draw_enable(pTwo));
    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 0u);

    GBL_TEST_CALL(GUM_add_child(pB, pNested));
    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 2u);

    GBL_TEST_CALL(GUM_draw_disable(pOne));
    GBL_TEST_CALL(GUM_setProperty(pNested, "parent", pA));
    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 1u);
    GBL_TEST_COMPARE(order[0], 2u);

    GUM_unref(pTwo);
    GUM_unref(pOne);
    GUM_unref(pNested);
    GUM_unref(pB);
    GUM_unref(pA);
GBL_TEST_CASE_END

GBL_TEST_CASE(largeSubtree)
    enum { ProbeCount = 65 };
    uint8_t order[ProbeCount];
    size_t count = 0;
    GUM_RootProbe* probes[ProbeCount] = { nullptr };
    GUM_Container* pContainer = GUM_Container_create();
    GBL_TEST_VERIFY(pContainer);

    for (size_t i = 0; i < ProbeCount; ++i) {
        probes[i] = rootProbe_(GBL_OBJECT(pContainer), (uint8_t)i, 50, order, &count);
        GBL_TEST_VERIFY(probes[i]);
    }

    GBL_TEST_CALL(GUM_remove_child(pFixture->pRoot, pContainer));
    count = 0;
    GBL_TEST_COMPARE(GUM_draw(), GBL_RESULT_PARTIAL);
    GBL_TEST_COMPARE(count, 0u);

    GBL_TEST_CALL(GUM_add_child(pFixture->pRoot, pContainer));
    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, ProbeCount);

    for (size_t i = 0; i < ProbeCount; ++i)
        GUM_unref(probes[i]);
    GUM_unref(pContainer);
GBL_TEST_CASE_END

GBL_TEST_CASE(drawLifetime)
    uint8_t order[2] = { 0 };
    size_t count = 0;
    GUM_RootProbe* pA = rootProbe_(nullptr, 1, 10, order, &count);
    GUM_RootProbe* pB = rootProbe_(nullptr, 2, 20, order, &count);
    GBL_TEST_VERIFY(pA && pB);

    pA->action      = GUM_ROOT_PROBE_UNREF_SELF;
    pA->ppSelfOwner = &pA;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_VERIFY(!pA);
    GBL_TEST_COMPARE(count, 2u);
    GBL_TEST_COMPARE(order[0], 1u);
    GBL_TEST_COMPARE(order[1], 2u);

    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 1u);
    GBL_TEST_COMPARE(order[0], 2u);
    GUM_unref(pB);

    count = 0;
    pA = rootProbe_(nullptr, 1, 10, order, &count);
    pB = rootProbe_(nullptr, 2, 20, order, &count);
    GBL_TEST_VERIFY(pA && pB);

    pA->action        = GUM_ROOT_PROBE_UNREF_TARGET;
    pA->pTarget       = GUM_WIDGET(pB);
    pA->ppTargetOwner = &pB;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_VERIFY(!pB);
    GBL_TEST_COMPARE(count, 2u);

    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 1u);
    GUM_unref(pA);

    GUM_Container* pContainer = GUM_Container_create();
    rootProbe_(GBL_OBJECT(pContainer), 1, 10, order, &count);
    rootProbe_(GBL_OBJECT(pContainer), 2, 20, order, &count);
    GBL_TEST_VERIFY(pContainer);
    GUM_unref(pContainer);

    count = 0;
    GBL_TEST_COMPARE(GUM_draw(), GBL_RESULT_PARTIAL);
    GBL_TEST_COMPARE(count, 0u);
GBL_TEST_CASE_END

GBL_TEST_CASE(drawMutation)
    uint8_t order[2] = { 0 };
    size_t count = 0;
    GUM_RootProbe* pA = rootProbe_(nullptr, 1, 10, order, &count);
    GUM_RootProbe* pB = rootProbe_(nullptr, 2, 20, order, &count);
    GBL_TEST_VERIFY(pA && pB);

    pA->pTarget = GUM_WIDGET(pB);
    pA->action  = GUM_ROOT_PROBE_DISABLE_TARGET;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 2u);

    count = 0;
    pA->action = GUM_ROOT_PROBE_ENABLE_TARGET;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 1u);

    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 2u);

    pA->action     = GUM_ROOT_PROBE_REPARENT_TARGET;
    pA->pNewParent = nullptr;
    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 2u);

    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 1u);

    pA->action     = GUM_ROOT_PROBE_REPARENT_TARGET;
    pA->pNewParent = GBL_OBJECT(pFixture->pRoot);
    GBL_TEST_CALL(GUM_draw());
    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(count, 2u);

    pA->action  = GUM_ROOT_PROBE_SET_TARGET_Z;
    pA->targetZ = 5;
    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(order[0], 1u);
    GBL_TEST_COMPARE(order[1], 2u);

    count = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(order[0], 2u);
    GBL_TEST_COMPARE(order[1], 1u);

    GUM_unref(pB);
    GUM_unref(pA);
GBL_TEST_CASE_END

GBL_TEST_CASE(pointerTargets)
    GUM_Container* pContainer = GUM_Container_create("x", 10.0f,
                                                     "y", 10.0f,
                                                     "w", 100.0f,
                                                     "h", 100.0f,
                                                     "alignWidgets", false,
                                                     "resizeWidgets", false);
    GUM_Button* pButton = GUM_Button_create("parent", pContainer,
                                            "x", 20.0f,
                                            "y", 20.0f,
                                            "w", 20.0f,
                                            "h", 20.0f);
    GBL_TEST_VERIFY(pContainer && pButton);
    GBL_TEST_VERIFY(!GUM_Widget_isActive(GUM_WIDGET(pContainer)));

    const GUM_Vector2 containerPoint = { 80.0f, 80.0f };
    GBL_TEST_COMPARE(GUM_Root_pointerTargetAt_(pFixture->pRoot, containerPoint), nullptr);
    GBL_TEST_COMPARE(GUM_Root_pointerHoverAt_(pFixture->pRoot, containerPoint), GUM_WIDGET(pContainer));

    const GUM_Vector2 buttonPoint = { 25.0f, 25.0f };
    GBL_TEST_COMPARE(GUM_Root_pointerTargetAt_(pFixture->pRoot, buttonPoint), GUM_WIDGET(pButton));
    GBL_TEST_COMPARE(GUM_Root_pointerHoverAt_(pFixture->pRoot, buttonPoint), GUM_WIDGET(pButton));

    GBL_TEST_CALL(GUM_setProperty(pContainer, "isInteractive", false));
    GBL_TEST_COMPARE(GUM_Root_pointerHoverAt_(pFixture->pRoot, containerPoint), nullptr);

    GUM_unref(pContainer);
GBL_TEST_CASE_END

GBL_TEST_CASE(results)
    uint8_t order[2] = { 0 };
    size_t drawCount = 0;
    size_t updateA = 0;
    size_t updateB = 0;
    GUM_RootProbe* pA = rootProbe_(nullptr, 1, 10, order, &drawCount);
    GUM_RootProbe* pB = rootProbe_(nullptr, 2, 20, order, &drawCount);
    GBL_TEST_VERIFY(pA && pB);

    pA->drawResult = GBL_RESULT_ERROR_INTERNAL;
    GBL_TEST_COMPARE(GUM_draw(), GBL_RESULT_ERROR_INTERNAL);
    GBL_TEST_COMPARE(drawCount, 2u);

    pA->pUpdateCount = &updateA;
    pB->pUpdateCount = &updateB;
    pA->updateResult = GBL_RESULT_ERROR_INVALID_OPERATION;
    GBL_TEST_COMPARE(GUM_update(), GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_TEST_COMPARE(updateA, 1u);
    GBL_TEST_COMPARE(updateB, 1u);

    GUM_unref(pB);
    GUM_unref(pA);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(singleRoot,
                  drawOrder,
                  membership,
                  largeSubtree,
                  drawLifetime,
                  drawMutation,
                  pointerTargets,
                  results)

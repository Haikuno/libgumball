#include "elements/gumball_root_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GUM_ROOT_DRAW_PROBE_TYPE (GBL_TYPEID(GUM_RootDrawProbe))
#define GUM_ROOT_DRAW_PROBE(self) (GBL_CAST(GUM_RootDrawProbe, self))

GBL_FORWARD_DECLARE_STRUCT(GUM_RootDrawProbe);
GblType GUM_RootDrawProbe_type(void) GBL_NOEXCEPT;

GBL_CLASS_DERIVE_EMPTY(GUM_RootDrawProbe, GUM_Widget)

GBL_INSTANCE_DERIVE(GUM_RootDrawProbe, GUM_Widget)
    uint8_t  id;
    uint8_t* pOrder;
    size_t*  pCount;
GBL_INSTANCE_END

static GBL_RESULT GUM_RootDrawProbe_draw_(GUM_Widget* pWidget, GUM_Renderer* pRenderer) {
    GBL_UNUSED(pRenderer);
    GUM_RootDrawProbe* pSelf = GUM_ROOT_DRAW_PROBE(pWidget);

    if (pSelf->pOrder && pSelf->pCount)
        pSelf->pOrder[(*pSelf->pCount)++] = pSelf->id;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_RootDrawProbeClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GUM_WIDGET_CLASS(pClass)->pFnDraw = GUM_RootDrawProbe_draw_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_RootDrawProbe_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_RootDrawProbe"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){ .classSize    = sizeof(GUM_RootDrawProbeClass),
                                                       .instanceSize = sizeof(GUM_RootDrawProbe),
                                                       .pFnClassInit = GUM_RootDrawProbeClass_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

static GUM_RootDrawProbe* drawProbeCreate_(uint8_t id, uint8_t zIndex,
                                           uint8_t* pOrder, size_t* pCount) {
    GUM_RootDrawProbe* pProbe = GBL_NEW(GUM_RootDrawProbe, "z_index", zIndex);
    if (pProbe) {
        pProbe->id     = id;
        pProbe->pOrder = pOrder;
        pProbe->pCount = pCount;
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
    GblModule* pRegistered = GblModule_find("GUM_Root");

    if (pSecond)
        GUM_unref(pSecond);

    GBL_TEST_VERIFY(!pSecond);
    GBL_TEST_COMPARE(pRegistered, GBL_MODULE(pFixture->pRoot));
GBL_TEST_CASE_END

GBL_TEST_CASE(drawOrderTracksZAndEnableOrder)
    uint8_t order[3] = { 0 };
    size_t count = 0;

    GUM_RootDrawProbe* pA = drawProbeCreate_(1, 100, order, &count);
    GUM_RootDrawProbe* pB = drawProbeCreate_(2,  50, order, &count);
    GUM_RootDrawProbe* pC = drawProbeCreate_(3, 100, order, &count);

    GBL_TEST_VERIFY(pA && pB && pC);

    GBL_TEST_COMPARE(GUM_draw(), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(count, 3);
    GBL_TEST_COMPARE(order[0], 2);
    GBL_TEST_COMPARE(order[1], 1);
    GBL_TEST_COMPARE(order[2], 3);

    count = 0;
    GBL_TEST_COMPARE(GUM_setProperty(pB, "z_index", (uint8_t)100), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GUM_draw(), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(count, 3);
    GBL_TEST_COMPARE(order[0], 1);
    GBL_TEST_COMPARE(order[1], 2);
    GBL_TEST_COMPARE(order[2], 3);

    count = 0;
    GBL_TEST_COMPARE(GUM_draw_disable(pA), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GUM_draw_enable(pA), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(GUM_draw(), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(count, 3);
    GBL_TEST_COMPARE(order[0], 2);
    GBL_TEST_COMPARE(order[1], 3);
    GBL_TEST_COMPARE(order[2], 1);

    GUM_unref(pA);
    GUM_unref(pB);
    GUM_unref(pC);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(singleRoot,
                  drawOrderTracksZAndEnableOrder)

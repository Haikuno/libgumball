#include "elements/gumball_common_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GUM_RECURSIVE_PROBE_TYPE (GBL_TYPEID(GUM_RecursiveProbe))
#define GUM_RECURSIVE_PROBE(self) (GBL_CAST(GUM_RecursiveProbe, self))

GBL_FORWARD_DECLARE_STRUCT(GUM_RecursiveProbe);
GblType GUM_RecursiveProbe_type(void) GBL_NOEXCEPT;

GBL_CLASS_DERIVE_EMPTY(GUM_RecursiveProbe, GUM_Widget)
GBL_INSTANCE_DERIVE_EMPTY(GUM_RecursiveProbe, GUM_Widget)

static size_t     updateCount_        = 0;
static size_t     drawCount_          = 0;
static GBL_RESULT nestedUpdateResult_ = GBL_RESULT_SUCCESS;
static GBL_RESULT nestedDrawResult_   = GBL_RESULT_SUCCESS;

static GBL_RESULT GUM_RecursiveProbe_update_(GUM_Widget* pWidget) {
    GBL_UNUSED(pWidget);
    ++updateCount_;
    nestedUpdateResult_ = GUM_update();
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_RecursiveProbe_draw_(GUM_Widget* pWidget, GUM_Renderer* pRenderer) {
    GBL_UNUSED(pWidget);
    ++drawCount_;
    nestedDrawResult_ = GUM_draw(pRenderer);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_RecursiveProbeClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GUM_WIDGET_CLASS(pClass)->pFnUpdate = GUM_RecursiveProbe_update_;
    GUM_WIDGET_CLASS(pClass)->pFnDraw   = GUM_RecursiveProbe_draw_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_RecursiveProbe_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_RecursiveProbe"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){
                                    .classSize    = sizeof(GUM_RecursiveProbeClass),
                                    .instanceSize = sizeof(GUM_RecursiveProbe),
                                    .pFnClassInit = GUM_RecursiveProbeClass_init_
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

#define GBL_SELF_TYPE GUM_CommonTestSuite

GBL_TEST_FIXTURE {
    GUM_Root* pRoot;
};

GBL_TEST_INIT()
    updateCount_        = 0;
    drawCount_          = 0;
    nestedUpdateResult_ = GBL_RESULT_SUCCESS;
    nestedDrawResult_   = GBL_RESULT_SUCCESS;
    pFixture->pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GUM_unref(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(updateReentry)
    GUM_RecursiveProbe* pProbe = GBL_NEW(GUM_RecursiveProbe);
    GBL_TEST_VERIFY(pProbe);

    GBL_TEST_COMPARE(GUM_update(), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(nestedUpdateResult_, GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_TEST_COMPARE(updateCount_, 1u);

    GUM_unref(pProbe);
GBL_TEST_CASE_END

GBL_TEST_CASE(drawReentry)
    GUM_RecursiveProbe* pProbe = GBL_NEW(GUM_RecursiveProbe);
    GBL_TEST_VERIFY(pProbe);

    GBL_TEST_COMPARE(GUM_draw(nullptr), GBL_RESULT_SUCCESS);
    GBL_TEST_COMPARE(nestedDrawResult_, GBL_RESULT_ERROR_INVALID_OPERATION);
    GBL_TEST_COMPARE(drawCount_, 1u);

    GUM_unref(pProbe);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(updateReentry,
                  drawReentry)

#include "elements/gumball_hierarchy_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#define GUM_HIERARCHY_PROBE_TYPE (GBL_TYPEID(GUM_HierarchyProbe))

GBL_FORWARD_DECLARE_STRUCT(GUM_HierarchyProbe);
GblType GUM_HierarchyProbe_type(void) GBL_NOEXCEPT;

GBL_CLASS_DERIVE_EMPTY(GUM_HierarchyProbe, GUM_Widget)
GBL_INSTANCE_DERIVE_EMPTY(GUM_HierarchyProbe, GUM_Widget)

static size_t drawCount_ = 0;

static GBL_RESULT GUM_HierarchyProbe_draw_(GUM_Widget* pWidget, GUM_Renderer* pRenderer) {
    GBL_UNUSED(pWidget, pRenderer);
    ++drawCount_;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_HierarchyProbeClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GUM_WIDGET_CLASS(pClass)->pFnDraw = GUM_HierarchyProbe_draw_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_HierarchyProbe_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if(type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_HierarchyProbe"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){
                                    .classSize    = sizeof(GUM_HierarchyProbeClass),
                                    .instanceSize = sizeof(GUM_HierarchyProbe),
                                    .pFnClassInit = GUM_HierarchyProbeClass_init_
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

#define GBL_SELF_TYPE GUM_HierarchyTestSuite

GBL_TEST_FIXTURE {
    GUM_Root* pRoot;
};

GBL_TEST_INIT()
    drawCount_ = 0;
    pFixture->pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    GUM_unref(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(childrenList)
    GblObject* pFirst  = GBL_NEW(GblObject);
    GblObject* pSecond = GBL_NEW(GblObject);
    GblObject* pThird  = GBL_NEW(GblObject);
    GBL_TEST_VERIFY(pFirst && pSecond && pThird);

    GblRingList* pChildren = GUM_childrenList(pFirst, pSecond, pThird);
    GBL_TEST_VERIFY(pChildren);
    GBL_TEST_COMPARE(GblRingList_size(pChildren), 3u);
    GBL_TEST_COMPARE(GblRingList_at(pChildren, 0), pFirst);
    GBL_TEST_COMPARE(GblRingList_at(pChildren, 1), pSecond);
    GBL_TEST_COMPARE(GblRingList_at(pChildren, 2), pThird);

    GblRingList_unref(pChildren);
    GUM_unref(pThird);
    GUM_unref(pSecond);
    GUM_unref(pFirst);
GBL_TEST_CASE_END

GBL_TEST_CASE(objectBridge)
    GUM_Widget* pOuter = GUM_Widget_create();
    GblObject* pBridge = GBL_NEW(GblObject);
    GUM_HierarchyProbe* pNested = GBL_NEW(GUM_HierarchyProbe);
    GBL_TEST_VERIFY(pOuter && pBridge && pNested);

    GBL_TEST_CALL(GUM_add_child(pOuter, pBridge));
    GBL_TEST_CALL(GUM_add_child(pBridge, pNested));
    GBL_TEST_COMPARE(GblObject_parent(pBridge), GBL_OBJECT(pOuter));
    GBL_TEST_COMPARE(GblObject_parent(GBL_OBJECT(pNested)), pBridge);

    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(drawCount_, 1u);

    GBL_TEST_CALL(GUM_remove_child(pOuter, pBridge));
    drawCount_ = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(drawCount_, 0u);

    GBL_TEST_CALL(GUM_add_child(pOuter, pBridge));
    drawCount_ = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(drawCount_, 1u);

    GBL_TEST_CALL(GUM_remove_child(pFixture->pRoot, pOuter));
    drawCount_ = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(drawCount_, 0u);

    GBL_TEST_CALL(GUM_add_child(pFixture->pRoot, pOuter));
    GBL_TEST_CALL(GUM_draw_disableAll(pOuter));
    drawCount_ = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(drawCount_, 0u);

    GBL_TEST_CALL(GUM_draw_enableAll(pOuter));
    drawCount_ = 0;
    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(drawCount_, 1u);

    GBL_TEST_CALL(GUM_update_disableAll(pOuter));
    GBL_TEST_VERIFY(!pOuter->shouldUpdate);
    GBL_TEST_VERIFY(!GUM_WIDGET(pNested)->shouldUpdate);
    GBL_TEST_CALL(GUM_update_enableAll(pOuter));
    GBL_TEST_VERIFY(pOuter->shouldUpdate);
    GBL_TEST_VERIFY(GUM_WIDGET(pNested)->shouldUpdate);

    GUM_unref(pOuter);
GBL_TEST_CASE_END

GBL_TEST_CASE(constructionBridge)
    GUM_HierarchyProbe* pNested = GBL_NEW(GUM_HierarchyProbe);
    GBL_TEST_VERIFY(pNested);
    GBL_TEST_CALL(GUM_remove_child(pFixture->pRoot, pNested));

    GblObject* pBridge = GBL_NEW(GblObject,
                                 "children", GUM_childrenList(pNested));
    GBL_TEST_VERIFY(pBridge);
    GBL_TEST_COMPARE(GblObject_parent(GBL_OBJECT(pNested)), pBridge);

    GUM_Widget* pOuter = GUM_Widget_create("children", GUM_childrenList(pBridge));
    GBL_TEST_VERIFY(pOuter);
    GBL_TEST_COMPARE(GblObject_parent(pBridge), GBL_OBJECT(pOuter));
    GBL_TEST_COMPARE(GblObject_parent(GBL_OBJECT(pOuter)), GBL_OBJECT(pFixture->pRoot));

    GBL_TEST_CALL(GUM_draw());
    GBL_TEST_COMPARE(drawCount_, 1u);

    GUM_unref(pOuter);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(childrenList,
                  objectBridge,
                  constructionBridge)

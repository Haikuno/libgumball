#include "elements/gumball_container_test_suite.h"
#include <gimbal/test/gimbal_test_macros.h>
#include <gumball/gumball.h>

#include "../../../lib/src/elements/gumball_container_.h"

#define GUM_LAYOUT_FAILURE_CONTAINER_TYPE (GBL_TYPEID(GUM_LayoutFailureContainer))

GBL_FORWARD_DECLARE_STRUCT(GUM_LayoutFailureContainer);
GblType GUM_LayoutFailureContainer_type(void) GBL_NOEXCEPT;

GBL_CLASS_DERIVE_EMPTY(GUM_LayoutFailureContainer, GUM_Container)
GBL_INSTANCE_DERIVE_EMPTY(GUM_LayoutFailureContainer, GUM_Container)

static bool layoutFailure_ = false;

static GBL_RESULT GUM_LayoutFailureContainer_updateContent_(GUM_Container* pSelf) {
    if (layoutFailure_)
        return GBL_RESULT_ERROR_INTERNAL;

    GUM_ContainerClass* pBase = GUM_CONTAINER_CLASS(GblClass_weakRefDefault(GUM_CONTAINER_TYPE));
    return pBase->pFnUpdateContent(pSelf);
}

static GBL_RESULT GUM_LayoutFailureContainerClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);
    GUM_CONTAINER_CLASS(pClass)->pFnUpdateContent = GUM_LayoutFailureContainer_updateContent_;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_LayoutFailureContainer_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_LayoutFailureContainer"),
                                GUM_CONTAINER_TYPE,
                                &(static GblTypeInfo){
                                    .classSize    = sizeof(GUM_LayoutFailureContainerClass),
                                    .instanceSize = sizeof(GUM_LayoutFailureContainer),
                                    .pFnClassInit = GUM_LayoutFailureContainerClass_init_
                                },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

#define GBL_SELF_TYPE GUM_ContainerTestSuite

GBL_TEST_FIXTURE {
    GUM_Root* pRoot;
};

GBL_TEST_INIT()
    layoutFailure_ = false;
    pFixture->pRoot = GUM_Root_create();
    GBL_TEST_VERIFY(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_FINAL()
    layoutFailure_ = false;
    GUM_unref(pFixture->pRoot);
GBL_TEST_CASE_END

GBL_TEST_CASE(properties)
    GUM_Container* pContainer = GUM_Container_create();
    GBL_TEST_VERIFY(pContainer);

    GBL_TEST_COMPARE(GUM_Container_padding(pContainer), 5.0f);
    GBL_TEST_COMPARE(GUM_Container_margin(pContainer), 2.0f);
    GBL_TEST_COMPARE(GUM_Container_minChildSize(pContainer), 0.15f);
    GBL_TEST_COMPARE(GUM_Container_direction(pContainer), GUM_DIRECTION_VERTICAL);
    GBL_TEST_VERIFY(GUM_Container_resizeWidgets(pContainer));
    GBL_TEST_VERIFY(GUM_Container_alignWidgets(pContainer));
    GBL_TEST_VERIFY(GUM_Container_scrollable(pContainer));

    GBL_TEST_CALL(GUM_setProperty(pContainer, "padding", 7.0f));
    GBL_TEST_CALL(GUM_setProperty(pContainer, "margin", 3.0f));
    GBL_TEST_CALL(GUM_setProperty(pContainer, "minChildSize", 0.25f));
    GBL_TEST_CALL(GUM_setProperty(pContainer, "direction", GUM_DIRECTION_HORIZONTAL));
    GBL_TEST_CALL(GUM_setProperty(pContainer, "resizeWidgets", false));
    GBL_TEST_CALL(GUM_setProperty(pContainer, "alignWidgets", false));
    GBL_TEST_CALL(GUM_setProperty(pContainer, "scrollable", false));

    GBL_TEST_COMPARE(GUM_Container_padding(pContainer), 7.0f);
    GBL_TEST_COMPARE(GUM_Container_margin(pContainer), 3.0f);
    GBL_TEST_COMPARE(GUM_Container_minChildSize(pContainer), 0.25f);
    GBL_TEST_COMPARE(GUM_Container_direction(pContainer), GUM_DIRECTION_HORIZONTAL);
    GBL_TEST_VERIFY(!GUM_Container_resizeWidgets(pContainer));
    GBL_TEST_VERIFY(!GUM_Container_alignWidgets(pContainer));
    GBL_TEST_VERIFY(!GUM_Container_scrollable(pContainer));

    GUM_unref(pContainer);
GBL_TEST_CASE_END

GBL_TEST_CASE(vertical)
    GUM_Container* pContainer = GUM_Container_create("w", 100.0f,
                                                     "h", 100.0f,
                                                     "padding", 10.0f,
                                                     "margin", 5.0f);
    GUM_Widget* pFirst  = GUM_Widget_create("parent", pContainer);
    GUM_Widget* pSecond = GUM_Widget_create("parent", pContainer);
    GBL_TEST_VERIFY(pContainer && pFirst && pSecond);

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

GBL_TEST_CASE(horizontal)
    GUM_Container* pContainer = GUM_Container_create("w", 100.0f,
                                                     "h", 60.0f,
                                                     "padding", 10.0f,
                                                     "margin", 5.0f,
                                                     "direction", GUM_DIRECTION_HORIZONTAL);
    GUM_Widget* pFirst  = GUM_Widget_create("parent", pContainer);
    GUM_Widget* pSecond = GUM_Widget_create("parent", pContainer);
    GBL_TEST_VERIFY(pContainer && pFirst && pSecond);

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

GBL_TEST_CASE(minChildSize)
    GUM_Container* pContainer = GUM_Container_create("w", 100.0f,
                                                     "h", 100.0f,
                                                     "padding", 0.0f,
                                                     "margin", 0.0f,
                                                     "minChildSize", 0.4f);
    GUM_Widget* pFirst  = GUM_Widget_create("parent", pContainer);
    GUM_Widget* pSecond = GUM_Widget_create("parent", pContainer);
    GUM_Widget* pThird  = GUM_Widget_create("parent", pContainer);
    GBL_TEST_VERIFY(pContainer && pFirst && pSecond && pThird);

    GBL_TEST_COMPARE(pFirst->h, 40.0f);
    GBL_TEST_COMPARE(pSecond->h, 40.0f);
    GBL_TEST_COMPARE(pThird->h, 40.0f);
    GBL_TEST_COMPARE(pFirst->y, 0.0f);
    GBL_TEST_COMPARE(pSecond->y, 40.0f);
    GBL_TEST_COMPARE(pThird->y, 80.0f);

    GUM_unref(pContainer);
GBL_TEST_CASE_END

GBL_TEST_CASE(scroll)
    GUM_Container* pContainer = GUM_Container_create("w", 100.0f,
                                                     "h", 100.0f,
                                                     "padding", 0.0f,
                                                     "margin", 0.0f,
                                                     "minChildSize", 0.4f);
    GBL_TEST_VERIFY(pContainer);
    GUM_Widget_create("parent", pContainer);
    GUM_Widget_create("parent", pContainer);
    GUM_Widget_create("parent", pContainer);

    GUM_Container_* pPrivate = GUM_CONTAINER_(pContainer);
    pPrivate->scrollAnimatorY.from    = 50.0f;
    pPrivate->scrollAnimatorY.current = 50.0f;
    pPrivate->scrollAnimatorY.to      = 50.0f;
    GBL_TEST_CALL(GUM_CONTAINER_CLASSOF(pContainer)->pFnUpdateContent(pContainer));

    GBL_TEST_COMPARE(pPrivate->scrollAnimatorY.from, 20.0f);
    GBL_TEST_COMPARE(pPrivate->scrollAnimatorY.current, 20.0f);
    GBL_TEST_COMPARE(pPrivate->scrollAnimatorY.to, 20.0f);

    pPrivate->scrollAnimatorY.to = 0.0f;
    GUM_Container_scrollTo_(pContainer, GUM_DIRECTION_VERTICAL, 50.0f);
    GBL_TEST_COMPARE(pPrivate->scrollAnimatorY.to, 20.0f);

    GUM_unref(pContainer);
GBL_TEST_CASE_END

GBL_TEST_CASE(layoutFailure)
    GUM_Container* pParent = GUM_Container_create();
    GBL_TEST_VERIFY(pParent);

    GUM_LayoutFailureContainer* pChild = GBL_NEW(GUM_LayoutFailureContainer, "parent", pParent);
    GUM_Widget* pWidget = GUM_Widget_create();
    GBL_TEST_VERIFY(pChild && pWidget);
    GBL_TEST_CALL(GUM_remove_child(pFixture->pRoot, pWidget));

    layoutFailure_ = true;
    const GBL_RESULT layoutResult   = GUM_CONTAINER_CLASSOF(pParent)->pFnUpdateContent(pParent);
    const GBL_RESULT propertyResult = GUM_setProperty(pChild, "padding", 9.0f);
    const float padding             = GUM_Container_padding(GUM_CONTAINER(pChild));
    const GBL_RESULT addResult      = GUM_add_child(pChild, pWidget);
    const bool parentCommitted      = GblObject_parent(GBL_OBJECT(pWidget)) == GBL_OBJECT(pChild);
    const GBL_RESULT removeResult   = GUM_remove_child(pChild, pWidget);
    const bool detachCommitted      = !GblObject_parent(GBL_OBJECT(pWidget));
    layoutFailure_ = false;

    GUM_unref(pWidget);
    GUM_unref(pParent);

    GBL_TEST_COMPARE(layoutResult, GBL_RESULT_ERROR_INTERNAL);
    GBL_TEST_COMPARE(propertyResult, GBL_RESULT_ERROR_INTERNAL);
    GBL_TEST_COMPARE(padding, 9.0f);
    GBL_TEST_COMPARE(addResult, GBL_RESULT_ERROR_INTERNAL);
    GBL_TEST_VERIFY(parentCommitted);
    GBL_TEST_COMPARE(removeResult, GBL_RESULT_ERROR_INTERNAL);
    GBL_TEST_VERIFY(detachCommitted);
GBL_TEST_CASE_END

GBL_TEST_CASE(retainedChild)
    GUM_Container* pContainer = GUM_Container_create();
    GUM_Widget* pChild = GUM_Widget_create("parent", pContainer);
    GBL_TEST_VERIFY(pContainer && pChild);
    GUM_ref(pChild);

    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pChild)), 2u);
    GUM_unref(pContainer);
    GBL_TEST_COMPARE(GblBox_refCount(GBL_BOX(pChild)), 1u);
    GBL_TEST_VERIFY(!GblObject_parent(GBL_OBJECT(pChild)));

    GUM_unref(pChild);
GBL_TEST_CASE_END

GBL_TEST_REGISTER(properties,
                  vertical,
                  horizontal,
                  minChildSize,
                  scroll,
                  layoutFailure,
                  retainedChild)

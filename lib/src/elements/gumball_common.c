#include <gumball/elements/gumball_widget.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/elements/gumball_common.h>
#include <gumball/core/gumball_logger.h>

#include <gimbal/gimbal_containers.h>
#include <gimbal/gimbal_algorithms.h>

GBL_EXPORT GBL_RESULT (GUM_update)(void) {
    static GUM_Root *pRoot = nullptr;

    struct {
        GblArrayList array;
        char         stackData[64 * sizeof(GblObject*)];
    } stackArray;

    GblArrayList_construct(&stackArray.array,
                        sizeof(GblObject*),
                        0,
                        NULL,
                        sizeof(stackArray));

    GBL_REQUIRE_SCOPE(GUM_Root, &pRoot, "GUM_Root") {
        if(!pRoot) {
            GUM_LOG_ERROR("No root element found! Create one first.");
            GBL_SCOPE_EXIT;
        }

        GblObject *pRootObject = GBL_OBJECT(pRoot);
        GblArrayList_pushBack(&stackArray.array, &pRootObject);

        while (GblArrayList_size(&stackArray.array)) {
            GblObject *pObject;
            GblArrayList_popFront(&stackArray.array, &pObject);
            GblObject *pChild = GblObject_childFirst(pObject);

            while (pChild) {
                GUM_Widget *pChildWidget = GBL_AS(GUM_Widget, pChild);
                if (pChildWidget && pChildWidget->shouldUpdate) {
                    GUM_WIDGET_CLASSOF(pChildWidget)->pFnUpdate(pChildWidget);
                }
                GblArrayList_pushBack(&stackArray.array, &pChild);
                pChild = GblObject_siblingNext(pChild);
            }

        }
    }

    GblArrayList_destruct(&stackArray.array);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_update_disable)(GblObject *pSelf) {
    GUM_Widget *pWidget = GBL_AS(GUM_Widget, pSelf);
    if GBL_UNLIKELY(!pWidget) return GBL_RESULT_ERROR_INVALID_TYPE;

    pWidget->shouldUpdate = false;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_update_enable)(GblObject *pSelf) {
    GUM_Widget *pWidget = GBL_AS(GUM_Widget, pSelf);
    if GBL_UNLIKELY(!pWidget) return GBL_RESULT_ERROR_INVALID_TYPE;

    pWidget->shouldUpdate = true;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_update_disableAll)(GblObject *pSelf) {
    size_t childCount = GblObject_childCount(pSelf);
    for (size_t i = 0; i < childCount; i++) {
        GblObject *childObj = GblObject_findChildByIndex(pSelf, i);
        (GUM_update_disable)(childObj);
    }
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_update_enableAll)(GblObject *pSelf) {
    size_t childCount = GblObject_childCount(pSelf);
    for (size_t i = 0; i < childCount; i++) {
        GblObject *childObj = GblObject_findChildByIndex(pSelf, i);
        (GUM_update_enable)(childObj);
    }
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_draw(GUM_Renderer *pRenderer) {
    size_t queueSize = GblArrayList_size(GUM_drawQueue_get());

    for (size_t i = 0; i < queueSize; i++) {
        GblObject **pObj = GblArrayList_at(GUM_drawQueue_get(), i);
        GUM_WidgetClass *pWidgetClass = GBL_CLASS_AS(GUM_Widget, GBL_OBJECT_GET_CLASS(*pObj));
        pWidgetClass->pFnDraw(GUM_WIDGET(*pObj), pRenderer);
    }

    return GBL_RESULT_SUCCESS;
}


GBL_EXPORT void (GUM_draw_disable)(GblObject *pSelf) {
    GUM_drawQueue_remove(pSelf);
}

GBL_EXPORT void (GUM_draw_enable)(GblObject *pSelf) {
    GUM_drawQueue_push(pSelf);
}

GBL_EXPORT void (GUM_draw_disableAll)(GblObject *pSelf) {
    size_t childCount = GblObject_childCount(pSelf);
    for (size_t i = 0; i < childCount; i++) {
        GblObject *childObj = GblObject_findChildByIndex(pSelf, i);
        GUM_draw_disableAll(childObj);
    }
    GUM_draw_disable(pSelf);
}

GBL_EXPORT void (GUM_draw_enableAll)(GblObject *pSelf) {
    GUM_draw_enable(pSelf);
    size_t childCount = GblObject_childCount(pSelf);
    for (size_t i = childCount; i -- > 0;) {
        GblObject *childObj = GblObject_findChildByIndex(pSelf, i);
        GUM_draw_enableAll(childObj);
    }
}

GBL_EXPORT GBL_RESULT (GUM_unref)(GblObject *pSelf) {
    GUM_draw_disableAll(pSelf);

    GUM_Widget *pWidget = GBL_AS(GUM_Widget, pSelf);
    if (pWidget) {
        GUM_WIDGET_CLASSOF(pSelf)->pFnDeactivate(pWidget);
    }

    size_t childCount = GblObject_childCount(pSelf);
    for (size_t i = childCount; i-- > 0;) {
        GblObject *childObj = GblObject_findChildByIndex(pSelf, i);
        if (childObj) GUM_unref(childObj);
    }

    if GBL_UNLIKELY(GBL_TYPEOF(pSelf) == GUM_ROOT_TYPE) {
        GblModule_unregister(GBL_MODULE(pSelf));
    }

    GBL_UNREF(pSelf);

    return GBL_RESULT_SUCCESS;
}

#include <gumball/elements/gumball_widget.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/elements/gumball_common.h>

#include <gimbal/gimbal_containers.h>
#include <gimbal/gimbal_algorithms.h>

GBL_RESULT (GUM_update)(GblObject* pSelf) {
    size_t childCount = GblObject_childCount(pSelf);

    GblObjectClass *pClass           = GBL_OBJECT_GET_CLASS(pSelf); // GUM_TODO: change this to GBL_OBJECT_CLASSOF once libGimbal is updated
    GUM_WidgetClass *pWidgetClass_   = GBL_CLASS_AS(GUM_Widget, pClass);

    if GBL_LIKELY(pWidgetClass_) {
        pWidgetClass_->pFnUpdate(GUM_WIDGET(pSelf));
    }

    for (size_t i = 0; i < childCount; i++) {
        GblObject* childObj = GblObject_findChildByIndex(pSelf, i);
        GUM_update(childObj);
    }

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_draw(GUM_Renderer *pRenderer) {
    size_t queueSize = GblArrayList_size(GUM_drawQueue_get());

    for (size_t i = 0; i < queueSize; i++) {
        GblObject **pObj = GblArrayList_at(GUM_drawQueue_get(), i);
        GUM_WidgetClass *pWidgetClass = GBL_CLASS_AS(GUM_Widget, GBL_OBJECT_GET_CLASS(*pObj));
        pWidgetClass->pFnDraw(GUM_WIDGET(*pObj), pRenderer);
    }

    return GBL_RESULT_SUCCESS;
}


GBL_RESULT (GUM_unref)(GblObject* pSelf) {
    GUM_draw_disableAll(pSelf);

    GUM_Widget *pWidget = GBL_AS(GUM_Widget, pSelf);
    if (pWidget) {
        GUM_WIDGET_CLASSOF(pSelf)->pFnDeactivate(pWidget);
    }

    size_t childCount = GblObject_childCount(pSelf);
    for (size_t i = childCount; i-- > 0;) {
        GblObject* childObj = GblObject_findChildByIndex(pSelf, i);
        if GBL_UNLIKELY(childObj) GUM_unref(childObj);
    }

    GBL_UNREF(pSelf);

    return GBL_RESULT_SUCCESS;
}

void (GUM_draw_disable)(GblObject *pSelf) {
    GUM_drawQueue_remove(pSelf);
}

void (GUM_draw_enable)(GblObject *pSelf) {
    GUM_drawQueue_push(pSelf);
}

void (GUM_draw_disableAll)(GblObject *pSelf) {
    size_t childCount = GblObject_childCount(pSelf);
    for (size_t i = 0; i < childCount; i++) {
        GblObject *childObj = GblObject_findChildByIndex(pSelf, i);
        GUM_draw_disableAll(childObj);
    }
    GUM_draw_disable(pSelf);
}

void (GUM_draw_enableAll)(GblObject *pSelf) {
    GUM_draw_enable(pSelf);
    size_t childCount = GblObject_childCount(pSelf);
    for (size_t i = childCount; i -- > 0;) {
        GblObject *childObj = GblObject_findChildByIndex(pSelf, i);
        GUM_draw_enableAll(childObj);
    }
}
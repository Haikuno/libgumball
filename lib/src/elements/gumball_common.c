#include <gumball/elements/gumball_widget.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/elements/gumball_common.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/types/gumball_rectangle.h>

#include <gimbal/gimbal_containers.h>
#include <gimbal/gimbal_algorithms.h>

#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_inputsystem.h>

#include "gumball_root_.h"

static void GUM_update_recursive_(GblObject* pObject) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pObject);
    if (pWidget && pWidget->shouldUpdate)
        GUM_WIDGET_CLASSOF(pWidget)->pFnUpdate(pWidget);

    GblObject_foreachChild(pObject, pChild)
        GUM_update_recursive_(pChild);
}

GBL_EXPORT GBL_RESULT (GUM_update)(void) {
    GUM_Root* pRoot = GUM_Root_active_();
    if GBL_UNLIKELY (!pRoot) {
        GUM_LOG_ERROR("No root element found! Create one first.");
        return GBL_RESULT_NOT_FOUND;
    }

    GUM_Root_update(pRoot);
    GUM_update_recursive_(GBL_OBJECT(pRoot));
    GUM_Widget_animate_update_();
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_update_disable)(GblObject* pSelf) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pSelf);
    if GBL_UNLIKELY (!pWidget)
        return GBL_RESULT_ERROR_INVALID_TYPE;

    pWidget->shouldUpdate = false;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_update_enable)(GblObject* pSelf) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pSelf);
    if GBL_UNLIKELY (!pWidget)
        return GBL_RESULT_ERROR_INVALID_TYPE;

    pWidget->shouldUpdate = true;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_update_disableAll)(GblObject* pSelf) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pSelf);
    if GBL_UNLIKELY (!pWidget)
        return GBL_RESULT_ERROR_INVALID_TYPE;

    GUM_update_disable(pSelf);
    GblObject_foreachChild(pSelf, pChild)
        (GUM_update_disableAll)(pChild);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_update_enableAll)(GblObject* pSelf) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pSelf);
    if GBL_UNLIKELY (!pWidget)
        return GBL_RESULT_ERROR_INVALID_TYPE;

    GUM_update_enable(pSelf);
    GblObject_foreachChild(pSelf, pChild)
        (GUM_update_enableAll)(pChild);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_draw(GUM_Renderer* pRenderer) {
    GUM_Root* pRoot = GUM_Root_active_();
    if GBL_UNLIKELY (!pRoot)
        return GBL_RESULT_NOT_FOUND;

    const GBL_RESULT result = GUM_Root_draw_(pRoot, pRenderer);
    if (GBL_RESULT_SUCCESS(result))
        GUM_InputSystem_drawFocusRings(pRenderer);

    return result;
}

GBL_EXPORT GBL_RESULT (GUM_draw_disable)(GblObject* pSelf) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pSelf);
    if GBL_UNLIKELY (!pWidget)
        return GBL_RESULT_ERROR_INVALID_TYPE;

    GUM_Root_drawDisable_(pWidget);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_draw_enable)(GblObject* pSelf) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pSelf);
    if GBL_UNLIKELY (!pWidget)
        return GBL_RESULT_ERROR_INVALID_TYPE;

    return GUM_Root_drawEnable_(pWidget);
}

GBL_EXPORT GBL_RESULT (GUM_draw_disableAll)(GblObject* pSelf) {
    GBL_RESULT result = GUM_draw_disable(pSelf);
    if GBL_UNLIKELY (result != GBL_RESULT_SUCCESS)
        return result;

    GblObject_foreachChild(pSelf, pChild)
        (GUM_draw_disableAll)(pChild);

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_draw_enableAll)(GblObject* pSelf) {
    GBL_RESULT result = GUM_draw_enable(pSelf);
    if GBL_UNLIKELY (result != GBL_RESULT_SUCCESS)
        return result;

    GblObject_foreachChild(pSelf, pChild)
        GUM_draw_enableAll(pChild);

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GblObject* (GUM_ref)(GblObject* pSelf) {
    return GBL_OBJECT(GBL_REF(pSelf));
}

GBL_EXPORT GBL_RESULT (GUM_unref)(GblObject* pSelf) {
    GUM_draw_disableAll(pSelf);

    GblObject_foreachChildReverse(pSelf, pChild) {
        GblObject_setParent(pChild, nullptr);
        GUM_unref(pChild);
    }

    if GBL_UNLIKELY (GBL_TYPEOF(pSelf) == GUM_ROOT_TYPE)
        GblModule_unregister(GBL_MODULE(pSelf));

    GBL_UNREF(pSelf);

    return GBL_RESULT_SUCCESS;
}

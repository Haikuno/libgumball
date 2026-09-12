#include <gumball/elements/gumball_widget.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/elements/gumball_common.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/types/gumball_rectangle.h>

#include <gimbal/gimbal_containers.h>
#include <gimbal/gimbal_algorithms.h>

#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_inputsystem.h>

#include "../core/gumball_inputsystem_.h"
#include "gumball_root_.h"
#include "gumball_widget_.h"

#include <stdarg.h>
#include <string.h>

typedef struct GUM_UpdateSnapshot_ {
    GblArrayList widgets;
    GUM_Widget*  stack[GUM_SNAPSHOT_INLINE_CAPACITY];
} GUM_UpdateSnapshot_;

static bool updating_ = false;
static bool drawing_  = false;

static void GUM_update_snapshotRelease_(GUM_UpdateSnapshot_* pSnapshot) {
    const size_t count = GblArrayList_size(&pSnapshot->widgets);
    for (size_t i = 0; i < count; ++i) {
        GUM_Widget* pWidget = *(GUM_Widget**)GblArrayList_at(&pSnapshot->widgets, i);
        GblBox_unref(GBL_BOX(pWidget));
    }
    GblArrayList_destruct(&pSnapshot->widgets);
}

static GBL_RESULT GUM_update_snapshotAppend_(GblObject* pObject, GUM_UpdateSnapshot_* pSnapshot) {
    GblObject_foreachChild(pObject, pChild) {
        GUM_Widget* pWidget = GBL_AS(GUM_Widget, pChild);
        if (pWidget) {
            GUM_Widget* pRetained = GUM_WIDGET(GblBox_ref(GBL_BOX(pWidget)));
            const GBL_RESULT result = GblArrayList_pushBack(&pSnapshot->widgets, &pRetained);
            if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
                GblBox_unref(GBL_BOX(pRetained));
                return result;
            }
        }

        const GBL_RESULT result = GUM_update_snapshotAppend_(pChild, pSnapshot);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            return result;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_update_snapshot_(GblObject* pRoot, GUM_UpdateSnapshot_* pSnapshot) {
    GBL_RESULT result = GblArrayList_construct(&pSnapshot->widgets,
                                               sizeof(GUM_Widget*),
                                               0,
                                               nullptr,
                                               sizeof(*pSnapshot));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    result = GUM_update_snapshotAppend_(pRoot, pSnapshot);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GUM_update_snapshotRelease_(pSnapshot);
        return result;
    }

    return GBL_RESULT_SUCCESS;
}

static void GUM_drawRefreshDescendants_(GblObject* pObject) {
    GblObject_foreachChild(pObject, pChild) {
        GUM_Widget* pWidget = GBL_AS(GUM_Widget, pChild);
        if (pWidget)
            GUM_Widget_drawSetEnabled_(pWidget, GUM_WIDGET_(pWidget)->drawEnabled);

        GUM_drawRefreshDescendants_(pChild);
    }
}

static GBL_RESULT GUM_hierarchyChanged_(GblObject* pObject,
                                        GblObject* pOldParent,
                                        GblObject* pNewParent) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pObject);
    if (pWidget)
        return GUM_Widget_hierarchyChanged_(pWidget, pOldParent, pNewParent);

    // Plain GblObjects may bridge Widget subtrees.
    GUM_drawRefreshDescendants_(pObject);
    GUM_Root_drawOrderChanged_();
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_reparent_(GblObject* pChild, GblObject* pParent) {
    GblObject* pOldParent = GblObject_parent(pChild);
    if (pOldParent == pParent)
        return GBL_RESULT_SUCCESS;

    // Parent changes emit propertyChange; retain affected objects through it.
    GblBox_ref(GBL_BOX(pChild));
    if (pOldParent)
        GblBox_ref(GBL_BOX(pOldParent));

    GblObject_setParent(pChild, pParent);
    const GBL_RESULT result = GUM_hierarchyChanged_(pChild,
                                                     pOldParent,
                                                     GblObject_parent(pChild));

    if (pOldParent)
        GblBox_unref(GBL_BOX(pOldParent));
    GblBox_unref(GBL_BOX(pChild));
    return result;
}

GBL_EXPORT GBL_RESULT (GUM_update)(void) {
    if GBL_UNLIKELY (updating_) {
        GUM_LOG_WARN("Ignoring recursive GUM_update().");
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    }

    GUM_Root* pRoot = GUM_Root_active_();
    if GBL_UNLIKELY (!pRoot) {
        GUM_LOG_ERROR("No root element found! Create one first.");
        return GBL_RESULT_NOT_FOUND;
    }

    updating_ = true;
    GblBox_ref(GBL_BOX(pRoot));
    GBL_RESULT result = GUM_Root_update(pRoot);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GblBox_unref(GBL_BOX(pRoot));
        updating_ = false;
        return result;
    }

    if (GUM_Root_active_() != pRoot) {
        GblBox_unref(GBL_BOX(pRoot));
        updating_ = false;
        return GBL_RESULT_SUCCESS;
    }

    GUM_UpdateSnapshot_ snapshot;
    result = GUM_update_snapshot_(GBL_OBJECT(pRoot), &snapshot);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GblBox_unref(GBL_BOX(pRoot));
        updating_ = false;
        return result;
    }

    GBL_RESULT firstFailure = GBL_RESULT_SUCCESS;
    const size_t count = GblArrayList_size(&snapshot.widgets);
    bool rootActive = true;

    for (size_t i = 0; i < count; ++i) {
        GUM_Widget* pWidget = *(GUM_Widget**)GblArrayList_at(&snapshot.widgets, i);
        if (!pWidget->shouldUpdate)
            continue;

        const GBL_RESULT updateResult = GUM_WIDGET_CLASSOF(pWidget)->pFnUpdate(pWidget);
        if (GBL_RESULT_SUCCESS(firstFailure) && !GBL_RESULT_SUCCESS(updateResult))
            firstFailure = updateResult;

        if (GUM_Root_active_() != pRoot) {
            rootActive = false;
            break;
        }
    }

    GUM_update_snapshotRelease_(&snapshot);
    if (rootActive)
        GUM_Widget_animate_update_();

    GblBox_unref(GBL_BOX(pRoot));
    updating_ = false;
    return firstFailure;
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

static void GUM_updateSetSubtree_(GblObject* pObject, bool enabled) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pObject);
    if (pWidget)
        pWidget->shouldUpdate = enabled;

    GblObject_foreachChild(pObject, pChild)
        GUM_updateSetSubtree_(pChild, enabled);
}

GBL_EXPORT GBL_RESULT (GUM_update_disableAll)(GblObject* pSelf) {
    if GBL_UNLIKELY (!GBL_AS(GUM_Widget, pSelf))
        return GBL_RESULT_ERROR_INVALID_TYPE;

    GUM_updateSetSubtree_(pSelf, false);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_update_enableAll)(GblObject* pSelf) {
    if GBL_UNLIKELY (!GBL_AS(GUM_Widget, pSelf))
        return GBL_RESULT_ERROR_INVALID_TYPE;

    GUM_updateSetSubtree_(pSelf, true);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_draw(GUM_Renderer* pRenderer) {
    if GBL_UNLIKELY (drawing_) {
        GUM_LOG_WARN("Ignoring recursive GUM_draw().");
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    }

    GUM_Root* pRoot = GUM_Root_active_();
    if GBL_UNLIKELY (!pRoot)
        return GBL_RESULT_NOT_FOUND;

    drawing_ = true;
    GblBox_ref(GBL_BOX(pRoot));
    GBL_RESULT result = GUM_Root_draw_(pRoot, pRenderer);
    if (GBL_RESULT_SUCCESS(result) && GUM_Root_active_() == pRoot)
        result = GUM_InputSystem_drawFocusRings_(pRenderer);

    GblBox_unref(GBL_BOX(pRoot));
    drawing_ = false;
    return result;
}

GBL_EXPORT GBL_RESULT (GUM_draw_disable)(GblObject* pSelf) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pSelf);
    if GBL_UNLIKELY (!pWidget)
        return GBL_RESULT_ERROR_INVALID_TYPE;

    return GUM_Widget_drawSetEnabled_(pWidget, false);
}

GBL_EXPORT GBL_RESULT (GUM_draw_enable)(GblObject* pSelf) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pSelf);
    if GBL_UNLIKELY (!pWidget)
        return GBL_RESULT_ERROR_INVALID_TYPE;

    return GUM_Widget_drawSetEnabled_(pWidget, true);
}

static GBL_RESULT GUM_drawSetSubtree_(GblObject* pObject, bool enabled) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pObject);
    if (pWidget) {
        const GBL_RESULT result = GUM_Widget_drawSetEnabled_(pWidget, enabled);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            return result;
    }

    GblObject_foreachChild(pObject, pChild) {
        const GBL_RESULT result = GUM_drawSetSubtree_(pChild, enabled);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            return result;
    }

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT (GUM_draw_disableAll)(GblObject* pSelf) {
    if GBL_UNLIKELY (!GBL_AS(GUM_Widget, pSelf))
        return GBL_RESULT_ERROR_INVALID_TYPE;

    return GUM_drawSetSubtree_(pSelf, false);
}

GBL_EXPORT GBL_RESULT (GUM_draw_enableAll)(GblObject* pSelf) {
    if GBL_UNLIKELY (!GBL_AS(GUM_Widget, pSelf))
        return GBL_RESULT_ERROR_INVALID_TYPE;

    return GUM_drawSetSubtree_(pSelf, true);
}

GBL_EXPORT GBL_RESULT (GUM_setProperty)(GblObject* pSelf, const char* pName, ...) {
    if GBL_UNLIKELY (!pSelf || !pName)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GblObject* pOldParent = nullptr;
    const bool parentMutation = strcmp(pName, "parent") == 0;
    if (parentMutation) {
        pOldParent = GblObject_parent(pSelf);
        if (pOldParent)
            GblBox_ref(GBL_BOX(pOldParent));
    }

    GblBox_ref(GBL_BOX(pSelf));

    va_list args;
    va_start(args, pName);
    GBL_RESULT result = GblObject_setPropertyVa(pSelf, pName, &args);
    va_end(args);

    if (parentMutation && GBL_RESULT_SUCCESS(result))
        result = GUM_hierarchyChanged_(pSelf, pOldParent, GblObject_parent(pSelf));

    if (pOldParent)
        GblBox_unref(GBL_BOX(pOldParent));
    GblBox_unref(GBL_BOX(pSelf));
    return result;
}

GBL_EXPORT GBL_RESULT (GUM_add_child)(GblObject* pSelf, GblObject* pChild) {
    if GBL_UNLIKELY (!pSelf || !pChild)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    return GUM_reparent_(pChild, pSelf);
}

GBL_EXPORT GBL_RESULT (GUM_remove_child)(GblObject* pSelf, GblObject* pChild) {
    if GBL_UNLIKELY (!pSelf || !pChild)
        return GBL_RESULT_ERROR_INVALID_POINTER;
    if (GblObject_parent(pChild) != pSelf)
        return GBL_RESULT_NOT_FOUND;

    return GUM_reparent_(pChild, nullptr);
}

GBL_EXPORT GblObject* (GUM_ref)(GblObject* pSelf) {
    return GBL_OBJECT(GBL_REF(pSelf));
}

GBL_EXPORT GBL_RESULT (GUM_unref)(GblObject* pSelf) {
    if GBL_UNLIKELY (!pSelf)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GBL_RESULT firstFailure = GBL_RESULT_SUCCESS;

    // Detach while the object still owns its live reference.
    if (GblObject_parent(pSelf)) {
        const GBL_RESULT result = GUM_reparent_(pSelf, nullptr);
        if (!GBL_RESULT_SUCCESS(result))
            firstFailure = result;
    }

    // Consume the tail because reparenting mutates the sibling list.
    for (GblObject* pChild = GblObject_childLast(pSelf);
         pChild;
         pChild = GblObject_childLast(pSelf)) {
        GBL_RESULT result = GUM_reparent_(pChild, nullptr);
        if (GBL_RESULT_SUCCESS(firstFailure) && !GBL_RESULT_SUCCESS(result))
            firstFailure = result;

        result = GUM_unref(pChild);
        if (GBL_RESULT_SUCCESS(firstFailure) && !GBL_RESULT_SUCCESS(result))
            firstFailure = result;
    }

    if GBL_UNLIKELY (GBL_TYPEOF(pSelf) == GUM_ROOT_TYPE)
        GblModule_unregister(GBL_MODULE(pSelf));

    GBL_UNREF(pSelf);
    return firstFailure;
}

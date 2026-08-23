#include <gumball/core/gumball_navigation.h>
#include <gumball/core/gumball_inputsystem.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/gumball_elements.h>
#include <gumball/gumball_types.h>

#include "gumball_inputsystem_.h"
#include "../devices/gumball_inputdevice_.h"
#include "../elements/gumball_container_.h"
#include "../elements/gumball_root_.h"
#include "../elements/gumball_widget_.h"

static GUM_Widget* GUM_Nav_asSelectable_(GblObject* pObject) {
    if (!pObject || !GBL_TYPECHECK(GUM_Widget, pObject))
        return nullptr;

    GUM_Widget* pWidget = GUM_WIDGET(pObject);
    return pWidget->isSelectable ? pWidget : nullptr;
}

static GblBool GUM_Nav_isWidgetType_(const GblObject* pObject, void* pClosure) {
    GBL_UNUSED(pClosure);
    return GBL_TYPECHECK(GUM_Widget, pObject);
}

static GblBool GUM_Nav_isContainerType_(const GblObject* pObject, void* pClosure) {
    GBL_UNUSED(pClosure);
    return GBL_TYPECHECK(GUM_Container, pObject);
}

static GblBool GUM_Nav_isSelectable_(const GblObject* pObject, void* pClosure) {
    GBL_UNUSED(pClosure);
    return GBL_TYPECHECK(GUM_Widget, pObject) && GUM_WIDGET(pObject)->isSelectable;
}

static GblObject* GUM_Nav_siblingStep_(GblObject* pObject, GblObjectCmpFn pFnCmp, bool forwards) {
    return forwards ? GblObject_siblingNextByCmpFn(pObject, pFnCmp, nullptr)
                    : GblObject_siblingPreviousByCmpFn(pObject, pFnCmp, nullptr);
}

static GUM_Widget* GUM_Nav_findSelectableSibling_(GblObject* pObject, bool forwards) {
    GblObject* pSibling = GUM_Nav_siblingStep_(pObject, GUM_Nav_isWidgetType_, forwards);

    while (pSibling) {
        GUM_Widget* pWidget = GUM_Nav_asSelectable_(pSibling);
        if (pWidget)
            return pWidget;
        pSibling = GUM_Nav_siblingStep_(pSibling, GUM_Nav_isWidgetType_, forwards);
    }

    return nullptr;
}

static GUM_Widget* GUM_Nav_findSelectableInContainer_(GblObject* pContainer,
                                                       size_t preferredIndex,
                                                       bool forwards) {
    if (preferredIndex != GBL_INDEX_INVALID) {
        GUM_Widget* pWidget = GUM_Nav_asSelectable_(GblObject_findChildByIndex(pContainer,
                                                                               preferredIndex));
        if (pWidget)
            return pWidget;
    }

    if (forwards) {
        GblObject_foreachChild(pContainer, pChild) {
            GUM_Widget* pWidget = GUM_Nav_asSelectable_(pChild);
            if (pWidget)
                return pWidget;
        }
    } else {
        GblObject_foreachChildReverse(pContainer, pChild) {
            GUM_Widget* pWidget = GUM_Nav_asSelectable_(pChild);
            if (pWidget)
                return pWidget;
        }
    }

    GblObject* pObject = GblObject_findDescendantByCmpFn(pContainer,
                                                         GUM_Nav_isSelectable_,
                                                         nullptr);
    return pObject ? GUM_WIDGET(pObject) : nullptr;
}

static GblObject* GUM_Nav_findSiblingContainer_(GblObject* pContainer, bool forwards) {
    GblObject* pSibling = GUM_Nav_siblingStep_(pContainer,
                                               GUM_Nav_isContainerType_,
                                               forwards);

    while (pSibling) {
        if (GUM_Nav_findSelectableInContainer_(pSibling, GBL_INDEX_INVALID, forwards))
            return pSibling;
        pSibling = GUM_Nav_siblingStep_(pSibling, GUM_Nav_isContainerType_, forwards);
    }

    return nullptr;
}

static GUM_Vector2 GUM_Nav_closestPointInRect_(GUM_Vector2 center, GUM_Rectangle rect) {
    const float minX = rect.x;
    const float maxX = rect.x + rect.width;
    const float minY = rect.y;
    const float maxY = rect.y + rect.height;

    return (GUM_Vector2){ .x = GBL_CLAMP(center.x, minX, maxX),
                          .y = GBL_CLAMP(center.y, minY, maxY) };
}

typedef struct GUM_NavSpatialSearch_ {
    GUM_Widget* pCurrent;
    GUM_Vector2 currentCenter;
    GUM_Vector2 cursorDirection;
    GUM_Widget* pBest;
    float       bestScore;
} GUM_NavSpatialSearch_;

static GblBool GUM_Nav_spatialCandidate_(GUM_Widget* pCandidate, void* pClosure) {
    GUM_NavSpatialSearch_* pSearch = pClosure;

    if (!pCandidate->isSelectable || pCandidate == pSearch->pCurrent)
        return GBL_FALSE;

    const GUM_Vector2 candidatePos = GUM_get_absolute_position_(pCandidate);
    const GUM_Vector2 candidateClosest = GUM_Nav_closestPointInRect_(
        pSearch->currentCenter,
        (GUM_Rectangle){ candidatePos.x, candidatePos.y, pCandidate->w, pCandidate->h });
    const GUM_Vector2 delta = GUM_Vector2_subtract(candidateClosest, pSearch->currentCenter);

    const float forwardDistance = delta.x * pSearch->cursorDirection.x +
                                  delta.y * pSearch->cursorDirection.y;
    if (forwardDistance <= 0.0f)
        return GBL_FALSE;

    const float angle = GUM_Vector2_signedAngleTo(pSearch->currentCenter,
                                                   candidateClosest,
                                                   pSearch->cursorDirection);
    if (fabsf(angle) > 0.4f)
        return GBL_FALSE;

    const float score = 10.0f - GUM_Vector2_distance(candidateClosest, pSearch->currentCenter);
    if (score > pSearch->bestScore) {
        pSearch->bestScore = score;
        pSearch->pBest     = pCandidate;
    }

    return GBL_FALSE;
}

static GUM_Widget* GUM_Nav_findSelectableByPosition_(GUM_Widget* pCurrent,
                                                      GUM_InputAction direction) {
    const GUM_Vector2 currentPos = GUM_get_absolute_position_(pCurrent);
    const GUM_Vector2 currentCenter = {
        currentPos.x + pCurrent->w * 0.5f,
        currentPos.y + pCurrent->h * 0.5f
    };

    GUM_Vector2 cursorDirection;
    switch (direction) {
        case GUM_INPUTACTION_MOVE_UP:    cursorDirection = (GUM_Vector2){ 0.0f, -1.0f }; break;
        case GUM_INPUTACTION_MOVE_DOWN:  cursorDirection = (GUM_Vector2){ 0.0f,  1.0f }; break;
        case GUM_INPUTACTION_MOVE_LEFT:  cursorDirection = (GUM_Vector2){-1.0f,  0.0f }; break;
        case GUM_INPUTACTION_MOVE_RIGHT: cursorDirection = (GUM_Vector2){ 1.0f,  0.0f }; break;
        default: return nullptr;
    }

    GUM_NavSpatialSearch_ search = {
        .pCurrent        = pCurrent,
        .currentCenter   = currentCenter,
        .cursorDirection = cursorDirection,
        .bestScore       = -FLT_MAX
    };

    const GBL_RESULT result = GUM_Root_foreachDrawable_(GUM_Root_active_(),
                                                        GUM_Nav_spatialCandidate_,
                                                        &search);
    return GBL_RESULT_SUCCESS(result) ? search.pBest : nullptr;
}

static GUM_Widget* GUM_Nav_moveCursor_(GblObject* pSelf, GUM_InputAction direction) {
    GblObject* pParent = GblObject_parent(pSelf);
    if (!pParent || !GBL_TYPECHECK(GUM_Container, pParent))
        return nullptr;

    const GUM_Direction parentDirection = GUM_Container_direction(GUM_CONTAINER(pParent));
    GblObject* pGrandParent = GblObject_parent(pParent);
    const GUM_Direction grandParentDirection =
        pGrandParent && GBL_TYPECHECK(GUM_Container, pGrandParent) ?
        GUM_Container_direction(GUM_CONTAINER(pGrandParent)) : GUM_DIRECTION_NULL;

    const GUM_Direction axis =
        direction == GUM_INPUTACTION_MOVE_LEFT || direction == GUM_INPUTACTION_MOVE_RIGHT ?
        GUM_DIRECTION_HORIZONTAL : GUM_DIRECTION_VERTICAL;
    const bool forwards = direction == GUM_INPUTACTION_MOVE_RIGHT ||
                          direction == GUM_INPUTACTION_MOVE_DOWN;

    if (axis != parentDirection && axis != grandParentDirection)
        return nullptr;

    const size_t childIndex = GblObject_childIndex(pSelf);

    if (axis == parentDirection) {
        GUM_Widget* pSibling = GUM_Nav_findSelectableSibling_(pSelf, forwards);
        if (pSibling)
            return pSibling;
    }

    GblObject* pNewContainer = nullptr;
    if (axis == grandParentDirection)
        pNewContainer = GUM_Nav_findSiblingContainer_(pParent, forwards);

    if (!pNewContainer)
        return GUM_Nav_findSelectableByPosition_(GUM_WIDGET(pSelf), direction);

    const GUM_Direction newDirection = GUM_Container_direction(GUM_CONTAINER(pNewContainer));
    const size_t preferredIndex =
        newDirection    == parentDirection &&
        parentDirection != grandParentDirection &&
        GblObject_childCount(pNewContainer) > childIndex ?
        childIndex : GBL_INDEX_INVALID;

    return GUM_Nav_findSelectableInContainer_(pNewContainer, preferredIndex, forwards);
}

static GUM_Widget* GUM_Nav_findSelectableDescendant_(GblObject* pSelf, bool defaultOnly) {
    if (!pSelf)
        return nullptr;

    GUM_Widget* pResult = nullptr;
    struct {
        GblArrayList array;
        char stackData[GUM_SNAPSHOT_INLINE_CAPACITY * sizeof(GblObject*)];
    } stack;

    if (!GBL_RESULT_SUCCESS(GblArrayList_construct(&stack.array,
                                                   sizeof(GblObject*),
                                                   0,
                                                   nullptr,
                                                   sizeof(stack)))) {
        return nullptr;
    }

    if (!GBL_RESULT_SUCCESS(GblArrayList_pushBack(&stack.array, &pSelf)))
        goto done;

    while (GblArrayList_size(&stack.array)) {
        GblObject* pObject;
        GblArrayList_popFront(&stack.array, &pObject);

        GblObject_foreachChild(pObject, pChild) {
            GUM_Widget* pWidget = GUM_Nav_asSelectable_(pChild);
            if (pWidget && (!defaultOnly || pWidget->isSelectedByDefault)) {
                pResult = pWidget;
                goto done;
            }

            if (!GBL_RESULT_SUCCESS(GblArrayList_pushBack(&stack.array, &pChild)))
                goto done;
        }
    }

done:
    GblArrayList_destruct(&stack.array);
    return pResult;
}

static bool GUM_Nav_focusApply_(GUM_InputDevice* pDevice, GUM_Widget* pWidget) {
    if (!pDevice ||
        (pWidget && !GUM_InputSystem_deviceEnabled(pDevice)) ||
        GUM_InputDevice_focusedWidget(pDevice) == pWidget)
        return pDevice && GUM_InputDevice_focusedWidget(pDevice) == pWidget;

    if (!GUM_InputDevice_focusTransitionBegin_(pDevice))
        return false;

    const uint64_t inputGeneration = GUM_InputSystem_generation_();
    GblBox_ref(GBL_BOX(pDevice));
    if (pWidget)
        GblBox_ref(GBL_BOX(pWidget));

    GUM_InputDevice_focusRelease_(pDevice);

    if (pWidget &&
        (GUM_InputSystem_generation_() != inputGeneration ||
         !GUM_InputSystem_deviceEnabled(pDevice) ||
         GblBox_refCount(GBL_BOX(pDevice)) <= 1 ||
         GblBox_refCount(GBL_BOX(pWidget)) <= 1)) {
        GblBox_unref(GBL_BOX(pWidget));
        GUM_InputDevice_focusTransitionEnd_(pDevice);
        GblBox_unref(GBL_BOX(pDevice));
        return false;
    }

    GUM_InputDevice_focusSet_(pDevice, pWidget);
    if (pWidget) {
        GUM_Widget_focusAcquire_(pWidget);
        GBL_EMIT(pWidget, "onFocusGained", pDevice);
    }

    GUM_InputDevice_focusTransitionEnd_(pDevice);

    const bool committed = !pWidget ||
                           (GUM_InputSystem_generation_() == inputGeneration &&
                            GUM_InputSystem_deviceEnabled(pDevice) &&
                            GUM_InputDevice_focusedWidget(pDevice) == pWidget &&
                            GblBox_refCount(GBL_BOX(pDevice)) > 1);

    if (!committed && pWidget && GUM_InputDevice_focusedWidget(pDevice) == pWidget) {
        if (GUM_InputDevice_focusTransitionBegin_(pDevice)) {
            GUM_InputDevice_focusRelease_(pDevice);
            GUM_InputDevice_focusTransitionEnd_(pDevice);
        }
    }

    GblBox_unref(GBL_BOX(pDevice));
    return committed;
}

void GUM_Nav_focus(GUM_InputDevice* pDevice, GUM_Widget* pWidget) {
    GUM_Nav_focusApply_(pDevice, pWidget);
}

void GUM_Nav_move(GUM_InputDevice* pDevice, GUM_InputAction direction) {
    if (!pDevice || !GUM_InputSystem_deviceEnabled(pDevice))
        return;
    if (direction < GUM_INPUTACTION_MOVE_UP || direction > GUM_INPUTACTION_MOVE_RIGHT)
        return;

    GUM_Widget* pFocusedWidget = GUM_InputDevice_focusedWidget(pDevice);
    if (!pFocusedWidget) {
        GUM_Root* pRoot = GUM_Root_active_();
        if GBL_UNLIKELY (!pRoot) {
            GUM_LOG_ERROR("No root element found! Create one first.");
            return;
        }

        GUM_Widget* pWidget = GUM_Nav_findSelectableDescendant_(GBL_OBJECT(pRoot), true);
        if (!pWidget)
            pWidget = GUM_Nav_findSelectableDescendant_(GBL_OBJECT(pRoot), false);
        if (pWidget)
            GUM_Nav_focusApply_(pDevice, pWidget);
        return;
    }

    GUM_Widget* pNext = GUM_Nav_moveCursor_(GBL_OBJECT(pFocusedWidget), direction);
    if (!pNext)
        return;

    // Focus/layout callbacks may release or reparent the selected path.
    GblObject* pChildOnPath = GBL_OBJECT(GblBox_ref(GBL_BOX(pNext)));
    if (!GUM_Nav_focusApply_(pDevice, pNext)) {
        GblBox_unref(GBL_BOX(pChildOnPath));
        return;
    }

    for (;;) {
        GblObject* pAncestor = GblObject_parent(pChildOnPath);
        if (!pAncestor || GBL_TYPEOF(pAncestor) == GUM_ROOT_TYPE)
            break;

        GblBox_ref(GBL_BOX(pAncestor));
        GUM_Container* pContainer = GBL_AS(GUM_Container, pAncestor);

        if (pContainer && GUM_Container_scrollable(pContainer)) {
            const GUM_Vector2 nextPos = GUM_get_absolute_position_(pNext);
            const GUM_Rectangle nextRect = { nextPos.x, nextPos.y, pNext->w, pNext->h };

            const GUM_Vector2 containerPos = GUM_get_absolute_position_(GUM_WIDGET(pContainer));
            const GUM_Rectangle containerRect = {
                containerPos.x,
                containerPos.y,
                GUM_WIDGET(pContainer)->w,
                GUM_WIDGET(pContainer)->h
            };

            const GUM_Rectangle overlap = GUM_Rectangle_intersect(containerRect, nextRect);
            const bool fullyVisible = overlap.x      == nextRect.x &&
                                      overlap.y      == nextRect.y &&
                                      overlap.width  == nextRect.width &&
                                      overlap.height == nextRect.height;

            if (!fullyVisible) {
                const float clippedTop    = overlap.y - nextRect.y;
                const float clippedBottom = nextRect.y + nextRect.height - overlap.y - overlap.height;
                const float clippedLeft   = overlap.x - nextRect.x;
                const float clippedRight  = nextRect.x + nextRect.width - overlap.x - overlap.width;
                const bool first = !GblObject_siblingPreviousByCmpFn(pChildOnPath,
                                                                     GUM_Nav_isWidgetType_,
                                                                     nullptr);
                const float margin = GUM_Container_margin(pContainer) * 2.0f;

                switch (direction) {
                    case GUM_INPUTACTION_MOVE_UP:
                        if (first)
                            GUM_Container_scrollTo_(pContainer, GUM_DIRECTION_VERTICAL, 0.0f);
                        else
                            GUM_Container_scrollBy_(pContainer,
                                                    GUM_DIRECTION_VERTICAL,
                                                    -(clippedTop + margin));
                        break;
                    case GUM_INPUTACTION_MOVE_DOWN:
                        GUM_Container_scrollBy_(pContainer,
                                                GUM_DIRECTION_VERTICAL,
                                                clippedBottom + margin);
                        break;
                    case GUM_INPUTACTION_MOVE_LEFT:
                        if (first)
                            GUM_Container_scrollTo_(pContainer, GUM_DIRECTION_HORIZONTAL, 0.0f);
                        else
                            GUM_Container_scrollBy_(pContainer,
                                                    GUM_DIRECTION_HORIZONTAL,
                                                    -(clippedLeft + margin));
                        break;
                    case GUM_INPUTACTION_MOVE_RIGHT:
                        GUM_Container_scrollBy_(pContainer,
                                                GUM_DIRECTION_HORIZONTAL,
                                                clippedRight + margin);
                        break;
                    default:
                        break;
                }
            }
        }

        if (GblObject_parent(pChildOnPath) != pAncestor) {
            GblBox_unref(GBL_BOX(pAncestor));
            break;
        }

        GblBox_unref(GBL_BOX(pChildOnPath));
        pChildOnPath = pAncestor;
    }

    GblBox_unref(GBL_BOX(pChildOnPath));
}

#include <gumball/core/gumball_navigation.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/gumball_elements.h>
#include <gumball/gumball_types.h>

#include "../elements/gumball_root_.h"

static GUM_Widget* GUM_Nav_asSelectable_(GblObject* pObject) {
    if (!pObject || !GBL_TYPECHECK(GUM_Widget, pObject))
        return nullptr;

    GUM_Widget* pWidget = GUM_WIDGET(pObject);

    return pWidget->isSelectable ? pWidget : nullptr;
}

// cmpfn helpers
static GblBool GUM_Nav_isWidgetType_(const GblObject* pObj, void* pClosure) {
    GBL_UNUSED(pClosure);
    return GBL_TYPECHECK(GUM_Widget, pObj);
}

static GblBool GUM_Nav_isContainerType_(const GblObject* pObj, void* pClosure) {
    GBL_UNUSED(pClosure);
    return GBL_TYPECHECK(GUM_Container, pObj);
}

static GblBool GUM_Nav_isSelectable_(const GblObject* pObj, void* pClosure) {
    GBL_UNUSED(pClosure);
    return GBL_TYPECHECK(GUM_Widget, pObj) && GUM_WIDGET(pObj)->isSelectable;
}

///////// hierarchy-aware search /////////
static GblObject* GUM_Nav_siblingStep_(GblObject* pObj, GblObjectCmpFn cmpFn, bool isForwards) {
    return isForwards ? GblObject_siblingNextByCmpFn(pObj, cmpFn, nullptr)
                      : GblObject_siblingPreviousByCmpFn(pObj, cmpFn, nullptr);
}

static GUM_Widget* GUM_Nav_findSelectableSibling_(GblObject* pObj, bool isForwards) {
    GblObject* pSibling = GUM_Nav_siblingStep_(pObj, GUM_Nav_isWidgetType_, isForwards);

    while (pSibling) {
        GUM_Widget* pWidget = GUM_Nav_asSelectable_(pSibling);
        if (pWidget) return pWidget;

        pSibling = GUM_Nav_siblingStep_(pSibling, GUM_Nav_isWidgetType_, isForwards);
    }
    return nullptr;
}

static GUM_Widget* GUM_Nav_findSelectableInContainer_(GblObject* pContainer, size_t preferredIndex, bool isForwards) {
    GUM_Widget* pWidget = nullptr;
    if (preferredIndex != GBL_INDEX_INVALID) {
        pWidget = GUM_Nav_asSelectable_(GblObject_findChildByIndex(pContainer, preferredIndex));
        if (pWidget) return pWidget;
    }

    if (isForwards) {
        GblObject_foreachChild(pContainer, pChild) {
            pWidget = GUM_Nav_asSelectable_(pChild);
            if (pWidget) return pWidget;
        }
    } else {
        GblObject_foreachChildReverse(pContainer, pChild) {
            pWidget = GUM_Nav_asSelectable_(pChild);
            if (pWidget) return pWidget;
        }
    }

    // descendant fallback
    if (!pWidget) {
        GblObject* pObj = GblObject_findDescendantByCmpFn(pContainer, GUM_Nav_isSelectable_, nullptr);
        if (pObj) pWidget = GUM_WIDGET(pObj);
    }

    return pWidget;
}

static GblObject* GUM_Nav_findSiblingContainerWithSelectable_(GblObject* pContainer, bool isForwards) {
    GblObject* pSiblingContainer = GUM_Nav_siblingStep_(pContainer, GUM_Nav_isContainerType_, isForwards);

    while (pSiblingContainer) {
        GUM_Widget* pWidget = GUM_Nav_findSelectableInContainer_(pSiblingContainer, GBL_INDEX_INVALID, isForwards);
        if (pWidget)
            return pSiblingContainer;

        pSiblingContainer = GUM_Nav_siblingStep_(pSiblingContainer, GUM_Nav_isContainerType_, isForwards);
    }

    return nullptr;
}

///////// spatial fallback search /////////

static GUM_Vector2 GUM_Nav_closestPointInRect_(GUM_Vector2 startCenter, GUM_Rectangle targetRect) {
    float min_x = targetRect.x;
    float max_x = targetRect.x + targetRect.width;
    float min_y = targetRect.y;
    float max_y = targetRect.y + targetRect.height;

    return (GUM_Vector2){ .x = GBL_CLAMP(startCenter.x, min_x, max_x),
                          .y = GBL_CLAMP(startCenter.y, min_y, max_y) };
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

    const GUM_Vector2 candPos          = GUM_get_absolute_position_(pCandidate);
    const GUM_Vector2 candSize         = { pCandidate->w, pCandidate->h };
    const GUM_Vector2 candClosestPoint = GUM_Nav_closestPointInRect_(
        pSearch->currentCenter,
        (GUM_Rectangle){ candPos.x, candPos.y, candSize.x, candSize.y });
    const GUM_Vector2 delta = GUM_Vector2_subtract(candClosestPoint, pSearch->currentCenter);

    const float forwardDistance = delta.x * pSearch->cursorDirection.x +
                                  delta.y * pSearch->cursorDirection.y;
    if (forwardDistance <= 0.0f)
        return GBL_FALSE;

    const float angle = GUM_Vector2_signedAngleTo(pSearch->currentCenter,
                                                   candClosestPoint,
                                                   pSearch->cursorDirection);
    if (fabsf(angle) > 0.4f)
        return GBL_FALSE;

    const float score = 10.0f - GUM_Vector2_distance(candClosestPoint, pSearch->currentCenter);
    if (score > pSearch->bestScore) {
        pSearch->bestScore = score;
        pSearch->pBest     = pCandidate;
    }

    return GBL_FALSE;
}

static GUM_Widget* GUM_Nav_findSelectableByPosition_(GUM_Widget* pCurrent, GUM_InputAction direction) {
    const GUM_Vector2 currPos    = GUM_get_absolute_position_(pCurrent);
    const GUM_Vector2 currSize   = { pCurrent->w, pCurrent->h };
    const GUM_Vector2 currCenter = { currPos.x + currSize.x * 0.5f,
                                     currPos.y + currSize.y * 0.5f };

    GUM_Vector2 cursorDirection;
    switch (direction) {
        case GUM_INPUTACTION_MOVE_UP:
            cursorDirection = (GUM_Vector2){ 0.0f, -1.0f };
            break;
        case GUM_INPUTACTION_MOVE_DOWN:
            cursorDirection = (GUM_Vector2){ 0.0f, 1.0f };
            break;
        case GUM_INPUTACTION_MOVE_LEFT:
            cursorDirection = (GUM_Vector2){ -1.0f, 0.0f };
            break;
        case GUM_INPUTACTION_MOVE_RIGHT:
            cursorDirection = (GUM_Vector2){ 1.0f, 0.0f };
            break;
        default:
            return nullptr;
    }

    GUM_NavSpatialSearch_ search = {
        .pCurrent        = pCurrent,
        .currentCenter   = currCenter,
        .cursorDirection = cursorDirection,
        .pBest           = nullptr,
        .bestScore       = -FLT_MAX
    };

    GUM_Root_foreachDrawable_(GUM_Root_active_(), GUM_Nav_spatialCandidate_, &search);
    return search.pBest;
}

static GUM_Widget* GUM_Nav_moveCursor_(GblObject* pSelf, GUM_InputAction direction) {
    GblObject* pParent = GblObject_parent(pSelf);
    if (!pParent || !GBL_TYPECHECK(GUM_Container, pParent))
        return nullptr;

    GUM_Direction parent_direction           = GUM_CONTAINER(pParent)->direction;
    GblObject* pGrandParent                  = GblObject_parent(pParent);
    GUM_Direction grand_parent_direction     = (pGrandParent && GBL_TYPECHECK(GUM_Container, pGrandParent)) ?
                                                GUM_CONTAINER(pGrandParent)->direction : GUM_DIRECTION_NULL;

    GUM_Direction axis    = (direction == GUM_INPUTACTION_MOVE_LEFT || direction == GUM_INPUTACTION_MOVE_RIGHT) ?
                             GUM_DIRECTION_HORIZONTAL : GUM_DIRECTION_VERTICAL;
    const bool isForwards = direction == GUM_INPUTACTION_MOVE_RIGHT ||
                            direction == GUM_INPUTACTION_MOVE_DOWN;

    if (axis != parent_direction && axis != grand_parent_direction)
        return nullptr;

    size_t childIndex = GblObject_childIndex(pSelf);

    // Intra-container movement, if the axis matches this container's direction
    if (axis == parent_direction) {
        GUM_Widget* pSibling = GUM_Nav_findSelectableSibling_(pSelf, isForwards);
        if (pSibling) return pSibling;
    }

    // Inter-container movement
    GblObject* pNewContainerObj = nullptr;

    if (axis == grand_parent_direction)
        pNewContainerObj = GUM_Nav_findSiblingContainerWithSelectable_(pParent, isForwards);

    // Spatial search fallback
    if (!pNewContainerObj)
        return GUM_Nav_findSelectableByPosition_(GUM_WIDGET(pSelf), direction);

    GUM_Direction new_direction = GUM_CONTAINER(pNewContainerObj)->direction;
    size_t preferredIndex       = (new_direction    == parent_direction       &&
                                   parent_direction != grand_parent_direction &&
                                   GblObject_childCount(pNewContainerObj) > childIndex) ?
                                   childIndex : GBL_INDEX_INVALID;

    return GUM_Nav_findSelectableInContainer_(pNewContainerObj, preferredIndex, isForwards);
}

static GUM_Widget* GUM_Nav_findSelectableDescendant_(GblObject* pSelf, bool filterByDefault) {
    if (!pSelf) return nullptr;
    GUM_Widget* pResult = nullptr;

    struct {
        GblArrayList array;
        char         stackData[64 * sizeof(GblObject*)];
    } stack;

    GblArrayList_construct(&stack.array, sizeof(GblObject*), 0, NULL, sizeof(stack));
    GblArrayList_pushBack(&stack.array, &pSelf);

    while (GblArrayList_size(&stack.array)) {
        GblObject* pObject;
        GblArrayList_popFront(&stack.array, &pObject);

        GblObject_foreachChild(pObject, pChild) {
            GUM_Widget* pWidget = GUM_Nav_asSelectable_(pChild);
            if (pWidget && (!filterByDefault || pWidget->isSelectedByDefault)) {
                pResult = pWidget;
                goto done;
            }
            GblArrayList_pushBack(&stack.array, &pChild);
        }
    }

done:
    GblArrayList_destruct(&stack.array);
    return pResult;
}

void GUM_Nav_focus(GUM_InputDevice* pDevice, GUM_Widget* pWidget) {
    if (!pDevice || pDevice->pFocusedWidget == pWidget)
        return;

    if (pDevice->pFocusedWidget) {
        GBL_EMIT(pDevice->pFocusedWidget, "onFocusLost", pDevice);
        pDevice->pFocusedWidget->focusCount--;
    }

    pDevice->pFocusedWidget = pWidget;

    if (pWidget) {
        pWidget->focusCount++;
        GBL_EMIT(pWidget, "onFocusGained", pDevice);
    }
}

void GUM_Nav_move(GUM_InputDevice* pDevice, GUM_InputAction direction) {
    if (!pDevice) return;

    if (direction < GUM_INPUTACTION_MOVE_UP || direction > GUM_INPUTACTION_MOVE_RIGHT)
        return;

    // find default button
    if (!pDevice->pFocusedWidget) {
        GUM_Root* pRoot = GUM_Root_active_();
        if GBL_UNLIKELY (!pRoot) {
            GUM_LOG_ERROR("No root element found! Create one first.");
            return;
        }

        GUM_Widget* pWidget = GUM_Nav_findSelectableDescendant_(GBL_OBJECT(pRoot), true);
        if (!pWidget)
            pWidget = GUM_Nav_findSelectableDescendant_(GBL_OBJECT(pRoot), false);

        if (pWidget)
            GUM_Nav_focus(pDevice, pWidget);
        return;
    }

    GUM_Widget* pNext = GUM_Nav_moveCursor_(GBL_OBJECT(pDevice->pFocusedWidget), direction);
    if (!pNext) return;

    GUM_Nav_focus(pDevice, pNext);

    GblObject* pChildOnPath = GBL_OBJECT(pNext);
    GblObject* pAncestor    = GblObject_parent(pChildOnPath);

    while (pAncestor && GBL_TYPEOF(pAncestor) != GUM_ROOT_TYPE) {
        GUM_Container* pContainer = GBL_AS(GUM_Container, pAncestor);

        if (pContainer && pContainer->scrollable) {
            const GUM_Vector2 nextAbsPos = GUM_get_absolute_position_(pNext);
            const GUM_Rectangle pNextRec = (GUM_Rectangle){ nextAbsPos.x, nextAbsPos.y,
                                                            pNext->w, pNext->h };

            const GUM_Vector2 contAbsPos = GUM_get_absolute_position_(GUM_WIDGET(pContainer));
            const GUM_Rectangle contRec  = (GUM_Rectangle){ contAbsPos.x, contAbsPos.y,
                                                            GUM_WIDGET(pContainer)->w,
                                                            GUM_WIDGET(pContainer)->h };

            const GUM_Rectangle overlap = GUM_Rectangle_intersect(contRec, pNextRec);
            const bool isFullyVisible   = overlap.x      == pNextRec.x     &&
                                          overlap.y      == pNextRec.y     &&
                                          overlap.width  == pNextRec.width &&
                                          overlap.height == pNextRec.height;

            if (!isFullyVisible) {
                const float clippedTop        = overlap.y - pNextRec.y;
                const float clippedBottom     = (pNextRec.y + pNextRec.height) - (overlap.y + overlap.height);
                const float clippedLeft       = overlap.x - pNextRec.x;
                const float clippedRight      = (pNextRec.x + pNextRec.width) - (overlap.x + overlap.width);
                const bool isFirstInContainer = !GblObject_siblingPreviousByCmpFn(pChildOnPath, GUM_Nav_isWidgetType_, nullptr);

                const float margin = pContainer->margin * 2;

                switch (direction) {
                    case GUM_INPUTACTION_MOVE_UP:
                        GUM_Animator_set(&pContainer->scrollAnimatorY,
                                         isFirstInContainer ? 0 : pContainer->scrollAnimatorY.to - (clippedTop + margin));
                        break;
                    case GUM_INPUTACTION_MOVE_DOWN:
                        GUM_Animator_set(&pContainer->scrollAnimatorY,
                                         pContainer->scrollAnimatorY.to + clippedBottom + margin);
                        break;
                    case GUM_INPUTACTION_MOVE_LEFT:
                        GUM_Animator_set(&pContainer->scrollAnimatorX,
                                         isFirstInContainer ? 0 : pContainer->scrollAnimatorX.to - (clippedLeft + margin));
                        break;
                    case GUM_INPUTACTION_MOVE_RIGHT:
                        GUM_Animator_set(&pContainer->scrollAnimatorX,
                                         pContainer->scrollAnimatorX.to + clippedRight + margin);
                        break;
                    default:
                        break;
                }

                GUM_CONTAINER_CLASSOF(pContainer)->pFnUpdateContent(pContainer);
            }
        }

        pChildOnPath = pAncestor;
        pAncestor    = GblObject_parent(pAncestor);
    }
}

#include <gumball/core/gumball_navigation.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/gumball_elements.h>
#include <gumball/gumball_types.h>

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
    if(!pWidget) {
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

static GUM_Widget* GUM_Nav_findSelectableByPosition_(GUM_Widget* pCurrent, GUM_InputAction direction) {
    GUM_Vector2 currPos    = GUM_get_absolute_position_(GUM_WIDGET(pCurrent));
    GUM_Vector2 currSize   = { GUM_WIDGET(pCurrent)->w, GUM_WIDGET(pCurrent)->h };
    GUM_Vector2 currCenter = { currPos.x + currSize.x * 0.5f, currPos.y + currSize.y * 0.5f };

    GUM_Widget* pBest     = nullptr;
    float       bestScore = -FLT_MAX;

    // Iterate over all widgets in draw order
    GblArrayList* pDrawQueue = GUM_drawQueue_get();
    for (size_t i = 0; i < GblArrayList_size(pDrawQueue); ++i) {
        GblObject*  pObject    = *(GblObject**)GblArrayList_at(pDrawQueue, i);
        GUM_Widget* pCandidate = GUM_Nav_asSelectable_(pObject);

        if (!pCandidate || pCandidate == pCurrent)
            continue;

        GUM_Vector2 candPos          = GUM_get_absolute_position_(pCandidate);
        GUM_Vector2 candSize         = { pCandidate->w, pCandidate->h };
        GUM_Vector2 candClosestPoint = GUM_Nav_closestPointInRect_(
            currCenter, (GUM_Rectangle){ candPos.x, candPos.y, candSize.x, candSize.y });

        GUM_Vector2 cursorDir = { 0, 0 };
        GUM_Vector2 delta     = GUM_Vector2_subtract(candClosestPoint, currCenter);
        float       dist      = GUM_Vector2_distance(candClosestPoint, currCenter);

        switch (direction) {
            case GUM_INPUTACTION_MOVE_UP:
                if (delta.y >= 0) continue; // must be above
                cursorDir = (GUM_Vector2){ 0, -1 };
                break;
            case GUM_INPUTACTION_MOVE_DOWN:
                if (delta.y <= 0) continue; // must be below
                cursorDir = (GUM_Vector2){ 0, 1 };
                break;
            case GUM_INPUTACTION_MOVE_LEFT:
                if (delta.x >= 0) continue; // must be left
                cursorDir = (GUM_Vector2){ -1, 0 };
                break;
            case GUM_INPUTACTION_MOVE_RIGHT:
                if (delta.x <= 0) continue; // must be right
                cursorDir = (GUM_Vector2){ 1, 0 };
                break;
            default:
                continue;
        }

        float angle = GUM_Vector2_signedAngleTo(currCenter, candClosestPoint, cursorDir);
        if (fabsf(angle) > 0.4f) continue;

        float score = 10 - dist;
        if (score > bestScore) {
            bestScore = score;
            pBest     = pCandidate;
        }
    }

    return pBest;
}

static GUM_Widget* GUM_Nav_moveCursor_(GblObject* pSelf, GUM_InputAction direction) {
    GblObject* pParent = GblObject_parent(pSelf);
    if (!pParent || !GBL_TYPECHECK(GUM_Container, pParent))
        return nullptr;

    GUM_Direction parent_direction           = GUM_CONTAINER(pParent)->direction;
    GblObject* pGrandParent                  = GblObject_parent(pParent);
    GUM_Direction grand_parent_direction     = (pGrandParent && GBL_TYPECHECK(GUM_Container, pGrandParent)) ?
                                                GUM_CONTAINER(pGrandParent)->direction : GUM_DIRECTION_NULL;

    GUM_Direction axis    = (direction == GUM_INPUTACTION_MOVE_LEFT  || direction == GUM_INPUTACTION_MOVE_RIGHT) ?
                             GUM_DIRECTION_HORIZONTAL : GUM_DIRECTION_VERTICAL;
    const bool isForwards = (direction == GUM_INPUTACTION_MOVE_RIGHT ||
                            direction == GUM_INPUTACTION_MOVE_DOWN);

    if (axis != parent_direction && axis != grand_parent_direction)
        return nullptr;

    size_t childIndex = GblObject_childIndex(pSelf);

    // Intra-container movement, if the axis matches this container's orientation
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
        GUM_Root* pRoot = nullptr;
        GBL_REQUIRE_SCOPE(GUM_Root, &pRoot, "GUM_Root") {
            if GBL_UNLIKELY (!pRoot) {
                GUM_LOG_ERROR("No root element found! Create one first.");
                GBL_SCOPE_EXIT;
            }

            GUM_Widget* pWidget = GUM_Nav_findSelectableDescendant_(GBL_OBJECT(pRoot), true);
            if (!pWidget)
                pWidget = GUM_Nav_findSelectableDescendant_(GBL_OBJECT(pRoot), false);

            if (pWidget)
                GUM_Nav_focus(pDevice, GUM_WIDGET(pWidget));
        }
        return;
    }

    GUM_Widget* pNext = GUM_Nav_moveCursor_(GBL_OBJECT(pDevice->pFocusedWidget), direction);
    if (!pNext) return;

    GUM_Nav_focus(pDevice, GUM_WIDGET(pNext));

    GblObject* pChildOnPath = GBL_OBJECT(pNext);
    GblObject* pAncestor    = GblObject_parent(pChildOnPath);

    while (pAncestor && GBL_TYPEOF(pAncestor) != GUM_ROOT_TYPE) {
        GUM_Container* pContainer = GUM_CONTAINER(pAncestor);

        if (pContainer->scrollable) {
            const GUM_Vector2 nextAbsPos = GUM_get_absolute_position_(pNext);
            const GUM_Rectangle pNextRec = (GUM_Rectangle){ nextAbsPos.x, nextAbsPos.y,
                                                                pNext->w, pNext->h };

            const GUM_Vector2 contAbsPos = GUM_get_absolute_position_(GUM_WIDGET(pContainer));
            const GUM_Rectangle contRec  = (GUM_Rectangle){ contAbsPos.x, contAbsPos.y,
                                                                GUM_WIDGET(pContainer)->w,
                                                                GUM_WIDGET(pContainer)->h };

            const GUM_Rectangle overlap = GUM_Rectangle_intersect(contRec, pNextRec);
            const bool isFullyVisible   =   overlap.x      == pNextRec.x     &&
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
                    pContainer->scrollOffsetTargetY = isFirstInContainer ? 0 :
                                                      pContainer->scrollOffsetTargetY - (clippedTop + margin);
                    break;
                case GUM_INPUTACTION_MOVE_DOWN:
                    pContainer->scrollOffsetTargetY += clippedBottom + margin;
                    break;
                case GUM_INPUTACTION_MOVE_LEFT:
                    pContainer->scrollOffsetTargetX = isFirstInContainer ? 0 :
                                                      pContainer->scrollOffsetTargetX - (clippedLeft + margin);
                    break;
                case GUM_INPUTACTION_MOVE_RIGHT:
                    pContainer->scrollOffsetTargetX += clippedRight + margin;
                    break;
                }

                GUM_CONTAINER_CLASSOF(pContainer)->pFnUpdateContent(pContainer);
            }
        }

        pChildOnPath = pAncestor;
        pAncestor    = GblObject_parent(pAncestor);
    }
}

#include <gumball/core/gumball_navigation.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/gumball_elements.h>
#include <gumball/gumball_types.h>

static GUM_Button* GUM_Nav_asSelectable_(GblObject* pObject) {
    if (!pObject || !GBL_TYPECHECK(GUM_Button, pObject))
        return nullptr;

    GUM_Button* pButton = GUM_BUTTON(pObject);

    return (pButton->isSelectable && GUM_WIDGET(pButton)->shouldUpdate) ? pButton : nullptr;
}

///////// hierarchy-aware search /////////

static GUM_Button* GUM_Nav_findSelectableSibling_(GblObject* pObj, bool next) {
    GblObject* pSibling = next ? GblObject_siblingNextByType(pObj, GUM_BUTTON_TYPE)
                               : GblObject_siblingPreviousByType(pObj, GUM_BUTTON_TYPE);
    while (pSibling) {
        GUM_Button* pButton = GUM_Nav_asSelectable_(pSibling);
        if (pButton) return pButton;

        pSibling = next ? GblObject_siblingNextByType(pSibling, GUM_BUTTON_TYPE)
                        : GblObject_siblingPreviousByType(pSibling, GUM_BUTTON_TYPE);
    }
    return nullptr;
}

static GUM_Button* GUM_Nav_findSelectableInContainer_(GblObject* pContainer, size_t preferredIndex, bool next) {
    if (preferredIndex != GBL_INDEX_INVALID) {
        GUM_Button* pButton = GUM_Nav_asSelectable_(GblObject_findChildByIndex(pContainer, preferredIndex));
        if (pButton) return pButton;
    }

    if (next) {
        GblObject_foreachChild(pContainer, pChild) {
            GUM_Button* pButton = GUM_Nav_asSelectable_(pChild);
            if (pButton) return pButton;
        }
    } else {
        GblObject_foreachChildReverse(pContainer, pChild) {
            GUM_Button* pButton = GUM_Nav_asSelectable_(pChild);
            if (pButton) return pButton;
        }
    }

    return nullptr;
}

static GblObject* GUM_Nav_findSiblingContainerWithSelectable_(GblObject* pContainer, bool next) {
    GblObject* pSibling = next ? GblObject_siblingNextByType(pContainer, GUM_CONTAINER_TYPE)
                               : GblObject_siblingPreviousByType(pContainer, GUM_CONTAINER_TYPE);
    while (pSibling) {
        GblObject* pButtonObj = GblObject_findChildByType(pSibling, GUM_BUTTON_TYPE);

        if (pButtonObj && GUM_Nav_asSelectable_(pButtonObj))
            return pSibling;

        pSibling = next ? GblObject_siblingNextByType(pSibling, GUM_CONTAINER_TYPE)
                        : GblObject_siblingPreviousByType(pSibling, GUM_CONTAINER_TYPE);
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

static GUM_Button* GUM_Nav_findSelectableByPosition_(GUM_Button* pCurrent, GUM_InputAction direction) {
    GUM_Vector2 currPos    = GUM_get_absolute_position_(GUM_WIDGET(pCurrent));
    GUM_Vector2 currSize   = { GUM_WIDGET(pCurrent)->w, GUM_WIDGET(pCurrent)->h };
    GUM_Vector2 currCenter = { currPos.x + currSize.x * 0.5f, currPos.y + currSize.y * 0.5f };

    GUM_Button* pBest     = nullptr;
    float       bestScore = -FLT_MAX;

    // Iterate over all widgets in draw order
    GblArrayList* pDrawQueue = GUM_drawQueue_get();
    for (size_t i = 0; i < GblArrayList_size(pDrawQueue); ++i) {
        GblObject*  pObject    = *(GblObject**)GblArrayList_at(pDrawQueue, i);
        GUM_Button* pCandidate = GUM_Nav_asSelectable_(pObject);

        if (!pCandidate || pCandidate == pCurrent)
            continue;

        GUM_Vector2 candPos          = GUM_get_absolute_position_(GUM_WIDGET(pCandidate));
        GUM_Vector2 candSize         = { GUM_WIDGET(pCandidate)->w, GUM_WIDGET(pCandidate)->h };
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

static GUM_Button* GUM_Nav_moveCursor_(GblObject* pSelf, GUM_InputAction direction) {
    GblObject* pParent = GblObject_parent(pSelf);
    if (!pParent || !GBL_TYPECHECK(GUM_Container, pParent))
        return nullptr;

    const char parent_orientation       = tolower(GUM_CONTAINER(pParent)->orientation);
    GblObject* pGrandParent             = GblObject_parent(pParent);
    const char grand_parent_orientation = (pGrandParent && GBL_TYPECHECK(GUM_Container, pGrandParent))
                                              ? tolower(GUM_CONTAINER(pGrandParent)->orientation)
                                              : 'N';

    const char axis = (direction == GUM_INPUTACTION_MOVE_LEFT ||
                       direction == GUM_INPUTACTION_MOVE_RIGHT) ? 'h' : 'v';
    const bool next  = (direction == GUM_INPUTACTION_MOVE_RIGHT ||
                        direction == GUM_INPUTACTION_MOVE_DOWN);

    if (axis != parent_orientation && axis != grand_parent_orientation)
        return nullptr;

    size_t childIndex = GblObject_childIndex(pSelf);

    // Intra-container movement, if the axis matches this container's orientation
    if (axis == parent_orientation) {
        GUM_Button* pSibling = GUM_Nav_findSelectableSibling_(pSelf, next);
        if (pSibling) return pSibling;
    }

    // Inter-container movement
    GblObject* pNewContainer = nullptr;

    if (axis == grand_parent_orientation)
        pNewContainer = GUM_Nav_findSiblingContainerWithSelectable_(pParent, next);

    if (!pNewContainer)
        return GUM_Nav_findSelectableByPosition_(GUM_BUTTON(pSelf), direction);

    char   new_orientation = tolower(GUM_CONTAINER(pNewContainer)->orientation);
    size_t preferredIndex  = (new_orientation == parent_orientation &&
                              parent_orientation != grand_parent_orientation &&
                              GblObject_childCount(pNewContainer) > childIndex)
                                 ? childIndex
                                 : GBL_INDEX_INVALID;

    return GUM_Nav_findSelectableInContainer_(pNewContainer, preferredIndex, next);
}

static GUM_Button* GUM_Nav_findSelectableDescendant_(GblObject* pSelf, bool filterByDefault) {
    if (!pSelf) return nullptr;
    GUM_Button* pResult = nullptr;

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
            GUM_Button* pButton = GUM_Nav_asSelectable_(pChild);
            if (pButton && (!filterByDefault || pButton->isSelectedByDefault)) {
                pResult = pButton;
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

    if (!pDevice->pFocusedWidget) {
        GUM_Root* pRoot = nullptr;
        GBL_REQUIRE_SCOPE(GUM_Root, &pRoot, "GUM_Root") {
            if GBL_UNLIKELY (!pRoot) {
                GUM_LOG_ERROR("No root element found! Create one first.");
                GBL_SCOPE_EXIT;
            }

            GUM_Button* pButton = GUM_Nav_findSelectableDescendant_(GBL_OBJECT(pRoot), true);
            if (!pButton)
                pButton = GUM_Nav_findSelectableDescendant_(GBL_OBJECT(pRoot), false);

            if (pButton)
                GUM_Nav_focus(pDevice, GUM_WIDGET(pButton));
        }
        return;
    }

    GUM_Button* pNext = GUM_Nav_moveCursor_(GBL_OBJECT(pDevice->pFocusedWidget), direction);
    if (pNext)
        GUM_Nav_focus(pDevice, GUM_WIDGET(pNext));
}

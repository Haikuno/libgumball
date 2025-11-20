#include <gumball/elements/gumball_controller.h>
#include <gumball/elements/gumball_container.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/core/gumball_backend.h>


#include <gimbal/gimbal_containers.h>

static GblObject *GblObject_findDescendantByType(GblObject *pSelf, GblType descendantType) {
    GblObject *result = nullptr;
    if (!pSelf) goto done;

    GblArrayDeque queue;
    GblArrayDeque_construct(&queue, sizeof(GblObject*), 16);
    GblArrayDeque_pushBack(&queue, &pSelf);

    while (GblArrayDeque_size(&queue)) {
        GblObject **ppObj = GblArrayDeque_popFront(&queue);
        GblObject *pObj = *ppObj;

        size_t childCount = GblObject_childCount(pObj);
        for (size_t i = 0; i < childCount; i++) {
            GblObject *childObj = GblObject_findChildByIndex(pObj, i);
            if (GBL_TYPEOF(childObj) == descendantType) {
                result = childObj;
                goto done;
            }
            GblArrayDeque_pushBack(&queue, &childObj);
        }
    }

    done:
    GblArrayDeque_destruct(&queue);
    return result;
}


static GUM_Button *findDefaultSelectableInContainer_(GUM_Container *pContainer) {
    GUM_Button *pButton = GBL_AS(GUM_Button, GblObject_findDescendantByType(GBL_OBJECT(pContainer), GUM_BUTTON_TYPE));
    while (pButton) {
        if (pButton->isSelectedByDefault) return pButton;
        pButton = GBL_AS(GUM_Button, GblObject_siblingNextByType(GBL_OBJECT(pButton), GUM_BUTTON_TYPE));
    }
    return nullptr;
}

static GBL_RESULT GUM_Controller_init_(GblInstance *pInstance) {
    GUM_Controller *pController = GUM_CONTROLLER(pInstance);
    GUM_Widget     *pWidget     = GUM_WIDGET(pInstance);

    pController->controllerId   = 0;
    pController->isKeyboard     = false;

	pWidget->r 		 = 0;
	pWidget->g 		 = 0;
	pWidget->b 		 = 0;
	pWidget->a 		 = 255;
	pWidget->z_index = 200;

	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Controller_GblObject_setProperty_(GblObject *pObject, const GblProperty *pProp, GblVariant *pValue) {
	GUM_Controller *pSelf = GUM_CONTROLLER(pObject);

	switch (pProp->id) {
		case GUM_Controller_Property_Id_controllerId:
			GblVariant_valueCopy(pValue, &pSelf->controllerId);
			break;
		case GUM_Controller_Property_Id_isKeyboard:
			GblVariant_valueCopy(pValue, &pSelf->isKeyboard);
			break;
		default:
			return GBL_RESULT_ERROR_INVALID_PROPERTY;
	}

	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Controller_GblObject_property_(const GblObject *pObject, const GblProperty *pProp, GblVariant *pValue) {
	GUM_Controller *pSelf = GUM_CONTROLLER(pObject);

	switch (pProp->id) {
		case GUM_Controller_Property_Id_controllerId:
			GblVariant_setUint8(pValue, pSelf->controllerId);
			break;
		case GUM_Controller_Property_Id_isKeyboard:
			GblVariant_setBool(pValue, pSelf->isKeyboard);
			break;
		default:
			return GBL_RESULT_ERROR_INVALID_PROPERTY;
	}

	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Controller_update_(GUM_Widget *pSelf) {
    // find first selected by default button
    if (!GUM_CONTROLLER(pSelf)->pSelectedButton) {
        auto root = GBL_REQUIRE(GUM_Root, "GUM_Root");

        GUM_Container *pContainer = GBL_AS(GUM_Container, GblObject_findDescendantByType(GBL_OBJECT(root), GUM_CONTAINER_TYPE));
        if(!pContainer) return GBL_RESULT_ERROR;
        GUM_Button    *pButton    = findDefaultSelectableInContainer_(pContainer);

        while (pContainer) {
            if (pButton) {
                GUM_CONTROLLER(pSelf)->pSelectedButton = pButton;
                return GBL_RESULT_SUCCESS;
            }

            auto pNextContainer = GBL_AS(GUM_Container, GblObject_siblingNextByType(GBL_OBJECT(pContainer), GUM_CONTAINER_TYPE));
            if (!pNextContainer) {
                pNextContainer = GBL_AS(GUM_Container, GblObject_findDescendantByType(GBL_OBJECT(pContainer), GUM_CONTAINER_TYPE));
                if (!pNextContainer) break;
            }
            pContainer = pNextContainer;
            pButton    = findDefaultSelectableInContainer_(pContainer);
        }
    }

    GUM_Backend_pollInput(GUM_CONTROLLER(pSelf));

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Controller_draw_(GUM_Widget *pSelf, GUM_Renderer *pRenderer) {
	GUM_Controller *pController = GUM_CONTROLLER(pSelf);

	if (pController->pSelectedButton) {
		GUM_Rectangle rec = {
			.x = GUM_WIDGET(pController->pSelectedButton)->x,
			.y = GUM_WIDGET(pController->pSelectedButton)->y,
			.width  = GUM_WIDGET(pController->pSelectedButton)->w,
			.height = GUM_WIDGET(pController->pSelectedButton)->h
		};

        GUM_Backend_rectangleLinesDraw(pRenderer, rec, pSelf->border_radius, pSelf->border_width * 0.75, (GUM_Color){ pSelf->r, pSelf->g, pSelf->b, pSelf->a });
	}

	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ControllerClass_init_(GblClass *pClass, const void *pData) {
	GBL_UNUSED(pData);

	if (!GblType_classRefCount(GUM_CONTROLLER_TYPE)) GBL_PROPERTIES_REGISTER(GUM_Controller);

	GBL_OBJECT_CLASS(pClass)->pFnSetProperty 	= GUM_Controller_GblObject_setProperty_;
	GBL_OBJECT_CLASS(pClass)->pFnProperty    	= GUM_Controller_GblObject_property_;


    GUM_WIDGET_CLASS(pClass)->pFnDraw       	= GUM_Controller_draw_;
	GUM_WIDGET_CLASS(pClass)->pFnUpdate			= GUM_Controller_update_;

	return GBL_RESULT_SUCCESS;
}

GblType GUM_Controller_type(void) {
	static GblType type = GBL_INVALID_TYPE;

	if (type == GBL_INVALID_TYPE) {
		type =
			GblType_register(GblQuark_internStatic("GUM_Controller"),
							 GUM_WIDGET_TYPE,
							 &(static GblTypeInfo){.classSize	    = sizeof(GUM_ControllerClass),
												   .pFnClassInit    = GUM_ControllerClass_init_,
												   .instanceSize    = sizeof(GUM_Controller),
												   .pFnInstanceInit = GUM_Controller_init_},
							 GBL_TYPE_FLAG_TYPEINFO_STATIC);
	}

	return type;
}

static GUM_Button* findSelectableSibling_(GblObject* pObj, bool next) {
    GblObject* sibling = next  ? GblObject_siblingNextByType(pObj, GUM_BUTTON_TYPE)
                               : GblObject_siblingPreviousByType(pObj, GUM_BUTTON_TYPE);
    while (sibling) {
        GUM_Button* button = GBL_AS(GUM_Button, sibling);
        if (button && button->isSelectable) return button;
        sibling = next ? GblObject_siblingNextByType(sibling, GUM_BUTTON_TYPE)
                       : GblObject_siblingPreviousByType(sibling, GUM_BUTTON_TYPE);
    }
    return nullptr;
}

static GblObject* findSiblingContainerWithButton_(GblObject* pContainer, bool next) {
    GblObject* sibling = next  ? GblObject_siblingNextByType(pContainer, GUM_CONTAINER_TYPE)
                               : GblObject_siblingPreviousByType(pContainer, GUM_CONTAINER_TYPE);
    while (sibling) {
        if (GblObject_findChildByType(sibling, GUM_BUTTON_TYPE)) return sibling;
        sibling = next ? GblObject_siblingNextByType(sibling, GUM_CONTAINER_TYPE)
                       : GblObject_siblingPreviousByType(sibling, GUM_CONTAINER_TYPE);
    }
    return nullptr;
}

static GUM_Button* findSelectableInContainer_(GblObject* pContainer, bool last, size_t preferredIndex) {
    GblObject* obj = GblObject_findDescendantByType(pContainer, GUM_BUTTON_TYPE);
    if (!obj) return nullptr;

    GUM_Button* candidate = nullptr;
    size_t index = 0;

    while (obj) {
        GUM_Button* button = GBL_AS(GUM_Button, obj);
        if (button && button->isSelectable) {
            if (preferredIndex != GBL_INDEX_INVALID && index == preferredIndex) return button;
            candidate = button;
        }
        obj = GblObject_siblingNextByType(obj, GUM_BUTTON_TYPE);
        index++;
    }

    // If no preferred index match, return first (if !last) or last (if last) selectable
    if (last && candidate) return candidate;
    if (!last) {
        obj = GblObject_findDescendantByType(pContainer, GUM_BUTTON_TYPE);
        while (obj) {
            GUM_Button* button = GBL_AS(GUM_Button, obj);
            if (button && button->isSelectable) return button;
            obj = GblObject_siblingNextByType(obj, GUM_BUTTON_TYPE);
        }
    }
    return nullptr;
}

static GUM_Vector2 vector2_FindClosestPoint_(GUM_Vector2 startCenter, GUM_Rectangle targetRect) {
    float min_x = targetRect.x;
    float max_x = targetRect.x + targetRect.width;
    float min_y = targetRect.y;
    float max_y = targetRect.y + targetRect.height;

    GUM_Vector2 closestPoint = {
        .x = GBL_CLAMP(startCenter.x, min_x, max_x),
        .y = GBL_CLAMP(startCenter.y, min_y, max_y)
    };

    return closestPoint;
}

static GUM_Button *findSelectableByPosition_(GUM_Button* pCurrent, GUM_CONTROLLER_BUTTON_ID direction) {
    GUM_Root *pRoot = GBL_REQUIRE(GUM_Root, "GUM_Root");
    if GBL_UNLIKELY(!pRoot) return nullptr;

    GUM_Vector2 currPos    =   GUM_get_absolute_position_(GUM_WIDGET(pCurrent));
    GUM_Vector2 currSize   =  {GUM_WIDGET(pCurrent)->w, GUM_WIDGET(pCurrent)->h};
    GUM_Vector2 currCenter = {
        currPos.x + currSize.x * 0.5f,
        currPos.y + currSize.y * 0.5f
    };

    GUM_Button* pBest      = nullptr;
    float      bestScore  = -FLT_MAX;

    // Iterate over all widgets in draw order
    GblArrayList *drawQueue = GUM_drawQueue_get();
    for (size_t i = 0; i < GblArrayList_size(drawQueue); ++i) {
        GblObject **ppObj      = GblArrayList_at(drawQueue, i);
        GUM_Button  *pCandidate = GBL_AS(GUM_Button, *ppObj);

        if (!pCandidate               ||
             pCandidate == pCurrent   ||
            !pCandidate->isSelectable) {
            continue;
        }

        GUM_Vector2 candPos          =   GUM_get_absolute_position_(GUM_WIDGET(pCandidate));
        GUM_Vector2 candSize         =  {GUM_WIDGET(pCandidate)->w, GUM_WIDGET(pCandidate)->h};
        GUM_Vector2 candClosestPoint =   vector2_FindClosestPoint_(currCenter, (GUM_Rectangle){candPos.x, candPos.y,
                                                                                       candSize.x, candSize.y});

        GUM_Vector2 cursorDir        = {0,0};
        GUM_Vector2 delta            = GUM_Vector2_subtract(candClosestPoint, currCenter);
        float   dist                 = GUM_Vector2_distance(candClosestPoint, currCenter);
        float   angle                = GUM_Vector2_angle(cursorDir, delta);

        switch (direction) {
            case GUM_CONTROLLER_UP:
                if (delta.y >= 0) continue; // must be above
                cursorDir = (GUM_Vector2){0, -1};
                break;
            case GUM_CONTROLLER_DOWN:
                if (delta.y <= 0) continue; // must be below
                cursorDir = (GUM_Vector2){0, 1};
                break;
            case GUM_CONTROLLER_LEFT:
                if (delta.x >= 0) continue; // must be left
                cursorDir = (GUM_Vector2){-1, 0};
                break;
            case GUM_CONTROLLER_RIGHT:
                if (delta.x <= 0) continue; // must be right
                cursorDir = (GUM_Vector2){1, 0};
                break;
            default:
                continue;
        }


        if(fabsf(angle) > 0.4f) continue;

        float score = 10.0f - dist;

        if (score > bestScore) {
            bestScore = score;
            pBest = pCandidate;
        }
    }

    return pBest;
}

static GUM_Button* moveCursor_(GblObject* pSelf, GUM_CONTROLLER_BUTTON_ID buttonPress) {
    GblObject* pParent = GblObject_parent(pSelf);
    if (!pParent || !GBL_AS(GUM_Container, pParent)) return nullptr;

    char parent_orientation = tolower(GBL_AS(GUM_Container, pParent)->orientation);
    GblObject* pGrandParent = GblObject_parent(pParent);
    char grand_parent_orientation = (pGrandParent && GBL_AS(GUM_Container, pGrandParent)) ?
                                    tolower(GBL_AS(GUM_Container, pGrandParent)->orientation) : 'N';

    char axis = (buttonPress == GUM_CONTROLLER_LEFT  || buttonPress == GUM_CONTROLLER_RIGHT) ? 'h' : 'v';
    bool next = (buttonPress == GUM_CONTROLLER_RIGHT || buttonPress == GUM_CONTROLLER_DOWN);

    if (axis != parent_orientation && axis != grand_parent_orientation) return nullptr;

    size_t childIndex = GblObject_childIndex(pSelf);

    // Intra-container movement if axis matches parent
    if (axis == parent_orientation) {
        GUM_Button* sibling = findSelectableSibling_(pSelf, next);
        if (sibling) return sibling;
    }

    // Inter-container movement

    GblObject* pNewContainer = nullptr;

    // If axis matches grandparent's orientation, find sibling container
    if (axis == grand_parent_orientation) {
        pNewContainer = findSiblingContainerWithButton_(pParent, next);
    }

    // Fallback to searching by position
    if (!pNewContainer) {
        return findSelectableByPosition_(GUM_BUTTON(pSelf), buttonPress);
    }

    if (!pNewContainer) return nullptr;

    char new_orientation  = tolower(GBL_AS(GUM_Container, pNewContainer)->orientation);
    size_t preferredIndex = (new_orientation == parent_orientation && GblObject_childCount(pNewContainer) > childIndex)
                            ? childIndex : GBL_INDEX_INVALID;

    // If orientations match at parent/grandparent level, use first/last, else try preferred index or fallback
    bool useLast = !next;
    if (parent_orientation == grand_parent_orientation) useLast = !next;
    return findSelectableInContainer_(pNewContainer, useLast, preferredIndex);
}

static GBL_RESULT GUM_Controller_handleButton_(GUM_Controller *pSelf, GUM_CONTROLLER_BUTTON_STATE eventState, GUM_CONTROLLER_BUTTON_ID eventButton) {
    GUM_Button		**ppButton		= &pSelf->pSelectedButton;
    auto            root            = GBL_REQUIRE(GUM_Root, "GUM_Root");

    const bool isDirection         =    eventButton == GUM_CONTROLLER_UP   ||
                                        eventButton == GUM_CONTROLLER_DOWN ||
                                        eventButton == GUM_CONTROLLER_LEFT ||
                                        eventButton == GUM_CONTROLLER_RIGHT;

    const bool isPress             =    eventState  == GUM_CONTROLLER_BUTTON_PRESS;

    const bool isDirectionPress    =    isDirection && isPress;

    if (isDirectionPress) {
        GUM_Button *newButton = nullptr;
        if (!(*ppButton)) {
            newButton = GBL_AS(GUM_Button, GblObject_findDescendantByType(GBL_OBJECT(root), GUM_BUTTON_TYPE));
            if (!newButton) return GBL_RESULT_ERROR;
            *ppButton = newButton;
            (*ppButton)->isSelected = true;
            return GBL_RESULT_SUCCESS;
        }

        newButton = moveCursor_(GBL_OBJECT(*ppButton), eventButton);

        if (newButton) {
            (*ppButton)->isSelected = false;
            *ppButton = newButton;
            (*ppButton)->isSelected = true;
        }
    }

    if (*ppButton && isPress && !isDirection) {
        switch (eventButton) {
            case GUM_CONTROLLER_PRIMARY:
                GBL_EMIT(*ppButton, "onPressPrimary");
                break;
            case GUM_CONTROLLER_SECONDARY:
                GBL_EMIT(*ppButton, "onPressSecondary");
                break;
            case GUM_CONTROLLER_TERTIARY:
                GBL_EMIT(*ppButton, "onPressTertiary");
                break;
            default:
                break;
        }
    }

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT void GUM_Controller_sendButton(GUM_Controller *pSelf, GUM_CONTROLLER_BUTTON_STATE state, GUM_CONTROLLER_BUTTON_ID button) {
    GUM_Controller_handleButton_(pSelf, state, button);
}

GBL_EXPORT void GUM_Controller_setSelectedButton(GUM_Controller *pSelf, GUM_Button* pButton) {
	GUM_CONTROLLER(pSelf)->pSelectedButton = pButton;
}

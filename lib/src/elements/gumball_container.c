#include "gumball/core/gumball_backend.h"
#include <gumball/elements/gumball_container.h>

// TODO: only resize and realign widgets on signal firing instead of every frame

static GBL_RESULT GUM_Container_init_(GblInstance *pInstance) {
    GUM_CONTAINER(pInstance)->padding        = 5.0f;
    GUM_CONTAINER(pInstance)->margin         = 2.0f;
    GUM_CONTAINER(pInstance)->minChildSize   = 0.15f;
    GUM_CONTAINER(pInstance)->orientation    = 'v';
    GUM_CONTAINER(pInstance)->resizeWidgets  = true;
    GUM_CONTAINER(pInstance)->alignWidgets   = true;
    GUM_CONTAINER(pInstance)->scrollable     = true;

    GUM_CONTAINER(pInstance)->scrollOffsetX = 0;
    GUM_CONTAINER(pInstance)->scrollOffsetY = 0;
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Container_GblObject_setProperty_(GblObject *pObject, const GblProperty *pProp, GblVariant *pValue) {
    GUM_Container *pSelf = GUM_CONTAINER(pObject);

    switch (pProp->id) {
        case GUM_Container_Property_Id_padding:
            GblVariant_valueCopy(pValue, &pSelf->padding);
            break;
        case GUM_Container_Property_Id_margin:
            GblVariant_valueCopy(pValue, &pSelf->margin);
            break;
        case GUM_Container_Property_Id_minChildSize:
            GblVariant_valueCopy(pValue, &pSelf->minChildSize);
            break;
        case GUM_Container_Property_Id_orientation:
            GblVariant_valueCopy(pValue, &pSelf->orientation);
            break;
        case GUM_Container_Property_Id_resizeWidgets:
            GblVariant_valueCopy(pValue, &pSelf->resizeWidgets);
            break;
        case GUM_Container_Property_Id_alignWidgets:
            GblVariant_valueCopy(pValue, &pSelf->alignWidgets);
            break;
        case GUM_Container_Property_Id_scrollable:
            GblVariant_valueCopy(pValue, &pSelf->scrollable);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Container_GblObject_property_(const GblObject *pObject, const GblProperty *pProp, GblVariant *pValue) {
    GUM_Container *pSelf = GUM_CONTAINER(pObject);

    switch (pProp->id) {
        case GUM_Container_Property_Id_padding:
            GblVariant_setFloat(pValue, pSelf->padding);
            break;
        case GUM_Container_Property_Id_margin:
            GblVariant_setFloat(pValue, pSelf->margin);
            break;
        case GUM_Container_Property_Id_minChildSize:
            GblVariant_setFloat(pValue, pSelf->minChildSize);
            break;
        case GUM_Container_Property_Id_orientation:
            GblVariant_setChar(pValue, pSelf->orientation);
            break;
        case GUM_Container_Property_Id_resizeWidgets:
            GblVariant_setBool(pValue, pSelf->resizeWidgets);
            break;
        case GUM_Container_Property_Id_alignWidgets:
            GblVariant_setBool(pValue, pSelf->alignWidgets);
            break;
        case GUM_Container_Property_Id_scrollable:
            GblVariant_setBool(pValue, pSelf->scrollable);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Container_updateContent_(GUM_Container *pSelf) {
    size_t      childCount  = GblObject_childCount(GBL_OBJECT(pSelf));
    GUM_Widget *pSelfWidget = GUM_WIDGET(pSelf);

    if GBL_UNLIKELY(childCount == 0) return GBL_RESULT_SUCCESS;

	const bool 	isHorizontal 				= pSelf->orientation == 'h' || pSelf->orientation == 'H';
    const float totalMargin  				= pSelf->margin * 2.0f * (float)childCount;
    const float totalPadding 				= pSelf->padding * 2.0f;

    const float container_mainPos 			= isHorizontal ? pSelfWidget->x : pSelfWidget->y;
	const float container_secondaryPos		= isHorizontal ? pSelfWidget->y : pSelfWidget->x;
	const float container_mainDim			= isHorizontal ? pSelfWidget->w : pSelfWidget->h;
	const float container_secondaryDim		= isHorizontal ? pSelfWidget->h : pSelfWidget->w;

	const float cornerRadius   = pSelfWidget->border_radius * GBL_MIN(container_mainDim, container_secondaryDim) * 0.5f;
	const float roundnessInset = cornerRadius * 0.293f; // 0.293f = 1 - 1/sqrt(2)

	const float totalPaddingWithRoundness 	= totalPadding + roundnessInset * 2.0f;
	float offset 							= container_mainPos + pSelf->padding + pSelf->margin + roundnessInset;

	for (size_t i = 0; i < childCount; ++i) {
		GblObject     *child_obj    = GblObject_findChildByIndex(GBL_OBJECT(pSelf), i);
		GUM_Widget    *child_widget = GBL_AS(GUM_Widget, child_obj);

        if GBL_UNLIKELY(!child_widget) continue;

        if (pSelf->scrollable) {
            child_widget->x += pSelf->scrollOffsetX;
            child_widget->y += pSelf->scrollOffsetY;
        }

		float *widget_mainPos		= isHorizontal ? &child_widget->x : &child_widget->y;
		float *widget_secondaryPos  = isHorizontal ? &child_widget->y : &child_widget->x;
		float *widget_mainDim		= isHorizontal ? &child_widget->w : &child_widget->h;
		float *widget_secondaryDim  = isHorizontal ? &child_widget->h : &child_widget->w;

		if (pSelf->resizeWidgets) {
			*widget_mainDim		    = (container_mainDim - totalMargin - totalPaddingWithRoundness) / (float)childCount;
			*widget_mainDim         = GBL_MAX(container_mainDim * 0.15, *widget_mainDim);
			*widget_secondaryDim	= container_secondaryDim - totalPaddingWithRoundness;
		}

		if (pSelf->alignWidgets) {
			*widget_mainPos		        = offset;
			if (offset + *widget_mainDim > container_mainPos + container_mainDim) {
			    child_widget->r = 50;
			    child_widget->g = 50;
                child_widget->b = 50;
			}
			const float availableSecDim = container_secondaryDim - totalPaddingWithRoundness;
            *widget_secondaryPos        = container_secondaryPos + pSelf->padding + roundnessInset + (availableSecDim - *widget_secondaryDim) / 2.0f;
			offset                     += *widget_mainDim + pSelf->margin * 2.0f;
		}
	}

    // second pass to notify child containers

    for (size_t i = 0; i < childCount; ++i) {
        GblObject *child_obj = GblObject_findChildByIndex(GBL_OBJECT(pSelf), i);
        GUM_Container *child_container = GBL_AS(GUM_Container, child_obj);
        if (child_container) {
            GUM_Container_updateContent_(child_container);
        }
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Container_Object_instantiated_(GblObject *pObject) {
    GBL_CTX_BEGIN(nullptr);
    GBL_VCALL_DEFAULT(GUM_Widget, base.pFnInstantiated, pObject);
    GUM_CONTAINER_CLASSOF(pObject)->pFnUpdateContent(GUM_CONTAINER(pObject));
    GBL_CTX_END();
}

static GBL_RESULT GUM_Container_draw_(GUM_Widget *pSelf, GUM_Renderer *pRenderer) {
    GUM_Container *pContainer = GUM_CONTAINER(pSelf);

    // Draw container background first (call parent draw)
    GBL_CTX_BEGIN(nullptr);
    GBL_VCALL_DEFAULT(GUM_Widget, pFnDraw, pSelf, pRenderer);

    // If scrollable, enable clipping
    if (pContainer->scrollable) {
        GUM_Rectangle clipRect = {
            .x = pSelf->x + pContainer->padding,
            .y = pSelf->y + pContainer->padding,
            .width = pSelf->w - pContainer->padding * 2,
            .height = pSelf->h - pContainer->padding * 2
        };
        // GUM_Backend_beginScissor(pRenderer, clipRect);
        // GUM_Backend_rectangleDraw(nullptr, clipRect, 0, (GUM_Color){255,0,0,255});
    }

    GBL_CTX_END();

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_ContainerClass_init_(GblClass *pClass, const void *pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_CONTAINER_TYPE)) GBL_PROPERTIES_REGISTER(GUM_Container);

    GBL_OBJECT_CLASS(pClass)->pFnSetProperty  = GUM_Container_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty     = GUM_Container_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)->pFnInstantiated = GUM_Container_Object_instantiated_;

    GUM_WIDGET_CLASS(pClass)->pFnDraw             = GUM_Container_draw_;
    GUM_CONTAINER_CLASS(pClass)->pFnUpdateContent = GUM_Container_updateContent_;

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Container_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Container"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo) {.classSize = sizeof(GUM_ContainerClass),
                                                       .pFnClassInit = GUM_ContainerClass_init_,
                                                       .instanceSize = sizeof(GUM_Container),
                                                       .pFnInstanceInit = GUM_Container_init_},
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

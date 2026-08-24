#include <gumball/core/gumball_backend.h>
#include <gumball/elements/gumball_container.h>
#include <gumball/core/gumball_logger.h>

#include <gimbal/gimbal_containers.h>

#include "gumball_container_.h"
#include "gumball_widget_.h"

typedef struct GUM_ContainerLayoutSnapshot_ {
    GblArrayList    containers;
    GUM_Container* stack[GUM_SNAPSHOT_INLINE_CAPACITY];
} GUM_ContainerLayoutSnapshot_;

static void GUM_Container_layoutSnapshotRelease_(GUM_ContainerLayoutSnapshot_* pSnapshot) {
    const size_t count = GblArrayList_size(&pSnapshot->containers);
    for (size_t i = 0; i < count; ++i) {
        GUM_Container* pContainer = *(GUM_Container**)GblArrayList_at(&pSnapshot->containers, i);
        GblBox_unref(GBL_BOX(pContainer));
    }
    GblArrayList_destruct(&pSnapshot->containers);
}

static GBL_RESULT GUM_Container_layoutSnapshot_(GUM_Container* pSelf,
                                                GUM_ContainerLayoutSnapshot_* pSnapshot) {
    GBL_RESULT result = GblArrayList_construct(&pSnapshot->containers,
                                               sizeof(GUM_Container*),
                                               0,
                                               nullptr,
                                               sizeof(*pSnapshot));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    GblObject_foreachChild(GBL_OBJECT(pSelf), pChild) {
        GUM_Container* pContainer = GBL_AS(GUM_Container, pChild);
        if (!pContainer)
            continue;

        GUM_Container* pRetained = GUM_CONTAINER(GblBox_ref(GBL_BOX(pContainer)));
        result = GblArrayList_pushBack(&pSnapshot->containers, &pRetained);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            GblBox_unref(GBL_BOX(pRetained));
            GUM_Container_layoutSnapshotRelease_(pSnapshot);
            return result;
        }
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Container_init_(GblInstance* pInstance) {
    GUM_Container_* pSelf_ = GUM_CONTAINER_(GUM_CONTAINER(pInstance));

    pSelf_->padding         = 5.0f;
    pSelf_->margin          = 2.0f;
    pSelf_->minChildSize    = 0.15f;
    pSelf_->scrollLimitX    = 0.0f;
    pSelf_->scrollLimitY    = 0.0f;
    pSelf_->direction       = GUM_DIRECTION_VERTICAL;
    pSelf_->resizeWidgets   = true;
    pSelf_->alignWidgets    = true;
    pSelf_->scrollable      = true;
    pSelf_->scrollAnimatorX = GUM_Animator_make(0.0f, 0.2f, GUM_EASE_QUAD_OUT);
    pSelf_->scrollAnimatorY = GUM_Animator_make(0.0f, 0.2f, GUM_EASE_QUAD_OUT);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT float GUM_Container_padding(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->padding : 0.0f;
}

GBL_EXPORT float GUM_Container_margin(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->margin : 0.0f;
}

GBL_EXPORT float GUM_Container_minChildSize(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->minChildSize : 0.0f;
}

GBL_EXPORT GUM_Direction GUM_Container_direction(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->direction : GUM_DIRECTION_NULL;
}

GBL_EXPORT GblBool GUM_Container_resizeWidgets(const GUM_Container* pSelf) {
    return pSelf && GUM_CONTAINER_(pSelf)->resizeWidgets;
}

GBL_EXPORT GblBool GUM_Container_alignWidgets(const GUM_Container* pSelf) {
    return pSelf && GUM_CONTAINER_(pSelf)->alignWidgets;
}

GBL_EXPORT GblBool GUM_Container_scrollable(const GUM_Container* pSelf) {
    return pSelf && GUM_CONTAINER_(pSelf)->scrollable;
}

static GBL_RESULT GUM_Container_GblObject_setProperty_(GblObject* pObject,
                                                       const GblProperty* pProp,
                                                       GblVariant* pValue) {
    GUM_Container*  pSelf  = GUM_CONTAINER(pObject);
    GUM_Container_* pSelf_ = GUM_CONTAINER_(pSelf);

    switch (pProp->id) {
        case GUM_Container_Property_Id_padding:
            pSelf_->padding = GblVariant_float(pValue);
            break;
        case GUM_Container_Property_Id_margin:
            pSelf_->margin = GblVariant_float(pValue);
            break;
        case GUM_Container_Property_Id_minChildSize:
            pSelf_->minChildSize = GBL_CLAMP(GblVariant_float(pValue), 0.0f, 1.0f);
            break;
        case GUM_Container_Property_Id_direction:
            pSelf_->direction = GblVariant_enum(pValue);
            break;
        case GUM_Container_Property_Id_resizeWidgets:
            pSelf_->resizeWidgets = GblVariant_bool(pValue);
            break;
        case GUM_Container_Property_Id_alignWidgets:
            pSelf_->alignWidgets = GblVariant_bool(pValue);
            break;
        case GUM_Container_Property_Id_scrollable:
            pSelf_->scrollable = GblVariant_bool(pValue);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GUM_CONTAINER_CLASSOF(pSelf)->pFnUpdateContent(pSelf);
}

static GBL_RESULT GUM_Container_GblObject_property_(const GblObject* pObject,
                                                    const GblProperty* pProp,
                                                    GblVariant* pValue) {
    const GUM_Container*  pSelf  = GUM_CONTAINER(pObject);
    const GUM_Container_* pSelf_ = GUM_CONTAINER_(pSelf);

    switch (pProp->id) {
        case GUM_Container_Property_Id_padding:
            GblVariant_setFloat(pValue, pSelf_->padding);
            break;
        case GUM_Container_Property_Id_margin:
            GblVariant_setFloat(pValue, pSelf_->margin);
            break;
        case GUM_Container_Property_Id_minChildSize:
            GblVariant_setFloat(pValue, pSelf_->minChildSize);
            break;
        case GUM_Container_Property_Id_direction:
            GblVariant_setEnum(pValue, GBL_TYPEID(GUM_Direction), pSelf_->direction);
            break;
        case GUM_Container_Property_Id_resizeWidgets:
            GblVariant_setBool(pValue, pSelf_->resizeWidgets);
            break;
        case GUM_Container_Property_Id_alignWidgets:
            GblVariant_setBool(pValue, pSelf_->alignWidgets);
            break;
        case GUM_Container_Property_Id_scrollable:
            GblVariant_setBool(pValue, pSelf_->scrollable);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static size_t GUM_Container_widgetChildCount_(const GUM_Container* pSelf) {
    size_t count = 0;
    GblObject_foreachChild(GBL_OBJECT(pSelf), pChild) {
        if (GBL_TYPECHECK(GUM_Widget, pChild))
            ++count;
    }
    return count;
}

static GBL_RESULT GUM_Container_updateContent_(GUM_Container* pSelf) {
    GUM_Container_* pSelf_      = GUM_CONTAINER_(pSelf);
    GUM_Widget*     pSelfWidget = GUM_WIDGET(pSelf);
    const size_t    childCount  = GUM_Container_widgetChildCount_(pSelf);

    if GBL_UNLIKELY (childCount == 0) {
        pSelf_->scrollLimitX = 0.0f;
        pSelf_->scrollLimitY = 0.0f;
        pSelf_->scrollAnimatorX.from = pSelf_->scrollAnimatorX.to = pSelf_->scrollAnimatorX.current = 0.0f;
        pSelf_->scrollAnimatorY.from = pSelf_->scrollAnimatorY.to = pSelf_->scrollAnimatorY.current = 0.0f;
        return GBL_RESULT_SUCCESS;
    }

    const bool  horizontal   = pSelf_->direction == GUM_DIRECTION_HORIZONTAL;
    const float totalMargin  = pSelf_->margin  * 2.0f * (float)(childCount - 1);
    const float totalPadding = pSelf_->padding * 2.0f;

    const float containerMainPos      = horizontal ? pSelfWidget->x : pSelfWidget->y;
    const float containerSecondaryPos = horizontal ? pSelfWidget->y : pSelfWidget->x;
    const float containerMainDim      = horizontal ? pSelfWidget->w : pSelfWidget->h;
    const float containerSecondaryDim = horizontal ? pSelfWidget->h : pSelfWidget->w;

    const float cornerRadius = pSelfWidget->border_radius
                             * GBL_MIN(containerMainDim, containerSecondaryDim)
                             * 0.5f;
    const float roundnessInset = cornerRadius * (1.0f - 1.0f / sqrtf(2.0f));
    const float totalPaddingWithRoundness = totalPadding + roundnessInset * 2.0f;
    const float availableMainDim = GBL_MAX(containerMainDim - totalMargin - totalPaddingWithRoundness, 0.0f);
    const float minimumMainDim = GBL_MAX(containerMainDim, 0.0f) * pSelf_->minChildSize;
    const float resizedMainDim = GBL_MAX(availableMainDim / (float)childCount, minimumMainDim);
    const float resizedSecondaryDim = GBL_MAX(containerSecondaryDim - totalPaddingWithRoundness, 0.0f);

    GUM_Animator* pScrollAnimator = horizontal ? &pSelf_->scrollAnimatorX : &pSelf_->scrollAnimatorY;
    const float* pScrollOffset = &pScrollAnimator->current;

    float  offset        = containerMainPos + pSelf_->padding + roundnessInset;
    float  contentExtent = offset;
    size_t widgetIndex   = 0;

    GblObject_foreachChild(GBL_OBJECT(pSelf), pChild) {
        GUM_Widget* pChildWidget = GBL_AS(GUM_Widget, pChild);
        if GBL_UNLIKELY (!pChildWidget)
            continue;

        float* pWidgetMainPos      = horizontal ? &pChildWidget->x : &pChildWidget->y;
        float* pWidgetSecondaryPos = horizontal ? &pChildWidget->y : &pChildWidget->x;
        float* pWidgetMainDim      = horizontal ? &pChildWidget->w : &pChildWidget->h;
        float* pWidgetSecondaryDim = horizontal ? &pChildWidget->h : &pChildWidget->w;

        if (pSelf_->resizeWidgets) {
            *pWidgetMainDim      = resizedMainDim;
            *pWidgetSecondaryDim = resizedSecondaryDim;
        }

        if (pSelf_->alignWidgets) {
            *pWidgetMainPos = offset;
            const float availableSecondaryDim = containerSecondaryDim - totalPaddingWithRoundness;
            *pWidgetSecondaryPos = containerSecondaryPos + pSelf_->padding + roundnessInset
                                 + (availableSecondaryDim - *pWidgetSecondaryDim) / 2.0f;
            offset += *pWidgetMainDim;
            if (++widgetIndex < childCount)
                offset += pSelf_->margin * 2.0f;
            contentExtent = GBL_MAX(contentExtent,
                                    *pWidgetMainPos + *pWidgetMainDim + pSelf_->padding + roundnessInset);
        } else {
            ++widgetIndex;
            contentExtent = GBL_MAX(contentExtent,
                                    *pWidgetMainPos + *pScrollOffset + *pWidgetMainDim);
        }
    }

    const bool  contentOverflows = contentExtent > containerMainPos + containerMainDim;
    const float maxScroll = contentOverflows ? contentExtent - containerMainPos - containerMainDim : 0.0f;
    const float scrollLimit = pSelf_->scrollable ? maxScroll : 0.0f;

    if (horizontal) {
        pSelf_->scrollLimitX = scrollLimit;
        pSelf_->scrollLimitY = 0.0f;
    } else {
        pSelf_->scrollLimitX = 0.0f;
        pSelf_->scrollLimitY = scrollLimit;
    }

    pScrollAnimator->from    = GBL_CLAMP(pScrollAnimator->from,    0.0f, scrollLimit);
    pScrollAnimator->to      = GBL_CLAMP(pScrollAnimator->to,      0.0f, scrollLimit);
    pScrollAnimator->current = GBL_CLAMP(pScrollAnimator->current, 0.0f, scrollLimit);

    GUM_Rectangle outgoingClip = GUM_Widget_clipRect_(pSelfWidget);

    if (pSelf_->scrollable && contentOverflows) {
        const GUM_Vector2 position = GUM_get_absolute_position_(pSelfWidget);
        const float borderWidth = pSelfWidget->border_width;
        const GUM_Rectangle selfRect = {
            position.x + borderWidth,
            position.y + borderWidth,
            pSelfWidget->w - 2.0f * borderWidth,
            pSelfWidget->h - 2.0f * borderWidth
        };
        outgoingClip = GUM_Rectangle_intersect(outgoingClip, selfRect);
    }

    // Finish direct child writes before invoking overridable child layout callbacks.
    GblObject_foreachChild(GBL_OBJECT(pSelf), pChild) {
        GUM_Widget* pChildWidget = GBL_AS(GUM_Widget, pChild);
        if GBL_UNLIKELY (!pChildWidget)
            continue;

        if (pSelf_->alignWidgets) {
            float* pWidgetMainPos = horizontal ? &pChildWidget->x : &pChildWidget->y;
            *pWidgetMainPos -= *pScrollOffset;
        }

        GUM_Widget_setClipRect_(pChildWidget, outgoingClip);
    }

    GUM_ContainerLayoutSnapshot_ snapshot;
    GBL_RESULT result = GUM_Container_layoutSnapshot_(pSelf, &snapshot);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    GBL_RESULT firstFailure = GBL_RESULT_SUCCESS;
    const size_t count = GblArrayList_size(&snapshot.containers);
    for (size_t i = 0; i < count; ++i) {
        GUM_Container* pChild = *(GUM_Container**)GblArrayList_at(&snapshot.containers, i);
        result = GUM_CONTAINER_CLASSOF(pChild)->pFnUpdateContent(pChild);
        if (GBL_RESULT_SUCCESS(firstFailure) && !GBL_RESULT_SUCCESS(result))
            firstFailure = result;
    }
    GUM_Container_layoutSnapshotRelease_(&snapshot);
    return firstFailure;
}

static GUM_Animator* GUM_Container_scrollAnimator_(GUM_Container* pSelf, GUM_Direction axis) {
    GUM_Container_* pSelf_ = GUM_CONTAINER_(pSelf);
    switch (axis) {
        case GUM_DIRECTION_HORIZONTAL: return &pSelf_->scrollAnimatorX;
        case GUM_DIRECTION_VERTICAL:   return &pSelf_->scrollAnimatorY;
        default:                       return nullptr;
    }
}

static float GUM_Container_scrollLimit_(const GUM_Container* pSelf, GUM_Direction axis) {
    const GUM_Container_* pSelf_ = GUM_CONTAINER_(pSelf);
    switch (axis) {
        case GUM_DIRECTION_HORIZONTAL: return pSelf_->scrollLimitX;
        case GUM_DIRECTION_VERTICAL:   return pSelf_->scrollLimitY;
        default:                       return 0.0f;
    }
}

void GUM_Container_scrollTo_(GUM_Container* pSelf, GUM_Direction axis, float offset) {
    if (!pSelf)
        return;

    GUM_Animator* pAnimator = GUM_Container_scrollAnimator_(pSelf, axis);
    if (!pAnimator)
        return;

    const float target = GBL_CLAMP(offset, 0.0f, GUM_Container_scrollLimit_(pSelf, axis));
    if (pAnimator->to == target)
        return;

    GUM_Animator_set(pAnimator, target);
}

void GUM_Container_scrollBy_(GUM_Container* pSelf, GUM_Direction axis, float delta) {
    if (!pSelf || delta == 0.0f)
        return;

    GUM_Animator* pAnimator = GUM_Container_scrollAnimator_(pSelf, axis);
    if (!pAnimator)
        return;

    GUM_Container_scrollTo_(pSelf, axis, pAnimator->to + delta);
}

static GBL_RESULT GUM_Container_update_(GUM_Widget* pSelf) {
    GUM_Container*  pContainer  = GUM_CONTAINER(pSelf);
    GUM_Container_* pContainer_ = GUM_CONTAINER_(pContainer);
    const float dt = GUM_Backend_frametime();

    const bool changedX = GUM_Animator_update(&pContainer_->scrollAnimatorX, dt);
    const bool changedY = GUM_Animator_update(&pContainer_->scrollAnimatorY, dt);

    if (changedX || changedY)
        return GUM_CONTAINER_CLASSOF(pContainer)->pFnUpdateContent(pContainer);

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Container_Object_instantiated_(GblObject* pObject) {
    GblObjectClass* pWidgetClass = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE));
    const GBL_RESULT result = pWidgetClass->pFnInstantiated(pObject);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    return GUM_CONTAINER_CLASSOF(pObject)->pFnUpdateContent(GUM_CONTAINER(pObject));
}

static GBL_RESULT GUM_ContainerClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_CONTAINER_TYPE))
        GBL_PROPERTIES_REGISTER(GUM_Container);

    GBL_OBJECT_CLASS(pClass)->pFnSetProperty  = GUM_Container_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty     = GUM_Container_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)->pFnInstantiated = GUM_Container_Object_instantiated_;

    GUM_WIDGET_CLASS(pClass)->pFnUpdate = GUM_Container_update_;
    GUM_CONTAINER_CLASS(pClass)->pFnUpdateContent = GUM_Container_updateContent_;

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Container_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Container"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){ .classSize           = sizeof(GUM_ContainerClass),
                                                       .pFnClassInit        = GUM_ContainerClass_init_,
                                                       .instanceSize        = sizeof(GUM_Container),
                                                       .instancePrivateSize = sizeof(GUM_Container_),
                                                       .pFnInstanceInit     = GUM_Container_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

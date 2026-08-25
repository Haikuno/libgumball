#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_inputsystem.h>
#include <gumball/elements/gumball_container.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/devices/gumball_mouse.h>

#include <gimbal/gimbal_containers.h>

#include "../devices/gumball_inputdevice_.h"
#include "gumball_container_.h"
#include "gumball_widget_.h"

typedef struct GUM_ContainerLayoutSnapshot_ {
    GblArrayList    containers;
    GUM_Container* stack[GUM_SNAPSHOT_INLINE_CAPACITY];
} GUM_ContainerLayoutSnapshot_;

typedef struct GUM_ContainerScrollbarDragState_ {
    GUM_Container* pContainer;
    GBL_RESULT     result;
    bool           mouseFound;
    bool           leftDown;
} GUM_ContainerScrollbarDragState_;

static GUM_ScrollAxis_ GUM_Container_scrollAxis_(GUM_Direction direction) {
    switch (direction) {
        case GUM_DIRECTION_HORIZONTAL: return GUM_SCROLL_AXIS_X;
        case GUM_DIRECTION_VERTICAL:   return GUM_SCROLL_AXIS_Y;
        default:                       return GUM_SCROLL_AXIS_NULL;
    }
}

static GUM_Color GUM_Container_color_(uint32_t color) {
    return (GUM_Color){
        .r = (color >> 24) & 0xFF,
        .g = (color >> 16) & 0xFF,
        .b = (color >> 8)  & 0xFF,
        .a = color & 0xFF
    };
}

static bool GUM_Container_pointInRect_(GUM_Vector2 point, GUM_Rectangle rect) {
    return point.x >= rect.x && point.x < rect.x + rect.width &&
           point.y >= rect.y && point.y < rect.y + rect.height;
}

static float GUM_Container_scrollbarMainStart_(GUM_Direction direction, GUM_Rectangle rect) {
    return direction == GUM_DIRECTION_HORIZONTAL ? rect.x : rect.y;
}

static float GUM_Container_scrollbarMainLength_(GUM_Direction direction, GUM_Rectangle rect) {
    return direction == GUM_DIRECTION_HORIZONTAL ? rect.width : rect.height;
}

static float GUM_Container_pointerMain_(GUM_Direction direction, GUM_Vector2 point) {
    return direction == GUM_DIRECTION_HORIZONTAL ? point.x : point.y;
}

static float GUM_Container_scrollbarGutter_(const GUM_Container* pSelf) {
    if (!GUM_Container_scrollbarVisible(pSelf))
        return 0.0f;

    const GUM_Widget* pWidget = GUM_WIDGET(pSelf);
    const bool horizontal = GUM_Container_direction(pSelf) == GUM_DIRECTION_HORIZONTAL;
    const float crossDim = horizontal ? pWidget->h : pWidget->w;
    const float innerCrossDim = GBL_MAX(crossDim - 2.0f * pWidget->border_width, 0.0f);
    const float thickness = GBL_MIN(GUM_Container_scrollbarThickness(pSelf), innerCrossDim);
    return GBL_MIN(thickness + GUM_Container_scrollbarGap(pSelf), crossDim);
}

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

    pSelf_->padding               = 5.0f;
    pSelf_->margin                = 2.0f;
    pSelf_->minChildSize          = 0.15f;
    pSelf_->scrollbarThickness    = 8.0f;
    pSelf_->scrollbarGap          = 2.0f;
    pSelf_->scrollbarMinThumbSize = 18.0f;
    pSelf_->scrollbarRoundness    = 1.0f;
    pSelf_->scrollbarTrackColor   = 0x00000060u;
    pSelf_->scrollbarThumbColor   = 0xFFFFFFC0u;
    pSelf_->scrollbarGrabOffset   = 0.0f;
    pSelf_->direction             = GUM_DIRECTION_VERTICAL;
    pSelf_->scrollbarPolicy       = GUM_SCROLLBAR_AUTO;
    pSelf_->resizeWidgets         = true;
    pSelf_->alignWidgets          = true;
    pSelf_->scrollable            = true;
    pSelf_->scrollbarDragging     = false;
    GUM_ScrollViewport_init_(&pSelf_->viewport);
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

GBL_EXPORT GUM_ScrollbarPolicy GUM_Container_scrollbarPolicy(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->scrollbarPolicy : GUM_SCROLLBAR_NONE;
}

GBL_EXPORT float GUM_Container_scrollbarThickness(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->scrollbarThickness : 0.0f;
}

GBL_EXPORT float GUM_Container_scrollbarGap(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->scrollbarGap : 0.0f;
}

GBL_EXPORT float GUM_Container_scrollbarMinThumbSize(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->scrollbarMinThumbSize : 0.0f;
}

GBL_EXPORT float GUM_Container_scrollbarRoundness(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->scrollbarRoundness : 0.0f;
}

GBL_EXPORT uint32_t GUM_Container_scrollbarTrackColor(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->scrollbarTrackColor : 0u;
}

GBL_EXPORT uint32_t GUM_Container_scrollbarThumbColor(const GUM_Container* pSelf) {
    return pSelf ? GUM_CONTAINER_(pSelf)->scrollbarThumbColor : 0u;
}

static GBL_RESULT GUM_Container_GblObject_setProperty_(GblObject* pObject,
                                                       const GblProperty* pProp,
                                                       GblVariant* pValue) {
    GUM_Container*  pSelf  = GUM_CONTAINER(pObject);
    GUM_Container_* pSelf_ = GUM_CONTAINER_(pSelf);
    bool refreshLayout = true;

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
            pSelf_->scrollbarDragging = false;
            break;
        case GUM_Container_Property_Id_resizeWidgets:
            pSelf_->resizeWidgets = GblVariant_bool(pValue);
            break;
        case GUM_Container_Property_Id_alignWidgets:
            pSelf_->alignWidgets = GblVariant_bool(pValue);
            break;
        case GUM_Container_Property_Id_scrollable:
            pSelf_->scrollable = GblVariant_bool(pValue);
            if (!pSelf_->scrollable)
                pSelf_->scrollbarDragging = false;
            break;
        case GUM_Container_Property_Id_scrollbarPolicy:
            pSelf_->scrollbarPolicy = GblVariant_enum(pValue);
            if (pSelf_->scrollbarPolicy == GUM_SCROLLBAR_NONE)
                pSelf_->scrollbarDragging = false;
            break;
        case GUM_Container_Property_Id_scrollbarThickness:
            pSelf_->scrollbarThickness = GBL_MAX(GblVariant_float(pValue), 1.0f);
            break;
        case GUM_Container_Property_Id_scrollbarGap:
            pSelf_->scrollbarGap = GBL_MAX(GblVariant_float(pValue), 0.0f);
            break;
        case GUM_Container_Property_Id_scrollbarMinThumbSize:
            pSelf_->scrollbarMinThumbSize = GBL_MAX(GblVariant_float(pValue), 1.0f);
            refreshLayout = false;
            break;
        case GUM_Container_Property_Id_scrollbarRoundness:
            pSelf_->scrollbarRoundness = GBL_CLAMP(GblVariant_float(pValue), 0.0f, 1.0f);
            refreshLayout = false;
            break;
        case GUM_Container_Property_Id_scrollbarTrackColor:
            pSelf_->scrollbarTrackColor = GblVariant_uint32(pValue);
            refreshLayout = false;
            break;
        case GUM_Container_Property_Id_scrollbarThumbColor:
            pSelf_->scrollbarThumbColor = GblVariant_uint32(pValue);
            refreshLayout = false;
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return refreshLayout ? GUM_CONTAINER_CLASSOF(pSelf)->pFnUpdateContent(pSelf)
                         : GBL_RESULT_SUCCESS;
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
        case GUM_Container_Property_Id_scrollbarPolicy:
            GblVariant_setEnum(pValue, GBL_TYPEID(GUM_ScrollbarPolicy), pSelf_->scrollbarPolicy);
            break;
        case GUM_Container_Property_Id_scrollbarThickness:
            GblVariant_setFloat(pValue, pSelf_->scrollbarThickness);
            break;
        case GUM_Container_Property_Id_scrollbarGap:
            GblVariant_setFloat(pValue, pSelf_->scrollbarGap);
            break;
        case GUM_Container_Property_Id_scrollbarMinThumbSize:
            GblVariant_setFloat(pValue, pSelf_->scrollbarMinThumbSize);
            break;
        case GUM_Container_Property_Id_scrollbarRoundness:
            GblVariant_setFloat(pValue, pSelf_->scrollbarRoundness);
            break;
        case GUM_Container_Property_Id_scrollbarTrackColor:
            GblVariant_setUint32(pValue, pSelf_->scrollbarTrackColor);
            break;
        case GUM_Container_Property_Id_scrollbarThumbColor:
            GblVariant_setUint32(pValue, pSelf_->scrollbarThumbColor);
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

GBL_EXPORT GblBool GUM_Container_scrollbarVisible(const GUM_Container* pSelf) {
    if (!pSelf || !GUM_Container_scrollable(pSelf))
        return GBL_FALSE;

    const GUM_Direction direction = GUM_Container_direction(pSelf);
    if (direction != GUM_DIRECTION_HORIZONTAL && direction != GUM_DIRECTION_VERTICAL)
        return GBL_FALSE;

    switch (GUM_Container_scrollbarPolicy(pSelf)) {
        case GUM_SCROLLBAR_ALWAYS: return GBL_TRUE;
        case GUM_SCROLLBAR_AUTO:   return GUM_Container_scrollRange(pSelf) > 0.0f;
        case GUM_SCROLLBAR_NONE:
        default:                   return GBL_FALSE;
    }
}

GblBool GUM_Container_scrollbarGeometry_(const GUM_Container* pSelf,
                                         GUM_Rectangle* pTrack,
                                         GUM_Rectangle* pThumb) {
    if (!pSelf || !pTrack || !pThumb || !GUM_Container_scrollbarVisible(pSelf))
        return GBL_FALSE;

    const GUM_Widget* pWidget = GUM_WIDGET(pSelf);
    const GUM_Direction direction = GUM_Container_direction(pSelf);
    const bool horizontal = direction == GUM_DIRECTION_HORIZONTAL;
    const GUM_Vector2 position = GUM_get_absolute_position_(pWidget);
    const float borderWidth = pWidget->border_width;
    const float innerWidth  = GBL_MAX(pWidget->w - 2.0f * borderWidth, 0.0f);
    const float innerHeight = GBL_MAX(pWidget->h - 2.0f * borderWidth, 0.0f);
    const float crossSize   = horizontal ? innerHeight : innerWidth;
    const float thickness   = GBL_MIN(GUM_Container_scrollbarThickness(pSelf), crossSize);

    if (thickness <= 0.0f)
        return GBL_FALSE;

    if (horizontal) {
        *pTrack = (GUM_Rectangle){
            .x = position.x + borderWidth,
            .y = position.y + pWidget->h - borderWidth - thickness,
            .width = innerWidth,
            .height = thickness
        };
    } else {
        *pTrack = (GUM_Rectangle){
            .x = position.x + pWidget->w - borderWidth - thickness,
            .y = position.y + borderWidth,
            .width = thickness,
            .height = innerHeight
        };
    }

    const float trackLength = GUM_Container_scrollbarMainLength_(direction, *pTrack);
    if (trackLength <= 0.0f)
        return GBL_FALSE;

    const float range = GUM_Container_scrollRange(pSelf);
    const float viewportLength = horizontal ? innerWidth : innerHeight;
    const float contentLength = viewportLength + range;
    float thumbLength = range > 0.0f && contentLength > 0.0f
                      ? trackLength * viewportLength / contentLength
                      : trackLength;
    thumbLength = GBL_CLAMP(thumbLength,
                            GBL_MIN(GUM_Container_scrollbarMinThumbSize(pSelf), trackLength),
                            trackLength);

    const float travel = trackLength - thumbLength;
    const float ratio = range > 0.0f ? GUM_Container_scrollPosition(pSelf) / range : 0.0f;
    const float thumbStart = GUM_Container_scrollbarMainStart_(direction, *pTrack)
                           + GBL_CLAMP(ratio, 0.0f, 1.0f) * travel;

    if (horizontal) {
        *pThumb = (GUM_Rectangle){
            .x = thumbStart,
            .y = pTrack->y,
            .width = thumbLength,
            .height = pTrack->height
        };
    } else {
        *pThumb = (GUM_Rectangle){
            .x = pTrack->x,
            .y = thumbStart,
            .width = pTrack->width,
            .height = thumbLength
        };
    }

    return GBL_TRUE;
}

static GBL_RESULT GUM_Container_updateContent_(GUM_Container* pSelf) {
    GUM_Container_* pSelf_      = GUM_CONTAINER_(pSelf);
    GUM_Widget*     pSelfWidget = GUM_WIDGET(pSelf);
    const size_t    childCount  = GUM_Container_widgetChildCount_(pSelf);

    if GBL_UNLIKELY (childCount == 0) {
        GUM_ScrollViewport_reset_(&pSelf_->viewport);
        return GBL_RESULT_SUCCESS;
    }

    const bool  horizontal   = pSelf_->direction == GUM_DIRECTION_HORIZONTAL;
    const float totalMargin  = pSelf_->margin * 2.0f * (float)(childCount - 1);
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
    const GUM_ScrollAxis_ scrollAxis = GUM_Container_scrollAxis_(pSelf_->direction);
    const float scrollOffset = GUM_ScrollViewport_position_(&pSelf_->viewport, scrollAxis);

    float  offset        = containerMainPos + pSelf_->padding + roundnessInset;
    float  contentExtent = offset;
    size_t widgetIndex   = 0;

    GblObject_foreachChild(GBL_OBJECT(pSelf), pChild) {
        GUM_Widget* pChildWidget = GBL_AS(GUM_Widget, pChild);
        if GBL_UNLIKELY (!pChildWidget)
            continue;

        float* pWidgetMainPos = horizontal ? &pChildWidget->x : &pChildWidget->y;
        float* pWidgetMainDim = horizontal ? &pChildWidget->w : &pChildWidget->h;

        if (pSelf_->resizeWidgets)
            *pWidgetMainDim = resizedMainDim;

        if (pSelf_->alignWidgets) {
            *pWidgetMainPos = offset;
            offset += *pWidgetMainDim;
            if (++widgetIndex < childCount)
                offset += pSelf_->margin * 2.0f;
            contentExtent = GBL_MAX(contentExtent,
                                    *pWidgetMainPos + *pWidgetMainDim + pSelf_->padding + roundnessInset);
        } else {
            ++widgetIndex;
            contentExtent = GBL_MAX(contentExtent,
                                    *pWidgetMainPos + scrollOffset + *pWidgetMainDim);
        }
    }

    const bool  contentOverflows = contentExtent > containerMainPos + containerMainDim;
    const float maxScroll = contentOverflows ? contentExtent - containerMainPos - containerMainDim : 0.0f;
    const float scrollRange = pSelf_->scrollable ? maxScroll : 0.0f;

    GUM_ScrollViewport_setRange_(&pSelf_->viewport,
                                 GUM_SCROLL_AXIS_X,
                                 horizontal ? scrollRange : 0.0f);
    GUM_ScrollViewport_setRange_(&pSelf_->viewport,
                                 GUM_SCROLL_AXIS_Y,
                                 horizontal ? 0.0f : scrollRange);

    const float scrollbarGutter = GUM_Container_scrollbarGutter_(pSelf);
    const float availableSecondaryDim = GBL_MAX(containerSecondaryDim
                                              - totalPaddingWithRoundness
                                              - scrollbarGutter,
                                              0.0f);
    const float resizedSecondaryDim = availableSecondaryDim;
    const float clampedScrollOffset = GUM_ScrollViewport_position_(&pSelf_->viewport, scrollAxis);
    GUM_Rectangle outgoingClip = GUM_Widget_clipRect_(pSelfWidget);

    if (pSelf_->scrollable && (contentOverflows || scrollbarGutter > 0.0f)) {
        const GUM_Vector2 position = GUM_get_absolute_position_(pSelfWidget);
        const float borderWidth = pSelfWidget->border_width;
        GUM_Rectangle selfRect = {
            position.x + borderWidth,
            position.y + borderWidth,
            GBL_MAX(pSelfWidget->w - 2.0f * borderWidth, 0.0f),
            GBL_MAX(pSelfWidget->h - 2.0f * borderWidth, 0.0f)
        };

        if (horizontal)
            selfRect.height = GBL_MAX(selfRect.height - scrollbarGutter, 0.0f);
        else
            selfRect.width = GBL_MAX(selfRect.width - scrollbarGutter, 0.0f);

        outgoingClip = GUM_Rectangle_intersect(outgoingClip, selfRect);
    }

    // Finish direct child writes before invoking overridable child layout callbacks.
    GblObject_foreachChild(GBL_OBJECT(pSelf), pChild) {
        GUM_Widget* pChildWidget = GBL_AS(GUM_Widget, pChild);
        if GBL_UNLIKELY (!pChildWidget)
            continue;

        float* pWidgetSecondaryPos = horizontal ? &pChildWidget->y : &pChildWidget->x;
        float* pWidgetSecondaryDim = horizontal ? &pChildWidget->h : &pChildWidget->w;

        if (pSelf_->resizeWidgets)
            *pWidgetSecondaryDim = resizedSecondaryDim;

        if (pSelf_->alignWidgets) {
            float* pWidgetMainPos = horizontal ? &pChildWidget->x : &pChildWidget->y;
            *pWidgetMainPos -= clampedScrollOffset;
            *pWidgetSecondaryPos = containerSecondaryPos + pSelf_->padding + roundnessInset
                                 + (availableSecondaryDim - *pWidgetSecondaryDim) / 2.0f;
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

void GUM_Container_scrollTo_(GUM_Container* pSelf, GUM_Direction axis, float offset) {
    if (pSelf)
        GUM_ScrollViewport_scrollTo_(&GUM_CONTAINER_(pSelf)->viewport,
                                     GUM_Container_scrollAxis_(axis),
                                     offset);
}

void GUM_Container_scrollBy_(GUM_Container* pSelf, GUM_Direction axis, float delta) {
    if (pSelf)
        GUM_ScrollViewport_scrollBy_(&GUM_CONTAINER_(pSelf)->viewport,
                                     GUM_Container_scrollAxis_(axis),
                                     delta);
}

GBL_EXPORT float GUM_Container_scrollPosition(const GUM_Container* pSelf) {
    return pSelf ? GUM_ScrollViewport_position_(&GUM_CONTAINER_(pSelf)->viewport,
                                                 GUM_Container_scrollAxis_(GUM_Container_direction(pSelf)))
                 : 0.0f;
}

GBL_EXPORT float GUM_Container_scrollRange(const GUM_Container* pSelf) {
    return pSelf ? GUM_ScrollViewport_range_(&GUM_CONTAINER_(pSelf)->viewport,
                                              GUM_Container_scrollAxis_(GUM_Container_direction(pSelf)))
                 : 0.0f;
}

GBL_EXPORT void GUM_Container_scrollTo(GUM_Container* pSelf, float position) {
    if (pSelf)
        GUM_Container_scrollTo_(pSelf, GUM_Container_direction(pSelf), position);
}

GBL_EXPORT void GUM_Container_scrollBy(GUM_Container* pSelf, float delta) {
    if (pSelf)
        GUM_Container_scrollBy_(pSelf, GUM_Container_direction(pSelf), delta);
}

static GBL_RESULT GUM_Container_scrollbarDragTo_(GUM_Container* pSelf, GUM_Vector2 point) {
    GUM_Rectangle track;
    GUM_Rectangle thumb;
    if (!GUM_Container_scrollbarGeometry_(pSelf, &track, &thumb))
        return GBL_RESULT_SUCCESS;

    const float range = GUM_Container_scrollRange(pSelf);
    if (range <= 0.0f)
        return GBL_RESULT_SUCCESS;

    const GUM_Direction direction = GUM_Container_direction(pSelf);
    const float trackStart = GUM_Container_scrollbarMainStart_(direction, track);
    const float trackLength = GUM_Container_scrollbarMainLength_(direction, track);
    const float thumbLength = GUM_Container_scrollbarMainLength_(direction, thumb);
    const float travel = trackLength - thumbLength;
    if (travel <= 0.0f)
        return GBL_RESULT_SUCCESS;

    const float pointer = GUM_Container_pointerMain_(direction, point);
    const float thumbStart = GBL_CLAMP(pointer - GUM_CONTAINER_(pSelf)->scrollbarGrabOffset,
                                       trackStart,
                                       trackStart + travel);
    const float target = (thumbStart - trackStart) / travel * range;

    if (GUM_ScrollViewport_setPosition_(&GUM_CONTAINER_(pSelf)->viewport,
                                        GUM_Container_scrollAxis_(direction),
                                        target))
        return GUM_CONTAINER_CLASSOF(pSelf)->pFnUpdateContent(pSelf);

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Container_scrollbarPress_(GUM_Container* pSelf, GUM_Vector2 point) {
    GUM_Rectangle track;
    GUM_Rectangle thumb;
    if (!GUM_Container_scrollbarGeometry_(pSelf, &track, &thumb) ||
        !GUM_Container_pointInRect_(point, track))
        return GBL_RESULT_PARTIAL;

    GUM_Container_* pSelf_ = GUM_CONTAINER_(pSelf);
    const GUM_Direction direction = GUM_Container_direction(pSelf);
    const float pointer = GUM_Container_pointerMain_(direction, point);
    const float thumbStart = GUM_Container_scrollbarMainStart_(direction, thumb);
    const float thumbLength = GUM_Container_scrollbarMainLength_(direction, thumb);

    if (GUM_Container_scrollRange(pSelf) <= 0.0f) {
        pSelf_->scrollbarDragging = false;
        return GBL_RESULT_SUCCESS;
    }

    if (GUM_Container_pointInRect_(point, thumb))
        pSelf_->scrollbarGrabOffset = pointer - thumbStart;
    else
        pSelf_->scrollbarGrabOffset = thumbLength * 0.5f;

    pSelf_->scrollbarDragging = true;
    return GUM_Container_scrollbarDragTo_(pSelf, point);
}

static void GUM_Container_scrollbarDragVisit_(GUM_InputDevice* pDevice, void* pClosure) {
    GUM_ContainerScrollbarDragState_* pState = pClosure;
    if (pState->mouseFound || !GUM_InputSystem_deviceEnabled(pDevice))
        return;

    GUM_Mouse* pMouse = GBL_AS(GUM_Mouse, pDevice);
    if (!pMouse)
        return;

    pState->mouseFound = true;
    pState->leftDown = (pDevice->buttons & GUM_MOUSE_BUTTON_LEFT) != 0;
    if (pState->leftDown)
        pState->result = GUM_Container_scrollbarDragTo_(pState->pContainer,
                                                        GUM_POINTER(pMouse)->position);
}

static GBL_RESULT GUM_Container_updateScrollbarDrag_(GUM_Container* pSelf) {
    GUM_Container_* pSelf_ = GUM_CONTAINER_(pSelf);
    if (!pSelf_->scrollbarDragging)
        return GBL_RESULT_SUCCESS;

    if (!GUM_Container_scrollbarVisible(pSelf) || GUM_Container_scrollRange(pSelf) <= 0.0f) {
        pSelf_->scrollbarDragging = false;
        return GBL_RESULT_SUCCESS;
    }

    GUM_ContainerScrollbarDragState_ state = {
        .pContainer = pSelf,
        .result = GBL_RESULT_SUCCESS,
        .mouseFound = false,
        .leftDown = false
    };

    const GBL_RESULT visitResult = GUM_InputDevice_foreach_(GUM_MOUSE_TYPE,
                                                            GUM_Container_scrollbarDragVisit_,
                                                            &state);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(visitResult))
        return visitResult;
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(state.result))
        return state.result;

    if (!state.mouseFound || !state.leftDown)
        pSelf_->scrollbarDragging = false;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Container_inputEvent_(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    GUM_Container* pContainer = GUM_CONTAINER(pSelf);
    GUM_Event_Mouse* pMouseEvent = GBL_AS(GUM_Event_Mouse, pEvent);

    if (pMouseEvent && pEvent->button == GUM_MOUSE_BUTTON_LEFT) {
        if (pEvent->state == GUM_INPUTSTATE_PRESS) {
            const GBL_RESULT result = GUM_Container_scrollbarPress_(pContainer,
                                                                    GUM_EVENT_POINTER(pMouseEvent)->position);
            if (result != GBL_RESULT_PARTIAL) {
                GblEvent_accept(GBL_EVENT(pEvent));
                return result;
            }
        } else if (pEvent->state == GUM_INPUTSTATE_RELEASE && GUM_CONTAINER_(pContainer)->scrollbarDragging) {
            GUM_CONTAINER_(pContainer)->scrollbarDragging = false;
            GblEvent_accept(GBL_EVENT(pEvent));
            return GBL_RESULT_SUCCESS;
        }
    }

    GUM_WidgetClass* pWidgetClass = GUM_WIDGET_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE));
    return pWidgetClass->pFnInputEvent(pSelf, pEvent);
}

static GBL_RESULT GUM_Container_draw_(GUM_Widget* pSelf, GUM_Renderer* pRenderer) {
    GUM_WidgetClass* pWidgetClass = GUM_WIDGET_CLASS(GblClass_weakRefDefault(GUM_WIDGET_TYPE));
    GBL_RESULT result = pWidgetClass->pFnDraw(pSelf, pRenderer);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    GUM_Container* pContainer = GUM_CONTAINER(pSelf);
    GUM_Rectangle track;
    GUM_Rectangle thumb;
    if (!GUM_Container_scrollbarGeometry_(pContainer, &track, &thumb))
        return GBL_RESULT_SUCCESS;

    const GUM_Rectangle clip = GUM_Widget_clipRect_(pSelf);
    const bool needsClip = clip.x != GUM_CLIP_RECT_NONE_.x ||
                           clip.width != GUM_CLIP_RECT_NONE_.width;
    bool scissorActive = false;

    if (needsClip) {
        const GUM_Rectangle overlap = GUM_Rectangle_intersect(clip, track);
        if (overlap.width <= 0.0f || overlap.height <= 0.0f)
            return GBL_RESULT_SUCCESS;

        result = GUM_Backend_beginScissor(pRenderer, clip);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            return result;
        scissorActive = true;
    }

    const GUM_Color trackColor = GUM_Container_color_(GUM_Container_scrollbarTrackColor(pContainer));
    const GUM_Color thumbColor = GUM_Container_color_(GUM_Container_scrollbarThumbColor(pContainer));
    const float roundness = GUM_Container_scrollbarRoundness(pContainer);

    if (trackColor.a) {
        result = GUM_Backend_rectangleDraw(pRenderer, track, roundness, trackColor);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            goto cleanup;
    }

    if (thumbColor.a)
        result = GUM_Backend_rectangleDraw(pRenderer, thumb, roundness, thumbColor);

cleanup:
    if (scissorActive) {
        const GBL_RESULT endResult = GUM_Backend_endScissor(pRenderer);
        if (GBL_RESULT_SUCCESS(result) && !GBL_RESULT_SUCCESS(endResult))
            result = endResult;
    }
    return result;
}

static GBL_RESULT GUM_Container_update_(GUM_Widget* pSelf) {
    GUM_Container* pContainer = GUM_CONTAINER(pSelf);

    GBL_RESULT result = GUM_Container_updateScrollbarDrag_(pContainer);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    if (GUM_ScrollViewport_update_(&GUM_CONTAINER_(pContainer)->viewport,
                                   GUM_Backend_frametime()))
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

    GUM_WIDGET_CLASS(pClass)->pFnUpdate     = GUM_Container_update_;
    GUM_WIDGET_CLASS(pClass)->pFnDraw       = GUM_Container_draw_;
    GUM_WIDGET_CLASS(pClass)->pFnInputEvent = GUM_Container_inputEvent_;
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

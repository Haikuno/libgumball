#include <gumball/ifaces/gumball_iresource.h>
#include <gumball/elements/gumball_widget.h>
#include <gumball/elements/gumball_button.h>
#include <gumball/elements/gumball_container.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/elements/gumball_common.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/core/gumball_inputsystem.h>
#include <gimbal/meta/signals/gimbal_c_closure.h>
#include <gimbal/containers/gimbal_array_list.h>

#include "../core/gumball_inputsystem_.h"
#include "gumball_root_.h"
#include "gumball_widget_.h"

static GBL_RESULT GUM_Widget_handleInputEvent_(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    if (!pSelf || !pEvent || !pSelf->isInteractive || !GUM_Widget_isActive(pSelf))
        return GBL_RESULT_SUCCESS;

    if (pEvent->state != GUM_INPUTSTATE_PRESS && pEvent->state != GUM_INPUTSTATE_RELEASE)
        return GBL_RESULT_SUCCESS;

    if (pEvent->action <= GUM_INPUTACTION_NULL ||
        (pEvent->action >= GUM_INPUTACTION_COUNT && pEvent->action != GUM_INPUTACTION_UNBOUND))
        return GBL_RESULT_SUCCESS;

    const GUM_InputState state = pEvent->state;
    const GUM_InputAction action = pEvent->action;

    static const char* pressActionSignals_[] = {
        [GUM_INPUTACTION_CONFIRM]    = "onPressConfirm",
        [GUM_INPUTACTION_CANCEL]     = "onPressCancel",
        [GUM_INPUTACTION_MOVE_UP]    = "onPressMoveUp",
        [GUM_INPUTACTION_MOVE_DOWN]  = "onPressMoveDown",
        [GUM_INPUTACTION_MOVE_LEFT]  = "onPressMoveLeft",
        [GUM_INPUTACTION_MOVE_RIGHT] = "onPressMoveRight"
    };

    static const char* releaseActionSignals_[] = {
        [GUM_INPUTACTION_CONFIRM]    = "onReleaseConfirm",
        [GUM_INPUTACTION_CANCEL]     = "onReleaseCancel",
        [GUM_INPUTACTION_MOVE_UP]    = "onReleaseMoveUp",
        [GUM_INPUTACTION_MOVE_DOWN]  = "onReleaseMoveDown",
        [GUM_INPUTACTION_MOVE_LEFT]  = "onReleaseMoveLeft",
        [GUM_INPUTACTION_MOVE_RIGHT] = "onReleaseMoveRight"
    };

    // Both signals use the same emitter.
    GblBox_ref(GBL_BOX(pSelf));

    const char* signal;

    if (state == GUM_INPUTSTATE_PRESS) {
        GBL_EMIT(pSelf, "onPress", pEvent);
        signal = action == GUM_INPUTACTION_UNBOUND ? "onPressUnbound" : pressActionSignals_[action];
    } else {
        GBL_EMIT(pSelf, "onRelease", pEvent);
        signal = action == GUM_INPUTACTION_UNBOUND ? "onReleaseUnbound" : releaseActionSignals_[action];
    }

    GBL_EMIT(pSelf, signal);
    GblEvent_accept(GBL_EVENT(pEvent));
    GblBox_unref(GBL_BOX(pSelf));
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_init_(GblInstance* pInstance) {
    GUM_Widget*  pSelf  = GUM_WIDGET(pInstance);
    GUM_Widget_* pSelf_ = GUM_WIDGET_(pSelf);

    pSelf_->pDrawPrev        = nullptr;
    pSelf_->pDrawNext        = nullptr;
    pSelf_->pLabel           = nullptr;
    pSelf_->pFont            = GUM_Backend_Font_default();
    pSelf_->pTexture         = nullptr;
    pSelf_->clipRect         = GUM_CLIP_RECT_NONE_;
    pSelf_->enableOrder      = 0;
    pSelf_->zIndex           = 50;
    pSelf_->focusCount       = 0;
    pSelf_->drawEnabled      = true;
    pSelf_->drawMember       = false;
    pSelf_->active           = false;
    pSelf_->activeTransition = false;

    if (pSelf_->pFont)
        GUM_IResource_ref(GUM_IRESOURCE(pSelf_->pFont));

    pSelf->shouldUpdate          = true;
    pSelf->isInteractive         = true;
    pSelf->isSelectable          = false;
    pSelf->isSelectedByDefault   = false;
    pSelf->x                     = 0.0f;
    pSelf->y                     = 0.0f;
    pSelf->w                     = 200.0f;
    pSelf->h                     = 200.0f;
    pSelf->isRelative            = false;
    pSelf->r                     = 0;
    pSelf->g                     = 255;
    pSelf->b                     = 0;
    pSelf->a                     = 255;
    pSelf->border_r              = 0;
    pSelf->border_g              = 0;
    pSelf->border_b              = 0;
    pSelf->border_a              = 0;
    pSelf->border_width          = 4;
    pSelf->border_radius         = 0.0f;
    pSelf->border_highlight      = false;
    pSelf->font_size             = 22;
    pSelf->font_r                = 255;
    pSelf->font_g                = 255;
    pSelf->font_b                = 255;
    pSelf->font_a                = 255;
    pSelf->font_border_r         = 0;
    pSelf->font_border_g         = 0;
    pSelf->font_border_b         = 0;
    pSelf->font_border_a         = 0;
    pSelf->font_border_thickness = 1;
    pSelf->textAlignment         = GUM_TEXT_ALIGN_CENTER;

    return GBL_RESULT_SUCCESS;
}

GUM_Rectangle GUM_Widget_clipRect_(const GUM_Widget* pWidget) {
    return pWidget ? GUM_WIDGET_(pWidget)->clipRect : GUM_CLIP_RECT_NONE_;
}

void GUM_Widget_setClipRect_(GUM_Widget* pWidget, GUM_Rectangle clip) {
    if (pWidget)
        GUM_WIDGET_(pWidget)->clipRect = clip;
}

void GUM_Widget_focusAcquire_(GUM_Widget* pWidget) {
    if (pWidget)
        ++GUM_WIDGET_(pWidget)->focusCount;
}

void GUM_Widget_focusRelease_(GUM_Widget* pWidget) {
    if (pWidget && GUM_WIDGET_(pWidget)->focusCount)
        --GUM_WIDGET_(pWidget)->focusCount;
}

void GUM_Widget_initActive_(GUM_Widget* pWidget, bool active) {
    if (pWidget)
        GUM_WIDGET_(pWidget)->active = active;
}

GBL_EXPORT GblBool GUM_Widget_isActive(const GUM_Widget* pSelf) {
    return pSelf && GUM_WIDGET_(pSelf)->active;
}

GBL_EXPORT GblBool GUM_Widget_isFocused(const GUM_Widget* pSelf) {
    return pSelf && GUM_WIDGET_(pSelf)->focusCount != 0;
}

GBL_EXPORT uint8_t GUM_Widget_zIndex(const GUM_Widget* pSelf) {
    return pSelf ? GUM_WIDGET_(pSelf)->zIndex : 0;
}

GBL_EXPORT GblStringRef* GUM_Widget_label(const GUM_Widget* pSelf) {
    return pSelf ? GUM_WIDGET_(pSelf)->pLabel : nullptr;
}

GBL_EXPORT GUM_Font* GUM_Widget_font(const GUM_Widget* pSelf) {
    return pSelf ? GUM_WIDGET_(pSelf)->pFont : nullptr;
}

GBL_EXPORT GUM_Texture* GUM_Widget_texture(const GUM_Widget* pSelf) {
    return pSelf ? GUM_WIDGET_(pSelf)->pTexture : nullptr;
}

static GBL_RESULT GUM_Widget_refreshContainer_(GblObject* pObject) {
    GUM_Container* pContainer = GBL_AS(GUM_Container, pObject);
    return pContainer ? GUM_CONTAINER_CLASSOF(pContainer)->pFnUpdateContent(pContainer)
                      : GBL_RESULT_SUCCESS;
}

static void GUM_Widget_reconcileDrawMembership_(GUM_Widget* pSelf) {
    GUM_Widget_* pSelf_ = GUM_WIDGET_(pSelf);
    GUM_Root* pRoot = GUM_Root_active_();
    const bool inActiveRoot = pRoot &&
        GblObject_findAncestorByType(GBL_OBJECT(pSelf), GUM_ROOT_TYPE) == GBL_OBJECT(pRoot);

    if (pSelf_->drawEnabled && inActiveRoot)
        GUM_Root_drawEnable_(pSelf);
    else
        GUM_Root_drawDisable_(pSelf);
}

static void GUM_Widget_reconcileDrawSubtree_(GblObject* pObject) {
    GUM_Widget* pWidget = GBL_AS(GUM_Widget, pObject);
    if (pWidget)
        GUM_Widget_reconcileDrawMembership_(pWidget);

    GblObject_foreachChild(pObject, pChild)
        GUM_Widget_reconcileDrawSubtree_(pChild);
}

static GBL_RESULT GUM_Widget_refreshHierarchyLayout_(GUM_Widget* pSelf,
                                                     GblObject* pOldParent) {
    // Layout callbacks may reparent the child; refresh its final parent.
    GBL_RESULT firstFailure = GUM_Widget_refreshContainer_(pOldParent);

    GblObject* pCurrentParent = GblObject_parent(GBL_OBJECT(pSelf));
    if (pCurrentParent)
        GblBox_ref(GBL_BOX(pCurrentParent));

    GUM_Container* pCurrentContainer = GBL_AS(GUM_Container, pCurrentParent);
    if (pCurrentContainer) {
        const GBL_RESULT result = GUM_CONTAINER_CLASSOF(pCurrentContainer)->pFnUpdateContent(pCurrentContainer);
        if (GBL_RESULT_SUCCESS(firstFailure) && !GBL_RESULT_SUCCESS(result))
            firstFailure = result;
        GblBox_unref(GBL_BOX(pCurrentParent));
        return firstFailure;
    }

    GUM_Widget_setClipRect_(pSelf, GUM_CLIP_RECT_NONE_);
    const GBL_RESULT result = GUM_Widget_refreshContainer_(GBL_OBJECT(pSelf));
    if (GBL_RESULT_SUCCESS(firstFailure) && !GBL_RESULT_SUCCESS(result))
        firstFailure = result;

    if (pCurrentParent)
        GblBox_unref(GBL_BOX(pCurrentParent));
    return firstFailure;
}

GBL_RESULT GUM_Widget_hierarchyChanged_(GUM_Widget* pSelf,
                                        GblObject* pOldParent,
                                        GblObject* pNewParent) {
    if (!pSelf || pOldParent == pNewParent)
        return GBL_RESULT_SUCCESS;

    // Parenting is already committed even if layout fails.
    const GBL_RESULT layoutResult = GUM_Widget_refreshHierarchyLayout_(pSelf, pOldParent);
    GUM_Widget_reconcileDrawSubtree_(GBL_OBJECT(pSelf));
    GUM_Root_drawOrderChanged_();
    return layoutResult;
}

GBL_RESULT GUM_Widget_drawSetEnabled_(GUM_Widget* pWidget, bool enabled) {
    if (!pWidget)
        return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_WIDGET_(pWidget)->drawEnabled = enabled;
    GUM_Widget_reconcileDrawMembership_(pWidget);
    return GBL_RESULT_SUCCESS;
}

typedef struct GUM_WidgetChildSnapshot_ {
    GblArrayList widgets;
    GUM_Widget*  stack[GUM_SNAPSHOT_INLINE_CAPACITY];
} GUM_WidgetChildSnapshot_;

static void GUM_Widget_childSnapshotRelease_(GUM_WidgetChildSnapshot_* pSnapshot) {
    const size_t count = GblArrayList_size(&pSnapshot->widgets);
    for (size_t i = 0; i < count; ++i) {
        GUM_Widget* pWidget = *(GUM_Widget**)GblArrayList_at(&pSnapshot->widgets, i);
        GblBox_unref(GBL_BOX(pWidget));
    }
    GblArrayList_destruct(&pSnapshot->widgets);
}

static GBL_RESULT GUM_Widget_childSnapshot_(GblObject* pParent,
                                            GUM_WidgetChildSnapshot_* pSnapshot) {
    GBL_RESULT result = GblArrayList_construct(&pSnapshot->widgets,
                                               sizeof(GUM_Widget*),
                                               0,
                                               nullptr,
                                               sizeof(*pSnapshot));
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
        return result;

    GblObject_foreachChild(pParent, pChild) {
        GUM_Widget* pWidget = GBL_AS(GUM_Widget, pChild);
        if (!pWidget)
            continue;

        GUM_Widget* pRetained = GUM_WIDGET(GblBox_ref(GBL_BOX(pWidget)));
        result = GblArrayList_pushBack(&pSnapshot->widgets, &pRetained);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
            GblBox_unref(GBL_BOX(pRetained));
            GUM_Widget_childSnapshotRelease_(pSnapshot);
            return result;
        }
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_Object_instantiated_(GblObject* pSelf) {
    GBL_RESULT firstFailure = GBL_RESULT_SUCCESS;

    if (!GblObject_parent(pSelf)) {
        GUM_Root* pRoot = GUM_Root_active_();
        if GBL_UNLIKELY (!pRoot) {
            GUM_LOG_ERROR("No root element found! Create one first.");
        } else {
            GblObject_setParent(pSelf, GBL_OBJECT(pRoot));
        }
    }

    GUM_Widget* pWidget = GUM_WIDGET(pSelf);
    firstFailure = GUM_Widget_hierarchyChanged_(pWidget,
                                                nullptr,
                                                GblObject_parent(pSelf));

    // Child layout callbacks may mutate the construction-time child list.
    GUM_WidgetChildSnapshot_ snapshot;
    GBL_RESULT result = GUM_Widget_childSnapshot_(pSelf, &snapshot);
    if GBL_LIKELY (GBL_RESULT_SUCCESS(result)) {
        const size_t count = GblArrayList_size(&snapshot.widgets);
        for (size_t i = 0; i < count; ++i) {
            GUM_Widget* pChildWidget = *(GUM_Widget**)GblArrayList_at(&snapshot.widgets, i);
            if (GblObject_parent(GBL_OBJECT(pChildWidget)) != pSelf)
                continue;

            result = GUM_Widget_hierarchyChanged_(pChildWidget, nullptr, pSelf);
            if (GBL_RESULT_SUCCESS(firstFailure) && !GBL_RESULT_SUCCESS(result))
                firstFailure = result;
        }
        GUM_Widget_childSnapshotRelease_(&snapshot);
    } else if (GBL_RESULT_SUCCESS(firstFailure)) {
        firstFailure = result;
    }

    // Preserve GblObject's lifecycle contract: construction-time property writes stay
    // silent until Widget setup is complete, then ordinary writes emit propertyChange.
    GblObjectClass* pObjectClass = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));
    result = pObjectClass->pFnInstantiated(pSelf);
    if (GBL_RESULT_SUCCESS(firstFailure) && !GBL_RESULT_SUCCESS(result))
        firstFailure = result;

    return firstFailure;
}

static GBL_RESULT GUM_Widget_activate_(GUM_Widget* pSelf) {
    GBL_UNUSED(pSelf);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_deactivate_(GUM_Widget* pSelf) {
    GBL_UNUSED(pSelf);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_setActive_(GUM_Widget* pSelf, bool requested) {
    GUM_Widget_* pSelf_ = GUM_WIDGET_(pSelf);

    if (requested == pSelf_->active)
        return GBL_RESULT_SUCCESS;

    if GBL_UNLIKELY (pSelf_->activeTransition) {
        GUM_LOG_WARN("Rejecting opposite Widget lifecycle transition during active callback.");
        return GBL_RESULT_ERROR_INVALID_OPERATION;
    }

    // Hooks and signals may release the final external Widget reference.
    GblBox_ref(GBL_BOX(pSelf));

    const bool previous = pSelf_->active;
    pSelf_->activeTransition = true;
    pSelf_->active = requested;

    GUM_WidgetClass* pClass = GUM_WIDGET_CLASSOF(pSelf);
    const GBL_RESULT hookResult = requested ?
        pClass->pFnActivate(pSelf) : pClass->pFnDeactivate(pSelf);

    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(hookResult)) {
        pSelf_->active = previous;
        pSelf_->activeTransition = false;
        GblBox_unref(GBL_BOX(pSelf));
        return hookResult;
    }

    // Signals observe the committed state.
    GBL_EMIT(pSelf, requested ? "onActivate" : "onDeactivate");
    pSelf_->activeTransition = false;

    GblBox_unref(GBL_BOX(pSelf));
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_GblObject_setProperty_(GblObject* pObject,
                                                    const GblProperty* pProp,
                                                    GblVariant* pValue) {
    GUM_Widget* pSelf = GUM_WIDGET(pObject);
    GUM_Widget_* pSelf_ = GUM_WIDGET_(pSelf);
    bool refreshLayout = false;

    switch (pProp->id) {
        case GUM_Widget_Property_Id_z_index: {
            const uint8_t zIndex = GblVariant_uint8(pValue);
            if (zIndex != pSelf_->zIndex) {
                pSelf_->zIndex = zIndex;
                GUM_Root_drawOrderChanged_();
            }
            break;
        }
        case GUM_Widget_Property_Id_x: {
            const float value = GblVariant_float(pValue);
            if (pSelf->x != value) {
                pSelf->x = value;
                refreshLayout = true;
            }
            break;
        }
        case GUM_Widget_Property_Id_y: {
            const float value = GblVariant_float(pValue);
            if (pSelf->y != value) {
                pSelf->y = value;
                refreshLayout = true;
            }
            break;
        }
        case GUM_Widget_Property_Id_w: {
            const float value = GblVariant_float(pValue);
            if (pSelf->w != value) {
                pSelf->w = value;
                refreshLayout = true;
            }
            break;
        }
        case GUM_Widget_Property_Id_h: {
            const float value = GblVariant_float(pValue);
            if (pSelf->h != value) {
                pSelf->h = value;
                refreshLayout = true;
            }
            break;
        }
        case GUM_Widget_Property_Id_isRelative: {
            const bool value = GblVariant_bool(pValue);
            if (pSelf->isRelative != value) {
                pSelf->isRelative = value;
                refreshLayout = true;
            }
            break;
        }
        case GUM_Widget_Property_Id_isInteractive:
            pSelf->isInteractive = GblVariant_bool(pValue);
            break;
        case GUM_Widget_Property_Id_isActive:
            return GUM_Widget_setActive_(pSelf, GblVariant_bool(pValue));
        case GUM_Widget_Property_Id_isSelectable:
            pSelf->isSelectable = GblVariant_bool(pValue);
            break;
        case GUM_Widget_Property_Id_isSelectedByDefault:
            pSelf->isSelectedByDefault = GblVariant_bool(pValue);
            break;
        case GUM_Widget_Property_Id_color: {
            const uint32_t color = GblVariant_uint32(pValue);
            pSelf->r = (color >> 24) & 0xFF;
            pSelf->g = (color >> 16) & 0xFF;
            pSelf->b = (color >> 8)  & 0xFF;
            pSelf->a = color & 0xFF;
            break;
        }
        case GUM_Widget_Property_Id_border_color: {
            const uint32_t color = GblVariant_uint32(pValue);
            pSelf->border_r = (color >> 24) & 0xFF;
            pSelf->border_g = (color >> 16) & 0xFF;
            pSelf->border_b = (color >> 8)  & 0xFF;
            pSelf->border_a = color & 0xFF;
            break;
        }
        case GUM_Widget_Property_Id_font_color: {
            const uint32_t color = GblVariant_uint32(pValue);
            pSelf->font_r = (color >> 24) & 0xFF;
            pSelf->font_g = (color >> 16) & 0xFF;
            pSelf->font_b = (color >> 8)  & 0xFF;
            pSelf->font_a = color & 0xFF;
            break;
        }
        case GUM_Widget_Property_Id_font_border_color: {
            const uint32_t color = GblVariant_uint32(pValue);
            pSelf->font_border_r = (color >> 24) & 0xFF;
            pSelf->font_border_g = (color >> 16) & 0xFF;
            pSelf->font_border_b = (color >> 8)  & 0xFF;
            pSelf->font_border_a = color & 0xFF;
            break;
        }
        case GUM_Widget_Property_Id_r:
            pSelf->r = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_g:
            pSelf->g = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_b:
            pSelf->b = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_a:
            pSelf->a = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_border_r:
            pSelf->border_r = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_border_g:
            pSelf->border_g = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_border_b:
            pSelf->border_b = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_border_a:
            pSelf->border_a = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_border_width: {
            const uint8_t value = GblVariant_uint8(pValue);
            if (pSelf->border_width != value) {
                pSelf->border_width = value;
                refreshLayout = true;
            }
            break;
        }
        case GUM_Widget_Property_Id_border_radius: {
            const float value = GBL_CLAMP(GblVariant_float(pValue), 0.0f, 1.0f);
            if (pSelf->border_radius != value) {
                pSelf->border_radius = value;
                refreshLayout = true;
            }
            break;
        }
        case GUM_Widget_Property_Id_border_highlight:
            pSelf->border_highlight = GblVariant_bool(pValue);
            break;
        case GUM_Widget_Property_Id_label:
        case GUM_Widget_Property_Id_labelAcquire:
            GblStringRef_unref(pSelf_->pLabel);
            pSelf_->pLabel = GblVariant_asString(pValue);
            break;
        case GUM_Widget_Property_Id_textAlignment:
            pSelf->textAlignment = GblVariant_enum(pValue);
            break;
        case GUM_Widget_Property_Id_font_size:
            pSelf->font_size = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_font_r:
            pSelf->font_r = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_font_g:
            pSelf->font_g = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_font_b:
            pSelf->font_b = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_font_a:
            pSelf->font_a = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_font_border_r:
            pSelf->font_border_r = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_font_border_g:
            pSelf->font_border_g = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_font_border_b:
            pSelf->font_border_b = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_font_border_a:
            pSelf->font_border_a = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_font_border_thickness:
            pSelf->font_border_thickness = GblVariant_uint8(pValue);
            break;
        case GUM_Widget_Property_Id_font: {
            if (pSelf_->pFont)
                GUM_IResource_unref(GUM_IRESOURCE(pSelf_->pFont));
            pSelf_->pFont = GUM_FONT(GblVariant_boxMove(pValue));
            break;
        }
        case GUM_Widget_Property_Id_texture: {
            if (pSelf_->pTexture)
                GUM_IResource_unref(GUM_IRESOURCE(pSelf_->pTexture));
            pSelf_->pTexture = GUM_TEXTURE(GblVariant_boxMove(pValue));
            break;
        }
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return refreshLayout ? GUM_Widget_refreshHierarchyLayout_(pSelf, nullptr)
                         : GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_GblObject_property_(const GblObject* pObject,
                                                 const GblProperty* pProp,
                                                 GblVariant* pValue) {
    const GUM_Widget* pSelf = GUM_WIDGET(pObject);
    const GUM_Widget_* pSelf_ = GUM_WIDGET_(pSelf);

    switch (pProp->id) {
        case GUM_Widget_Property_Id_z_index:
            GblVariant_setUint8(pValue, pSelf_->zIndex);
            break;
        case GUM_Widget_Property_Id_x:
            GblVariant_setFloat(pValue, pSelf->x);
            break;
        case GUM_Widget_Property_Id_y:
            GblVariant_setFloat(pValue, pSelf->y);
            break;
        case GUM_Widget_Property_Id_w:
            GblVariant_setFloat(pValue, pSelf->w);
            break;
        case GUM_Widget_Property_Id_h:
            GblVariant_setFloat(pValue, pSelf->h);
            break;
        case GUM_Widget_Property_Id_isRelative:
            GblVariant_setBool(pValue, pSelf->isRelative);
            break;
        case GUM_Widget_Property_Id_isInteractive:
            GblVariant_setBool(pValue, pSelf->isInteractive);
            break;
        case GUM_Widget_Property_Id_isActive:
            GblVariant_setBool(pValue, pSelf_->active);
            break;
        case GUM_Widget_Property_Id_isSelectable:
            GblVariant_setBool(pValue, pSelf->isSelectable);
            break;
        case GUM_Widget_Property_Id_isSelectedByDefault:
            GblVariant_setBool(pValue, pSelf->isSelectedByDefault);
            break;
        case GUM_Widget_Property_Id_color:
            GblVariant_setUint32(pValue,
                                 (uint32_t)pSelf->r << 24 |
                                 (uint32_t)pSelf->g << 16 |
                                 (uint32_t)pSelf->b << 8  |
                                 (uint32_t)pSelf->a);
            break;
        case GUM_Widget_Property_Id_border_color:
            GblVariant_setUint32(pValue,
                                 (uint32_t)pSelf->border_r << 24 |
                                 (uint32_t)pSelf->border_g << 16 |
                                 (uint32_t)pSelf->border_b << 8  |
                                 (uint32_t)pSelf->border_a);
            break;
        case GUM_Widget_Property_Id_font_color:
            GblVariant_setUint32(pValue,
                                 (uint32_t)pSelf->font_r << 24 |
                                 (uint32_t)pSelf->font_g << 16 |
                                 (uint32_t)pSelf->font_b << 8  |
                                 (uint32_t)pSelf->font_a);
            break;
        case GUM_Widget_Property_Id_font_border_color:
            GblVariant_setUint32(pValue,
                                 (uint32_t)pSelf->font_border_r << 24 |
                                 (uint32_t)pSelf->font_border_g << 16 |
                                 (uint32_t)pSelf->font_border_b << 8  |
                                 (uint32_t)pSelf->font_border_a);
            break;
        case GUM_Widget_Property_Id_r: GblVariant_setUint8(pValue, pSelf->r); break;
        case GUM_Widget_Property_Id_g: GblVariant_setUint8(pValue, pSelf->g); break;
        case GUM_Widget_Property_Id_b: GblVariant_setUint8(pValue, pSelf->b); break;
        case GUM_Widget_Property_Id_a: GblVariant_setUint8(pValue, pSelf->a); break;
        case GUM_Widget_Property_Id_border_r: GblVariant_setUint8(pValue, pSelf->border_r); break;
        case GUM_Widget_Property_Id_border_g: GblVariant_setUint8(pValue, pSelf->border_g); break;
        case GUM_Widget_Property_Id_border_b: GblVariant_setUint8(pValue, pSelf->border_b); break;
        case GUM_Widget_Property_Id_border_a: GblVariant_setUint8(pValue, pSelf->border_a); break;
        case GUM_Widget_Property_Id_border_width: GblVariant_setUint8(pValue, pSelf->border_width); break;
        case GUM_Widget_Property_Id_border_radius: GblVariant_setFloat(pValue, pSelf->border_radius); break;
        case GUM_Widget_Property_Id_border_highlight: GblVariant_setBool(pValue, pSelf->border_highlight); break;
        case GUM_Widget_Property_Id_label: GblVariant_setString(pValue, pSelf_->pLabel); break;
        case GUM_Widget_Property_Id_textAlignment: GblVariant_setEnum(pValue, GUM_TEXT_ALIGNMENT_TYPE, pSelf->textAlignment); break;
        case GUM_Widget_Property_Id_font_size: GblVariant_setUint8(pValue, pSelf->font_size); break;
        case GUM_Widget_Property_Id_font_r: GblVariant_setUint8(pValue, pSelf->font_r); break;
        case GUM_Widget_Property_Id_font_g: GblVariant_setUint8(pValue, pSelf->font_g); break;
        case GUM_Widget_Property_Id_font_b: GblVariant_setUint8(pValue, pSelf->font_b); break;
        case GUM_Widget_Property_Id_font_a: GblVariant_setUint8(pValue, pSelf->font_a); break;
        case GUM_Widget_Property_Id_font_border_r: GblVariant_setUint8(pValue, pSelf->font_border_r); break;
        case GUM_Widget_Property_Id_font_border_g: GblVariant_setUint8(pValue, pSelf->font_border_g); break;
        case GUM_Widget_Property_Id_font_border_b: GblVariant_setUint8(pValue, pSelf->font_border_b); break;
        case GUM_Widget_Property_Id_font_border_a: GblVariant_setUint8(pValue, pSelf->font_border_a); break;
        case GUM_Widget_Property_Id_font_border_thickness: GblVariant_setUint8(pValue, pSelf->font_border_thickness); break;
        case GUM_Widget_Property_Id_font:
            if (pSelf_->pFont) GblVariant_setBoxCopy(pValue, GBL_BOX(pSelf_->pFont));
            break;
        case GUM_Widget_Property_Id_texture:
            if (pSelf_->pTexture) GblVariant_setBoxCopy(pValue, GBL_BOX(pSelf_->pTexture));
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_update_(GUM_Widget* pSelf) {
    GBL_UNUSED(pSelf);
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_draw_(GUM_Widget* pSelf, GUM_Renderer* pRenderer) {
    GUM_Widget_* pSelf_ = GUM_WIDGET_(pSelf);
    const GUM_Vector2 position = GUM_get_absolute_position_(pSelf);
    GUM_Rectangle rec = { position.x, position.y, pSelf->w, pSelf->h };

    if (pSelf_->clipRect.x != GUM_CLIP_RECT_NONE_.x) {
        GUM_Rectangle overlap = GUM_Rectangle_intersect(pSelf_->clipRect, rec);
        if (overlap.width <= 0.0f || overlap.height <= 0.0f)
            return GBL_RESULT_SUCCESS;
    }

    const bool needsClip = pSelf_->clipRect.x != GUM_CLIP_RECT_NONE_.x ||
                           pSelf_->clipRect.width != GUM_CLIP_RECT_NONE_.width;
    bool scissorActive = false;
    GBL_RESULT result = GBL_RESULT_SUCCESS;

    if (needsClip) {
        result = GUM_Backend_beginScissor(pRenderer, pSelf_->clipRect);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            return result;
        scissorActive = true;
    }

    if (pSelf->a) {
        result = GUM_Backend_rectangleDraw(pRenderer, rec, pSelf->border_radius,
                                           (GUM_Color){ pSelf->r, pSelf->g, pSelf->b, pSelf->a });
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            goto cleanup;
    }

    if (pSelf->border_a) {
        result = GUM_Backend_rectangleLinesDraw(pRenderer, rec, pSelf->border_radius, pSelf->border_width,
                                                (GUM_Color){ pSelf->border_r, pSelf->border_g, pSelf->border_b, pSelf->border_a });
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            goto cleanup;

        if (pSelf->border_highlight) {
            const float inner_thickness = 1.0f;
            const float inset = (pSelf->border_width - inner_thickness) / 2.0f;
            const GUM_Rectangle inner = { rec.x - inset / 2, rec.y - inset / 2,
                                          rec.width + inset, rec.height + inset };
            result = GUM_Backend_rectangleLinesDraw(pRenderer, inner, pSelf->border_radius, inner_thickness,
                                                    (GUM_Color){ 255, 255, 255, 255 });
            if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                goto cleanup;
        }
    }

    GUM_Vector2 textSize = { 0, 0 };
    GUM_Vector2 textPos  = { 0, 0 };
    const float margin   = 3.0f;

    if (GblStringRef_length(pSelf_->pLabel)) {
        textSize = GUM_Backend_Font_measureText(pSelf_->pFont, pSelf_->pLabel, pSelf->font_size);

        switch (pSelf->textAlignment) {
            case GUM_TEXT_ALIGN_CENTER:
                textPos = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2,
                                         rec.y + rec.height / 2 - textSize.y / 2 };
                if (pSelf_->pTexture)
                    textPos = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2,
                                            rec.y + rec.height - textSize.y - margin };
                break;
            case GUM_TEXT_ALIGN_TOP:
                textPos = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2,
                                         rec.y + textSize.y / 2 + margin };
                break;
            case GUM_TEXT_ALIGN_RIGHT:
                textPos = (GUM_Vector2){ rec.x + rec.width - textSize.x - margin,
                                         rec.y + (rec.height - textSize.y) / 2 };
                break;
            case GUM_TEXT_ALIGN_BOTTOM:
                textPos = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2,
                                         rec.y + rec.height - textSize.y - margin };
                break;
            case GUM_TEXT_ALIGN_LEFT:
                textPos = (GUM_Vector2){ rec.x + margin,
                                         rec.y + (rec.height - pSelf->font_size) / 2 };
                break;
        }

        if (pSelf->font_border_a && pSelf->font_border_thickness) {
            for (int dx = -pSelf->font_border_thickness; dx <= pSelf->font_border_thickness; dx++) {
                for (int dy = -pSelf->font_border_thickness; dy <= pSelf->font_border_thickness; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    result = GUM_Backend_Font_draw(pRenderer, pSelf_->pFont, pSelf_->pLabel,
                                                  (GUM_Vector2){ .x = textPos.x + dx, .y = textPos.y + dy },
                                                  (GUM_Color){ pSelf->font_border_r, pSelf->font_border_g,
                                                               pSelf->font_border_b, pSelf->font_border_a },
                                                  pSelf->font_size, 1.2f);
                    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
                        goto cleanup;
                }
            }
        }

        result = GUM_Backend_Font_draw(pRenderer, pSelf_->pFont, pSelf_->pLabel,
                                       textPos,
                                       (GUM_Color){ pSelf->font_r, pSelf->font_g, pSelf->font_b, pSelf->font_a },
                                       pSelf->font_size, 1.2f);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result))
            goto cleanup;
    }

    if (pSelf_->pTexture) {
        GUM_Vector2 textureSize = { rec.width, rec.height };
        GUM_Vector2 texturePos = { rec.x, rec.y };

        if (GblStringRef_length(pSelf_->pLabel)) {
            textureSize.y *= 0.6f;
            textureSize.x *= 0.6f;
        }

        switch (pSelf->textAlignment) {
            case GUM_TEXT_ALIGN_CENTER:
            case GUM_TEXT_ALIGN_BOTTOM:
                texturePos = (GUM_Vector2){ rec.x + (rec.width - textureSize.x) / 2,
                                            rec.y + (rec.height - textureSize.y) / 2 - margin - textSize.y / 2 };
                break;
            case GUM_TEXT_ALIGN_TOP:
                texturePos = (GUM_Vector2){ rec.x + (rec.width - textureSize.x) / 2,
                                            rec.y + (rec.height - textureSize.y) / 2 + margin + textSize.y / 2 };
                break;
            case GUM_TEXT_ALIGN_LEFT:
                texturePos = (GUM_Vector2){ rec.x + (rec.width - textureSize.x) / 2 + margin + textSize.x / 2,
                                            rec.y + (rec.height - textureSize.y) / 2 };
                break;
            case GUM_TEXT_ALIGN_RIGHT:
                texturePos = (GUM_Vector2){ rec.x + (rec.width - textureSize.x) / 2 - margin - textSize.x / 2,
                                            rec.y + (rec.height - textureSize.y) / 2 };
                break;
        }

        const GUM_Rectangle textureRec = { texturePos.x, texturePos.y, textureSize.x, textureSize.y };
        result = GUM_Backend_Texture_draw(pRenderer, pSelf_->pTexture, textureRec,
                                          (GUM_Color){ 255, 255, 255, 255 });
    }

cleanup:
    if (scissorActive) {
        const GBL_RESULT endResult = GUM_Backend_endScissor(pRenderer);
        if (GBL_RESULT_SUCCESS(result) && !GBL_RESULT_SUCCESS(endResult))
            result = endResult;
    }

    return result;
}

constexpr int GUM_WIDGET_ANIMATE_MAX_ACTIVE_ = 64;

typedef struct {
    GUM_Widget*  pWidget;
    GblQuark     property;
    GUM_Animator animator;
    uint64_t     revision;
    bool         inUse;
} GUM_WidgetTween_;

static GUM_WidgetTween_ s_widgetTweens_[GUM_WIDGET_ANIMATE_MAX_ACTIVE_];
static uint64_t s_widgetTweenRevision_ = 0;

static GUM_WidgetTween_* GUM_Widget_findTween_(GUM_Widget* pWidget, GblQuark property) {
    for (size_t i = 0; i < GUM_WIDGET_ANIMATE_MAX_ACTIVE_; ++i) {
        GUM_WidgetTween_* pTween = &s_widgetTweens_[i];
        if (pTween->inUse && pTween->pWidget == pWidget && pTween->property == property)
            return pTween;
    }
    return nullptr;
}

static GUM_WidgetTween_* GUM_Widget_allocTween_(void) {
    for (size_t i = 0; i < GUM_WIDGET_ANIMATE_MAX_ACTIVE_; ++i)
        if (!s_widgetTweens_[i].inUse)
            return &s_widgetTweens_[i];

    GUM_LOG_ERROR("Ran out of widget tween slots! Bump GUM_WIDGET_ANIMATE_MAX_ACTIVE_.");
    return nullptr;
}

static void GUM_Widget_freeTween_(GUM_WidgetTween_* pTween) {
    GUM_Animator_setOnDone(&pTween->animator, nullptr);
    pTween->pWidget  = nullptr;
    pTween->property = GBL_QUARK_INVALID;
    pTween->inUse    = false;
}

static void GUM_Widget_applyTweenValue_(GUM_WidgetTween_* pTween) {
    if (!pTween || !pTween->inUse || !pTween->pWidget)
        return;

    // setProperty may destroy the Widget and cancel this tween.
    GUM_Widget* pWidget = GUM_WIDGET(GblBox_ref(GBL_BOX(pTween->pWidget)));
    const GblQuark property = pTween->property;
    const float current = pTween->animator.current;

    GBL_VARIANT(value);
    GblVariant_setFloat(&value, current);
    GblObject_setPropertyVariantByQuark(GBL_OBJECT(pWidget), property, &value);
    GblVariant_destruct(&value);
    GblBox_unref(GBL_BOX(pWidget));
}

static void GUM_Widget_animateStart_(GUM_Widget* pSelf, const char* pProperty, float target,
                                     float duration, GUM_EasingType easing, GUM_EasingFn pFnEase) {
    if (!pSelf || !pProperty || !pProperty[0] || easing < GUM_EASE_LINEAR || easing >= GUM_EASE_COUNT)
        return;
    if (easing == GUM_EASE_CUSTOM && !pFnEase)
        return;

    const GblQuark quark = GblQuark_fromString(pProperty);
    const GblProperty* pProp = GblProperty_findQuark(GBL_TYPEOF(pSelf), quark);
    if GBL_UNLIKELY (!pProp ||
                     (pProp->flags & GBL_PROPERTY_FLAG_READ_WRITE) != GBL_PROPERTY_FLAG_READ_WRITE ||
                     !GblVariant_canConvert(pProp->valueType, GBL_FLOAT_TYPE) ||
                     !GblVariant_canConvert(GBL_FLOAT_TYPE, pProp->valueType)) {
        return;
    }

    GUM_WidgetTween_* pTween = GUM_Widget_findTween_(pSelf, quark);

    if (!pTween) {
        pTween = GUM_Widget_allocTween_();
        if (!pTween) return;

        GBL_VARIANT(value);
        const GBL_RESULT propertyResult = GblObject_propertyVariantByQuark(GBL_OBJECT(pSelf), quark, &value);
        if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(propertyResult)) {
            GblVariant_destruct(&value);
            return;
        }

        const float current = GblVariant_toFloat(&value);
        GblVariant_destruct(&value);

        pTween->pWidget  = pSelf;
        pTween->property = quark;
        pTween->inUse    = true;
        pTween->animator = GUM_Animator_make(current, duration, easing);
    } else {
        GUM_Animator_setOnDone(&pTween->animator, nullptr);
        pTween->animator.duration = GBL_MAX(duration, 0.0f);
        pTween->animator.easing   = easing;
    }

    pTween->revision = ++s_widgetTweenRevision_;
    pTween->animator.pFnEase = pFnEase;
    GUM_Animator_set(&pTween->animator, target);

    // Keep settled tweens until the completion pass so callbacks can still attach.
    if (GUM_Animator_settled(&pTween->animator))
        GUM_Widget_applyTweenValue_(pTween);
}

GBL_EXPORT void GUM_Widget_animate(GUM_Widget* pSelf, const char* pProperty,
                                   float target, float duration, GUM_EasingType easing) {
    GUM_Widget_animateStart_(pSelf, pProperty, target, duration, easing, nullptr);
}

GBL_EXPORT void GUM_Widget_animateCustom(GUM_Widget* pSelf, const char* pProperty,
                                         float target, float duration, GUM_EasingFn pFnEase) {
    GUM_Widget_animateStart_(pSelf, pProperty, target, duration, GUM_EASE_CUSTOM, pFnEase);
}

static GBL_RESULT GUM_Widget_animateMarshal_(GblClosure* pClosure, GblVariant* pRetValue,
                                             size_t argCount, GblVariant* pArgs, GblPtr pMarshalData) {
    GBL_UNUSED(pRetValue, argCount, pArgs, pMarshalData);
    const GUM_Widget_doneFn pFnDone = (GUM_Widget_doneFn)GblCClosure_callback(GBL_C_CLOSURE(pClosure));
    pFnDone(GUM_WIDGET(GblBox_userdata(GBL_BOX(pClosure))));
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT void GUM_Widget_animateOnDone(GUM_Widget* pSelf, const char* pProperty, GUM_Widget_doneFn pFnDone) {
    if (!pSelf || !pProperty || !pProperty[0])
        return;

    GUM_WidgetTween_* pTween = GUM_Widget_findTween_(pSelf, GblQuark_fromString(pProperty));
    if (!pTween) return;

    if (!pFnDone) {
        GUM_Animator_setOnDone(&pTween->animator, nullptr);
        return;
    }

    GblClosure* pClosure = GBL_CLOSURE(GblCClosure_create((GblFnPtr)pFnDone, pSelf));
    if (!pClosure)
        return;
    GblClosure_setMarshal(pClosure, GUM_Widget_animateMarshal_);
    GUM_Animator_setOnDone(&pTween->animator, pClosure);
    GblClosure_unref(pClosure);
}

GBL_EXPORT void GUM_Widget_animateCancel(GUM_Widget* pSelf, const char* pProperty) {
    if (!pSelf || !pProperty || !pProperty[0])
        return;

    GUM_WidgetTween_* pTween = GUM_Widget_findTween_(pSelf, GblQuark_fromString(pProperty));
    if (pTween)
        GUM_Widget_freeTween_(pTween);
}

void GUM_Widget_animate_update_(void) {
    const float dt = GUM_Backend_frametime();

    for (size_t i = 0; i < GUM_WIDGET_ANIMATE_MAX_ACTIVE_; ++i) {
        GUM_WidgetTween_* pTween = &s_widgetTweens_[i];
        if (!pTween->inUse) continue;

        if (GUM_Animator_update(&pTween->animator, dt))
            GUM_Widget_applyTweenValue_(pTween);
    }

    for (size_t i = 0; i < GUM_WIDGET_ANIMATE_MAX_ACTIVE_; ++i) {
        GUM_WidgetTween_* pTween = &s_widgetTweens_[i];
        if (!pTween->inUse || !GUM_Animator_settled(&pTween->animator))
            continue;

        const uint64_t revision = pTween->revision;
        GblClosure* pOnDone = pTween->animator.pOnDone ?
            GblClosure_ref(pTween->animator.pOnDone) : nullptr;

        if (pOnDone) {
            GblClosure_invoke(pOnDone, nullptr, 0, nullptr);
            GblClosure_unref(pOnDone);
        }

        // Completion may cancel, retarget, or reuse this slot.
        if (pTween->inUse &&
            pTween->revision == revision &&
            GUM_Animator_settled(&pTween->animator)) {
            GUM_Widget_freeTween_(pTween);
        }
    }
}

void GUM_Widget_animate_widgetDestroyed_(GUM_Widget* pSelf) {
    for (size_t i = 0; i < GUM_WIDGET_ANIMATE_MAX_ACTIVE_; ++i) {
        GUM_WidgetTween_* pTween = &s_widgetTweens_[i];
        if (pTween->inUse && pTween->pWidget == pSelf)
            GUM_Widget_freeTween_(pTween);
    }
}

static GBL_RESULT GUM_Widget_GblBox_destructor_(GblBox* pBox) {
    GUM_Widget* pSelf = GUM_WIDGET(pBox);
    GUM_Widget_* pSelf_ = GUM_WIDGET_(pSelf);

    GUM_Root_drawDisable_(pSelf);
    GUM_InputSystem_widgetDestroyed_(pSelf);
    GUM_Widget_animate_widgetDestroyed_(pSelf);
    GblStringRef_unref(pSelf_->pLabel);

    if (pSelf_->pTexture)
        GUM_IResource_unref(GUM_IRESOURCE(pSelf_->pTexture));
    if (pSelf_->pFont)
        GUM_IResource_unref(GUM_IRESOURCE(pSelf_->pFont));

    GblObjectClass* pObjClass = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));
    return pObjClass->base.pFnDestructor(pBox);
}

static GBL_RESULT GUM_Widget_receiveEvent_(GblIEventReceiver* pSelf,
                                           GblIEventReceiver* pDest,
                                           GblEvent* pEvent) {
    GBL_UNUSED(pDest);
    GUM_Widget* pWidget = GUM_WIDGET(pSelf);
    GUM_WidgetClass* pClass = GUM_WIDGET_CLASSOF(pWidget);

    if (GblType_check(GBL_TYPEOF(pEvent), GUM_EVENT_INPUT_TYPE))
        return pClass->pFnInputEvent(pWidget, GUM_EVENT_INPUT(pEvent));

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_WidgetClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_WIDGET_TYPE)) {
        GBL_PROPERTIES_REGISTER(GUM_Widget);

        GblSignal_install(GUM_WIDGET_TYPE, "onPress",            GblMarshal_CClosure_VOID__INSTANCE_BOX, 1, GUM_EVENT_INPUT_TYPE);
        GblSignal_install(GUM_WIDGET_TYPE, "onPressConfirm",     GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onPressCancel",      GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onPressMoveUp",      GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onPressMoveDown",    GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onPressMoveLeft",    GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onPressMoveRight",   GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onPressUnbound",     GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onRelease",          GblMarshal_CClosure_VOID__INSTANCE_BOX, 1, GUM_EVENT_INPUT_TYPE);
        GblSignal_install(GUM_WIDGET_TYPE, "onReleaseConfirm",   GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onReleaseCancel",    GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onReleaseMoveUp",    GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onReleaseMoveDown",  GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onReleaseMoveLeft",  GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onReleaseMoveRight", GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onReleaseUnbound",   GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onActivate",         GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onDeactivate",       GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onFocusGained",      GblMarshal_CClosure_VOID__INSTANCE_BOX, 1, GUM_INPUTDEVICE_TYPE);
        GblSignal_install(GUM_WIDGET_TYPE, "onFocusLost",        GblMarshal_CClosure_VOID__INSTANCE_BOX, 1, GUM_INPUTDEVICE_TYPE);
    }

    GBL_IEVENT_RECEIVER_CLASS(pClass)->pFnReceiveEvent = GUM_Widget_receiveEvent_;
    GBL_OBJECT_CLASS(pClass)->pFnSetProperty  = GUM_Widget_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty     = GUM_Widget_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)->pFnInstantiated = GUM_Widget_Object_instantiated_;
    GBL_BOX_CLASS(pClass)->pFnDestructor = GUM_Widget_GblBox_destructor_;

    GUM_WIDGET_CLASS(pClass)->pFnActivate   = GUM_Widget_activate_;
    GUM_WIDGET_CLASS(pClass)->pFnDeactivate = GUM_Widget_deactivate_;
    GUM_WIDGET_CLASS(pClass)->pFnUpdate     = GUM_Widget_update_;
    GUM_WIDGET_CLASS(pClass)->pFnDraw       = GUM_Widget_draw_;
    GUM_WIDGET_CLASS(pClass)->pFnInputEvent = GUM_Widget_handleInputEvent_;

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_WidgetClass_final_(GblClass* pClass, const void* pClassData) {
    GBL_UNUSED(pClassData);

    if (!GblType_classRefCount(GUM_WIDGET_TYPE)) {
        GblProperty_uninstallAll(GUM_WIDGET_TYPE);

        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPress");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPressConfirm");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPressCancel");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPressMoveUp");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPressMoveDown");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPressMoveLeft");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPressMoveRight");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPressUnbound");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onRelease");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onReleaseConfirm");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onReleaseCancel");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onReleaseMoveUp");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onReleaseMoveDown");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onReleaseMoveLeft");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onReleaseMoveRight");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onReleaseUnbound");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onActivate");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onDeactivate");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onFocusGained");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onFocusLost");
    }

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Widget_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Widget"),
                                GBL_OBJECT_TYPE,
                                &(static GblTypeInfo){ .classSize           = sizeof(GUM_WidgetClass),
                                                       .pFnClassInit        = GUM_WidgetClass_init_,
                                                       .instanceSize        = sizeof(GUM_Widget),
                                                       .instancePrivateSize = sizeof(GUM_Widget_),
                                                       .pFnInstanceInit     = GUM_Widget_init_,
                                                       .pFnClassFinal       = GUM_WidgetClass_final_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

GUM_Vector2 GUM_get_absolute_position_(GUM_Widget* pWidget) {
    GUM_Vector2 pos = { pWidget->x, pWidget->y };
    if (!pWidget->isRelative)
        return pos;

    GblObject* pParent = GblObject_parent(GBL_OBJECT(pWidget));
    GUM_Widget* pParentWidget = GBL_AS(GUM_Widget, pParent);
    if (!pParentWidget)
        return pos;

    GUM_Container* pParentContainer = GBL_AS(GUM_Container, pParentWidget);
    if (pParentContainer && !GUM_Container_alignWidgets(pParentContainer))
        return pos;

    const GUM_Vector2 parentPos = GUM_get_absolute_position_(pParentWidget);
    pos.x += parentPos.x;
    pos.y += parentPos.y;
    return pos;
}

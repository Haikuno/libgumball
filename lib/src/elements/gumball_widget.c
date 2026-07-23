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

static void GUM_Widget_GblObject_onPropertyChange_(GblObject* pSelf, GblProperty* pProp) {
    switch (pProp->id) {
        case GUM_Widget_Property_Id_x:
        case GUM_Widget_Property_Id_y:
        case GUM_Widget_Property_Id_w:
        case GUM_Widget_Property_Id_h:
            GblObject* pParent = GblObject_parent(pSelf);
            GUM_CONTAINER_CLASSOF(pParent)->pFnUpdateContent(GUM_CONTAINER(pParent));
            break;
        default:
            break;
    }
}

static GBL_RESULT GUM_Widget_handleInputEvent_(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    if (!pSelf->isActive || !pEvent->state || !pEvent->action)
        return GBL_RESULT_SUCCESS;

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

    const char* signal;

    if (pEvent->state == GUM_INPUTSTATE_PRESS) {
        GBL_EMIT(pSelf, "onPress", pEvent);
        signal = pEvent->action == GUM_INPUTACTION_UNBOUND ? "onPressUnbound" : pressActionSignals_[pEvent->action];
    } else if (pEvent->state == GUM_INPUTSTATE_RELEASE) {
        GBL_EMIT(pSelf, "onRelease", pEvent);
        signal = pEvent->action == GUM_INPUTACTION_UNBOUND ? "onReleaseUnbound" : releaseActionSignals_[pEvent->action];
    }

    GBL_EMIT(pSelf, signal);
    GblEvent_accept(GBL_EVENT(pEvent));

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_init_(GblInstance* pInstance) {
    GUM_Widget* pSelf = GUM_WIDGET(pInstance);

    pSelf->z_index      = 50;
    pSelf->shouldUpdate = true;
    pSelf->focusCount   = 0;

    pSelf->isInteractive       = true;
    pSelf->isActive            = false;
    pSelf->isSelectable        = false;
    pSelf->isSelectedByDefault = false;

    pSelf->x          = 0.0f;
    pSelf->y          = 0.0f;
    pSelf->w          = 200.0f;
    pSelf->h          = 200.0f;
    pSelf->isRelative = false;
    pSelf->clipRect   = GUM_CLIP_RECT_NONE_;

    pSelf->r = 0;
    pSelf->g = 255;
    pSelf->b = 0;
    pSelf->a = 255;

    pSelf->border_r         = 0;
    pSelf->border_g         = 0;
    pSelf->border_b         = 0;
    pSelf->border_a         = 0;
    pSelf->border_width     = 4;
    pSelf->border_radius    = 0.0f;
    pSelf->border_highlight = false;

    pSelf->font                  = GUM_Backend_Font_default();
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

    pSelf->texture = nullptr;

    GUM_drawQueue_push(GBL_OBJECT(pInstance));

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_Object_instantiated_(GblObject* pSelf) {
    if (!GblObject_parent(pSelf)) {
        if GBL_LIKELY (GBL_TYPEOF(pSelf) != GUM_ROOT_TYPE) {
            static GUM_Root* pRoot = nullptr;

            GBL_REQUIRE_SCOPE(GUM_Root, &pRoot, "GUM_Root") {
                if GBL_UNLIKELY (!pRoot) {
                    GUM_LOG_ERROR("No root element found! Create one first.");
                    GBL_SCOPE_EXIT;
                }
                GblObject_setParent(pSelf, GBL_OBJECT(pRoot));
            }
        }
    } else {
        GblObject* pParent = GblObject_parent(pSelf);
        if (GBL_TYPEOF(pParent) == GUM_CONTAINER_TYPE) {
            GUM_CONTAINER_CLASSOF(pParent)->pFnUpdateContent(GUM_CONTAINER(pParent));
        }
    }

    // bump z-index of children (if any)
    GblObject_foreachChild(pSelf, pChild) {
        GUM_Widget* pWidget = GBL_AS(GUM_Widget, pChild);
        if (pWidget) pWidget->z_index++;
    }

    GUM_drawQueue_sort();
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_activate_(GUM_Widget* pSelf) {
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_deactivate_(GUM_Widget* pSelf) {
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_GblObject_setProperty_(GblObject* pObject, const GblProperty* pProp, GblVariant* pValue) {
    GUM_Widget* pSelf = GUM_WIDGET(pObject);
    switch (pProp->id) {
        case GUM_Widget_Property_Id_z_index:
            GblVariant_valueCopy(pValue, &pSelf->z_index);
            break;
        case GUM_Widget_Property_Id_x:
            GblVariant_valueCopy(pValue, &pSelf->x);
            break;
        case GUM_Widget_Property_Id_y:
            GblVariant_valueCopy(pValue, &pSelf->y);
            break;
        case GUM_Widget_Property_Id_w:
            GblVariant_valueCopy(pValue, &pSelf->w);
            break;
        case GUM_Widget_Property_Id_h:
            GblVariant_valueCopy(pValue, &pSelf->h);
            break;
        case GUM_Widget_Property_Id_isRelative:
            GblVariant_valueCopy(pValue, &pSelf->isRelative);
            break;
        case GUM_Widget_Property_Id_isInteractive:
            GblVariant_valueCopy(pValue, &pSelf->isInteractive);
            break;
        case GUM_Widget_Property_Id_isSelectable:
            GblVariant_valueCopy(pValue, &pSelf->isSelectable);
            break;
        case GUM_Widget_Property_Id_isSelectedByDefault:
            GblVariant_valueCopy(pValue, &pSelf->isSelectedByDefault);
            break;
        case GUM_Widget_Property_Id_color:
            uint32_t color_;
            GblVariant_valueCopy(pValue, &color_);
            pSelf->r = (color_ >> 24) & 0xFF;
            pSelf->g = (color_ >> 16) & 0xFF;
            pSelf->b = (color_ >> 8)  & 0xFF;
            pSelf->a = color_ & 0xFF;
            break;
        case GUM_Widget_Property_Id_border_color:
            uint32_t border_color_;
            GblVariant_valueCopy(pValue, &border_color_);
            pSelf->border_r = (border_color_ >> 24) & 0xFF;
            pSelf->border_g = (border_color_ >> 16) & 0xFF;
            pSelf->border_b = (border_color_ >> 8)  & 0xFF;
            pSelf->border_a =  border_color_        & 0xFF;
            break;
        case GUM_Widget_Property_Id_font_color:
            uint32_t font_color_;
            GblVariant_valueCopy(pValue, &font_color_);
            pSelf->font_r = (font_color_ >> 24) & 0XFF;
            pSelf->font_g = (font_color_ >> 16) & 0XFF;
            pSelf->font_b = (font_color_ >>  8) & 0XFF;
            pSelf->font_a =  font_color_        & 0XFF;
            break;
        case GUM_Widget_Property_Id_font_border_color:
            uint32_t font_border_color_;
            GblVariant_valueCopy(pValue, &font_border_color_);
            pSelf->font_border_r = (font_border_color_ >> 24) & 0xFF;
            pSelf->font_border_g = (font_border_color_ >> 16) & 0xFF;
            pSelf->font_border_b = (font_border_color_ >> 8)  & 0xFF;
            pSelf->font_border_a =  font_border_color_        & 0xFF;
            break;
        case GUM_Widget_Property_Id_r:
            GblVariant_valueCopy(pValue, &pSelf->r);
            break;
        case GUM_Widget_Property_Id_g:
            GblVariant_valueCopy(pValue, &pSelf->g);
            break;
        case GUM_Widget_Property_Id_b:
            GblVariant_valueCopy(pValue, &pSelf->b);
            break;
        case GUM_Widget_Property_Id_a:
            GblVariant_valueCopy(pValue, &pSelf->a);
            break;
        case GUM_Widget_Property_Id_border_r:
            GblVariant_valueCopy(pValue, &pSelf->border_r);
            break;
        case GUM_Widget_Property_Id_border_g:
            GblVariant_valueCopy(pValue, &pSelf->border_g);
            break;
        case GUM_Widget_Property_Id_border_b:
            GblVariant_valueCopy(pValue, &pSelf->border_b);
            break;
        case GUM_Widget_Property_Id_border_a:
            GblVariant_valueCopy(pValue, &pSelf->border_a);
            break;
        case GUM_Widget_Property_Id_border_width:
            GblVariant_valueCopy(pValue, &pSelf->border_width);
            break;
        case GUM_Widget_Property_Id_border_radius:
            pValue->f32 = GBL_CLAMP(pValue->f32, 0.0f, 1.0f);
            GblVariant_valueCopy(pValue, &pSelf->border_radius);
            break;
        case GUM_Widget_Property_Id_border_highlight:
            GblVariant_valueCopy(pValue, &pSelf->border_highlight);
            break;
        case GUM_Widget_Property_Id_label:
            GblStringRef_unref(pSelf->label);
            pSelf->label = GblVariant_asString(pValue);
            break;
        case GUM_Widget_Property_Id_labelAcquire:
            GblStringRef_unref(pSelf->label);
            pSelf->label = GblVariant_asString(pValue);
            break;
        case GUM_Widget_Property_Id_textAlignment:
            GblVariant_valueCopy(pValue, &pSelf->textAlignment);
            break;
        case GUM_Widget_Property_Id_font_size:
            GblVariant_valueCopy(pValue, &pSelf->font_size);
            break;
        case GUM_Widget_Property_Id_font_r:
            GblVariant_valueCopy(pValue, &pSelf->font_r);
            break;
        case GUM_Widget_Property_Id_font_g:
            GblVariant_valueCopy(pValue, &pSelf->font_g);
            break;
        case GUM_Widget_Property_Id_font_b:
            GblVariant_valueCopy(pValue, &pSelf->font_b);
            break;
        case GUM_Widget_Property_Id_font_a:
            GblVariant_valueCopy(pValue, &pSelf->font_a);
            break;
        case GUM_Widget_Property_Id_font_border_r:
            GblVariant_valueCopy(pValue, &pSelf->font_border_r);
            break;
        case GUM_Widget_Property_Id_font_border_g:
            GblVariant_valueCopy(pValue, &pSelf->font_border_g);
            break;
        case GUM_Widget_Property_Id_font_border_b:
            GblVariant_valueCopy(pValue, &pSelf->font_border_b);
            break;
        case GUM_Widget_Property_Id_font_border_a:
            GblVariant_valueCopy(pValue, &pSelf->font_border_a);
            break;
        case GUM_Widget_Property_Id_font_border_thickness:
            GblVariant_valueCopy(pValue, &pSelf->font_border_thickness);
            break;
        case GUM_Widget_Property_Id_font:
            GblBox* pFont = GblVariant_boxMove(pValue);
            pSelf->font   = GUM_FONT(pFont);
            break;
        case GUM_Widget_Property_Id_texture:
            GblBox* pTexture = GblVariant_boxMove(pValue);
            pSelf->texture   = GUM_TEXTURE(pTexture);
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_GblObject_property_(const GblObject* pObject, const GblProperty* pProp,
                                                 GblVariant* pValue) {
    GUM_Widget* pSelf = GUM_WIDGET(pObject);

    switch (pProp->id) {
        case GUM_Widget_Property_Id_z_index:
            GblVariant_setUint8(pValue, pSelf->z_index);
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
            GblVariant_setBool(pValue, pSelf->isActive);
            break;
        case GUM_Widget_Property_Id_isSelectable:
            GblVariant_setBool(pValue, pSelf->isSelectable);
            break;
        case GUM_Widget_Property_Id_isSelectedByDefault:
            GblVariant_setBool(pValue, pSelf->isSelectedByDefault);
            break;
        case GUM_Widget_Property_Id_color:
            GblVariant_setUint32(pValue, pSelf->r << 24 |
                                         pSelf->g << 16 |
                                         pSelf->b << 8  |
                                         pSelf->a);
            break;
        case GUM_Widget_Property_Id_border_color:
            GblVariant_setUint32(pValue, pSelf->border_r << 24 |
                                         pSelf->border_g << 16 |
                                         pSelf->border_b << 8  |
                                         pSelf->border_a);
            break;
        case GUM_Widget_Property_Id_font_color:
            GblVariant_setUint32(pValue, pSelf->font_r << 24 |
                                         pSelf->font_g << 16 |
                                         pSelf->font_b << 8  |
                                         pSelf->font_a);
            break;
        case GUM_Widget_Property_Id_font_border_color:
            GblVariant_setUint32(pValue, pSelf->font_border_r << 24 |
                                         pSelf->font_border_g << 16 |
                                         pSelf->font_border_b << 8  |
                                         pSelf->font_border_a);
            break;
        case GUM_Widget_Property_Id_r:
            GblVariant_setUint8(pValue, pSelf->r);
            break;
        case GUM_Widget_Property_Id_g:
            GblVariant_setUint8(pValue, pSelf->g);
            break;
        case GUM_Widget_Property_Id_b:
            GblVariant_setUint8(pValue, pSelf->b);
            break;
        case GUM_Widget_Property_Id_a:
            GblVariant_setUint8(pValue, pSelf->a);
            break;
        case GUM_Widget_Property_Id_border_r:
            GblVariant_setUint8(pValue, pSelf->border_r);
            break;
        case GUM_Widget_Property_Id_border_g:
            GblVariant_setUint8(pValue, pSelf->border_g);
            break;
        case GUM_Widget_Property_Id_border_b:
            GblVariant_setUint8(pValue, pSelf->border_b);
            break;
        case GUM_Widget_Property_Id_border_a:
            GblVariant_setUint8(pValue, pSelf->border_a);
            break;
        case GUM_Widget_Property_Id_border_width:
            GblVariant_setUint8(pValue, pSelf->border_width);
            break;
        case GUM_Widget_Property_Id_border_radius:
            GblVariant_setFloat(pValue, pSelf->border_radius);
            break;
        case GUM_Widget_Property_Id_border_highlight:
            GblVariant_setUint8(pValue, pSelf->border_highlight);
            break;
        case GUM_Widget_Property_Id_label:
            GblVariant_setString(pValue, pSelf->label);
            break;
        case GUM_Widget_Property_Id_textAlignment:
            GblVariant_setEnum(pValue, GUM_TEXT_ALIGNMENT_TYPE, pSelf->textAlignment);
            break;
        case GUM_Widget_Property_Id_font_size:
            GblVariant_setUint8(pValue, pSelf->font_size);
            break;
        case GUM_Widget_Property_Id_font_r:
            GblVariant_setUint8(pValue, pSelf->font_r);
            break;
        case GUM_Widget_Property_Id_font_g:
            GblVariant_setUint8(pValue, pSelf->font_g);
            break;
        case GUM_Widget_Property_Id_font_b:
            GblVariant_setUint8(pValue, pSelf->font_b);
            break;
        case GUM_Widget_Property_Id_font_a:
            GblVariant_setUint8(pValue, pSelf->font_a);
            break;
        case GUM_Widget_Property_Id_font_border_r:
            GblVariant_setUint8(pValue, pSelf->font_border_r);
            break;
        case GUM_Widget_Property_Id_font_border_g:
            GblVariant_setUint8(pValue, pSelf->font_border_g);
            break;
        case GUM_Widget_Property_Id_font_border_b:
            GblVariant_setUint8(pValue, pSelf->font_border_b);
            break;
        case GUM_Widget_Property_Id_font_border_a:
            GblVariant_setUint8(pValue, pSelf->font_border_a);
            break;
        case GUM_Widget_Property_Id_font_border_thickness:
            GblVariant_setUint8(pValue, pSelf->font_border_thickness);
            break;
        case GUM_Widget_Property_Id_font:
            GblVariant_setBoxCopy(pValue, GBL_BOX(pSelf->font));
            break;
        case GUM_Widget_Property_Id_texture:
            if (pSelf->texture) GblVariant_setBoxCopy(pValue, GBL_BOX(pSelf->texture));
            break;
        default:
            return GBL_RESULT_ERROR_INVALID_PROPERTY;
    }

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_update_(GUM_Widget* pSelf) {
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_draw_(GUM_Widget* pSelf, GUM_Renderer* pRenderer) {
    GUM_Rectangle rec     = (GUM_Rectangle){ pSelf->x, pSelf->y, pSelf->w, pSelf->h };
    GblObject*    pParent = GblObject_parent(GBL_OBJECT(pSelf));

    if (pParent && GBL_TYPEOF(pParent) != GUM_ROOT_TYPE && pSelf->isRelative) {
        GUM_Vector2 parent_pos  = GUM_get_absolute_position_(GUM_WIDGET(pParent));
        rec.x                  += parent_pos.x;
        rec.y                  += parent_pos.y;
    }

    const bool needsClip = pSelf->clipRect.x     != GUM_CLIP_RECT_NONE_.x ||
                           pSelf->clipRect.width != GUM_CLIP_RECT_NONE_.width;

    // Draw scope. Clips when needed
    GBL_SCOPE(needsClip ? GUM_Backend_beginScissor(pRenderer, pSelf->clipRect) : 0,
              needsClip ? GUM_Backend_endScissor(pRenderer) : 0) {

        if (pSelf->a) {
            GUM_Backend_rectangleDraw(pRenderer, rec, pSelf->border_radius,
                                    (GUM_Color){ pSelf->r, pSelf->g, pSelf->b, pSelf->a });
        }

        GUM_Button* pButton    = GBL_AS(GUM_Button, pSelf);

        if (pSelf->border_a) {
            GUM_Backend_rectangleLinesDraw(
                pRenderer, rec, pSelf->border_radius, pSelf->border_width,
                (GUM_Color){ pSelf->border_r, pSelf->border_g, pSelf->border_b, pSelf->border_a });

            if (pSelf->border_highlight) {
                float         inner_thickness = 1;
                float         inset           = (pSelf->border_width - inner_thickness) / 2.0f;
                GUM_Rectangle inner           = { rec.x - inset / 2, rec.y - inset / 2,
                                                rec.width + inset, rec.height + inset };

                GUM_Backend_rectangleLinesDraw(pRenderer, inner, pSelf->border_radius, inner_thickness,
                                            (GUM_Color){ 255, 255, 255, 255 });
            }
        }

        // text and texture rendering
        GUM_Vector2 textSize = { 0, 0 };
        GUM_Vector2 textPos  = { 0, 0 };
        const float margin   = 3.0f;

        if (GblStringRef_length(pSelf->label)) {
            textSize = GUM_Backend_Font_measureText(pSelf->font, pSelf->label, pSelf->font_size);

            switch (pSelf->textAlignment) {
                case GUM_TEXT_ALIGN_CENTER:
                    textPos
                        = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2, rec.y + rec.height / 2 - textSize.y / 2 };

                    // can't align to the center if there's a texture, so default to bottom
                    if (pSelf->texture) {
                        textPos = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2,
                                                rec.y + rec.height - textSize.y - margin };
                    }

                    break;
                case GUM_TEXT_ALIGN_TOP:
                    textPos = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2, rec.y + textSize.y / 2 + margin };
                    break;
                case GUM_TEXT_ALIGN_RIGHT:
                    textPos
                        = (GUM_Vector2){ rec.x + rec.width - textSize.x - margin, rec.y + (rec.height - textSize.y) / 2 };
                    break;
                case GUM_TEXT_ALIGN_BOTTOM:
                    textPos
                        = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2, rec.y + rec.height - textSize.y - margin };
                    break;
                case GUM_TEXT_ALIGN_LEFT:
                    textPos = (GUM_Vector2){ rec.x + margin, rec.y + (rec.height - pSelf->font_size) / 2 };
                    break;
            }

            // text border
            if (pSelf->font_border_a && pSelf->font_border_thickness) {
                for (int dx = -pSelf->font_border_thickness; dx <= pSelf->font_border_thickness; dx++) {
                    for (int dy = -pSelf->font_border_thickness; dy <= pSelf->font_border_thickness; dy++) {
                        if (dx == 0 && dy == 0) continue;
                        GUM_Backend_Font_draw(pRenderer, pSelf->font, pSelf->label,
                                            (GUM_Vector2){ .x = textPos.x + dx, .y = textPos.y + dy },
                                            (GUM_Color){ pSelf->font_border_r, pSelf->font_border_g, pSelf->font_border_b,
                                                        pSelf->font_border_a },
                                            pSelf->font_size, 1.2f);
                    }
                }
            }

            GUM_Backend_Font_draw(pRenderer, pSelf->font, pSelf->label, (GUM_Vector2){ .x = textPos.x, .y = textPos.y },
                                (GUM_Color){ pSelf->font_r, pSelf->font_g, pSelf->font_b, pSelf->font_a },
                                pSelf->font_size, 1.2f);
        }

        if (pSelf->texture) {
            GUM_Vector2 textureSize;
            GUM_Vector2 texturePos = { rec.x, rec.y };

            // adjust texture size based on pSelf size
            textureSize.x = rec.width;
            textureSize.y = rec.height;

            // if there is text, shrink it
            if (GblStringRef_length(pSelf->label)) {
                textureSize.y *= 0.6f;
                textureSize.x *= 0.6f;
            }

            // adjust texture position based on text size, position and alignment
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

            GUM_Rectangle rec = { texturePos.x, texturePos.y, textureSize.x, textureSize.y };

            GUM_Backend_Texture_draw(pRenderer, pSelf->texture, rec, (GUM_Color){ 255, 255, 255, 255 });
        }
    }

    return GBL_RESULT_SUCCESS;
}

// ===== Widget Animation =====

constexpr int GUM_WIDGET_ANIMATE_MAX_ACTIVE_ = 64; // TODO: grow dynamically if this ever isn't enough

typedef struct {
    GUM_Widget*  pWidget;
    GblQuark     property;
    GUM_Animator animator;
    bool         inUse;
} GUM_WidgetTween_;

static GUM_WidgetTween_ s_widgetTweens_[GUM_WIDGET_ANIMATE_MAX_ACTIVE_];

static GUM_WidgetTween_* GUM_Widget_findTween_(GUM_Widget* pWidget, GblQuark property) {
    for (size_t i = 0; i < GUM_WIDGET_ANIMATE_MAX_ACTIVE_; ++i) {
        GUM_WidgetTween_* pTween = &s_widgetTweens_[i];

        if (pTween->inUse && pTween->pWidget == pWidget && pTween->property == property)
            return pTween;
    }

    return nullptr;
}

static GUM_WidgetTween_* GUM_Widget_allocTween_(void) {
    for (size_t i = 0; i < GUM_WIDGET_ANIMATE_MAX_ACTIVE_; ++i) {
        if (!s_widgetTweens_[i].inUse)
            return &s_widgetTweens_[i];
    }

    GUM_LOG_ERROR("Ran out of widget tween slots! Bump GUM_WIDGET_ANIMATE_MAX_ACTIVE_.");
    return nullptr;
}

static void GUM_Widget_freeTween_(GUM_WidgetTween_* pTween) {
    GUM_Animator_setOnDone(&pTween->animator, nullptr); // releases any attached closure's ref
    pTween->inUse = false;
}

static void GUM_Widget_animateStart_(GUM_Widget* pSelf, const char* pProperty, float target,
                                     float duration, GUM_EasingType easing, GUM_EasingFn pFnEase) {
    const GblQuark    quark  = GblQuark_fromString(pProperty);
    GUM_WidgetTween_* pTween = GUM_Widget_findTween_(pSelf, quark);

    if (!pTween) {
        pTween = GUM_Widget_allocTween_();
        if (!pTween) return;

        GBL_VARIANT(value);
        GblObject_propertyVariantByQuark(GBL_OBJECT(pSelf), quark, &value);
        const float current = GblVariant_toFloat(&value); // converts byte properties (like alpha) via the registered converter table
        GblVariant_destruct(&value);

        pTween->pWidget  = pSelf;
        pTween->property = quark;
        pTween->inUse    = true;
        pTween->animator = GUM_Animator_make(current, duration, easing);
    } else {
        GUM_Animator_setOnDone(&pTween->animator, nullptr);
        pTween->animator.duration = duration;
        pTween->animator.easing   = easing;
    }

    pTween->animator.pFnEase = pFnEase;
    GUM_Animator_set(&pTween->animator, target);
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
    GUM_WidgetTween_* pTween = GUM_Widget_findTween_(pSelf, GblQuark_fromString(pProperty));
    if (!pTween) return;

    if (!pFnDone) {
        GUM_Animator_setOnDone(&pTween->animator, nullptr); // clears any existing callback
        return;
    }

    GblClosure* pClosure = GBL_CLOSURE(GblCClosure_create((GblFnPtr)pFnDone, pSelf));
    GblClosure_setMarshal(pClosure, GUM_Widget_animateMarshal_);
    GUM_Animator_setOnDone(&pTween->animator, pClosure);
    GblClosure_unref(pClosure); // the tween holds its own ref now
}

GBL_EXPORT void GUM_Widget_animateCancel(GUM_Widget* pSelf, const char* pProperty) {
    GUM_WidgetTween_* pTween = GUM_Widget_findTween_(pSelf, GblQuark_fromString(pProperty));

    if (pTween)
        GUM_Widget_freeTween_(pTween);
}

void GUM_Widget_animate_update_(void) {
    const float dt = GUM_Backend_frametime();

    for (size_t i = 0; i < GUM_WIDGET_ANIMATE_MAX_ACTIVE_; ++i) {
        GUM_WidgetTween_* pTween = &s_widgetTweens_[i];

        if (!pTween->inUse)
            continue;

        if (GUM_Animator_update(&pTween->animator, dt)) {
            GBL_VARIANT(value);
            GblVariant_setFloat(&value, pTween->animator.current);
            GblObject_setPropertyVariantByQuark(GBL_OBJECT(pTween->pWidget), pTween->property, &value);
            GblVariant_destruct(&value);
        }
    }

    // second pass to support callbacks destroying their own widgets
    for (size_t i = 0; i < GUM_WIDGET_ANIMATE_MAX_ACTIVE_; ++i) {
        GUM_WidgetTween_* pTween = &s_widgetTweens_[i];

        if (!pTween->inUse)
            continue;

        if (GUM_Animator_settled(&pTween->animator)) {
            if (pTween->animator.pOnDone)
                GblClosure_invoke(pTween->animator.pOnDone, nullptr, 0, nullptr);

            // check again to see if the callback started a new animation on the same property
            if (GUM_Animator_settled(&pTween->animator))
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

static GBL_RESULT GUM_Widget_postDraw_(GUM_Widget* pSelf, GUM_Renderer* pRenderer) {
    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_GblBox_destructor_(GblBox* pBox) {
    GUM_Widget* pSelf = GUM_WIDGET(pBox);

    GUM_InputSystem_widgetDestroyed(pSelf);
    GUM_Widget_animate_widgetDestroyed_(pSelf);
    GblStringRef_unref(pSelf->label);

    if (pSelf->texture)
        GUM_IResource_unref(GUM_IRESOURCE(pSelf->texture));

    if (pSelf->label && pSelf->font != GUM_Backend_Font_default())
        GUM_IResource_unref(GUM_IRESOURCE(pSelf->font));

    GblObjectClass* pObjClass = GBL_OBJECT_CLASS(GblClass_weakRefDefault(GBL_OBJECT_TYPE));
    return pObjClass->base.pFnDestructor(pBox);
}

static GBL_RESULT GUM_Widget_receiveEvent_(GblIEventReceiver* pSelf,
                                           GblIEventReceiver* pDest,
                                           GblEvent*          pEvent) {
    GUM_Widget*      pWidget = GUM_WIDGET(pSelf);
    GUM_WidgetClass* pClass  = GUM_WIDGET_CLASSOF(pWidget);

    if (GblType_check(GBL_TYPEOF(pEvent), GUM_EVENT_INPUT_TYPE))
        return pClass->pFnInputEvent(pWidget, GUM_EVENT_INPUT(pEvent));

    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_WidgetClass_init_(GblClass* pClass, const void* pData) {
    GBL_UNUSED(pData);

    if (!GblType_classRefCount(GUM_WIDGET_TYPE)) {
        GBL_PROPERTIES_REGISTER(GUM_Widget);

        GblSignal_install(GUM_WIDGET_TYPE, "onPress",          GblMarshal_CClosure_VOID__INSTANCE_BOX, 1, GUM_EVENT_INPUT_TYPE);
        GblSignal_install(GUM_WIDGET_TYPE, "onPressConfirm",   GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onPressCancel",    GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onPressUnbound",   GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onRelease",        GblMarshal_CClosure_VOID__INSTANCE_BOX, 1, GUM_EVENT_INPUT_TYPE);
        GblSignal_install(GUM_WIDGET_TYPE, "onReleaseConfirm", GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onReleaseCancel",  GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onReleaseUnbound", GblMarshal_CClosure_VOID__INSTANCE, 0);
        GblSignal_install(GUM_WIDGET_TYPE, "onFocusGained",    GblMarshal_CClosure_VOID__INSTANCE_BOX, 1, GUM_INPUTDEVICE_TYPE);
        GblSignal_install(GUM_WIDGET_TYPE, "onFocusLost",      GblMarshal_CClosure_VOID__INSTANCE_BOX, 1, GUM_INPUTDEVICE_TYPE);
    }

    GBL_IEVENT_RECEIVER_CLASS(pClass)->pFnReceiveEvent = GUM_Widget_receiveEvent_;

    GBL_OBJECT_CLASS(pClass)->pFnSetProperty  = GUM_Widget_GblObject_setProperty_;
    GBL_OBJECT_CLASS(pClass)->pFnProperty     = GUM_Widget_GblObject_property_;
    GBL_OBJECT_CLASS(pClass)->pFnInstantiated = GUM_Widget_Object_instantiated_;

    GBL_BOX_CLASS(pClass)->pFnDestructor      = GUM_Widget_GblBox_destructor_;

    GUM_WIDGET_CLASS(pClass)->pFnActivate     = GUM_Widget_activate_;
    GUM_WIDGET_CLASS(pClass)->pFnDeactivate   = GUM_Widget_deactivate_;
    GUM_WIDGET_CLASS(pClass)->pFnUpdate       = GUM_Widget_update_;
    GUM_WIDGET_CLASS(pClass)->pFnDraw         = GUM_Widget_draw_;
    GUM_WIDGET_CLASS(pClass)->pFnInputEvent   = GUM_Widget_handleInputEvent_;


    return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_WidgetClass_final_(GblClass* pClass, const void* pClassData) {
    GBL_UNUSED(pClassData);

    if (!GblType_classRefCount(GUM_WIDGET_TYPE)) {
        GblProperty_uninstallAll(GUM_WIDGET_TYPE);

        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPress"         );
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPressConfirm"  );
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPressCancel"   );
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onPressUnbound"  );
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onRelease"       );
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onReleaseConfirm");
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onReleaseCancel" );
        GblSignal_uninstall(GUM_WIDGET_TYPE, "onReleaseUnbound");
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
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_WidgetClass),
                                                       .pFnClassInit    = GUM_WidgetClass_init_,
                                                       .instanceSize    = sizeof(GUM_Widget),
                                                       .pFnInstanceInit = GUM_Widget_init_,
                                                       .pFnClassFinal   = GUM_WidgetClass_final_},
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

GUM_Vector2 GUM_get_absolute_position_(GUM_Widget* pWidget) {
    GblObject*     pParent          = GblObject_parent(GBL_OBJECT(pWidget));
    GUM_Container* pParentContainer = GBL_AS(GUM_Container, pParent);
    GUM_Vector2    pos              = { pWidget->x, pWidget->y };

    if (!pParent || !pWidget->isRelative) return pos;

    if (pParentContainer && !pParentContainer->alignWidgets) return pos;

    GUM_Vector2 parent_pos = GUM_get_absolute_position_(GUM_WIDGET(pParent));

    pos.x += parent_pos.x;
    pos.y += parent_pos.y;

    return pos;
}

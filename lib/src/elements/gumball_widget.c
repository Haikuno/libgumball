#include <gumball/elements/gumball_widget.h>
#include <gumball/elements/gumball_button.h>
#include <gumball/elements/gumball_container.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/elements/gumball_common.h>
#include <gumball/core/gumball_logger.h>
#include <gumball/core/gumball_backend.h>

// TODO: set root as parent after creation is parent prop is not set, to avoid doing it twice

static void GUM_Widget_GblObject_onPropertyChange_(GblObject *pSelf, GblProperty *pProp) {
	switch(pProp->id) {
		default:
			break;
	}
}

static GBL_RESULT GUM_Widget_init_(GblInstance *pInstance) {
	GUM_Widget *pSelf = GUM_WIDGET(pInstance);

	pSelf->z_index    = 50;

	pSelf->x 		  = 0;
	pSelf->y 		  = 0;
	pSelf->w 		  = 200;
	pSelf->h 		  = 200;
	pSelf->isRelative = false;

	pSelf->r = 0;
	pSelf->g = 255;
	pSelf->b = 0;
	pSelf->a = 255;

	pSelf->border_r			= 0;
	pSelf->border_g			= 0;
	pSelf->border_b			= 0;
	pSelf->border_a			= 0;
	pSelf->border_width		= 4;
	pSelf->border_radius	= 0.0f;
	pSelf->border_highlight = false;

	pSelf->font 				 = GUM_Backend_Font_default();
	pSelf->font_size 			 = 22;
	pSelf->font_r 				 = 255;
	pSelf->font_g 				 = 255;
	pSelf->font_b 				 = 255;
	pSelf->font_a 		 		 = 255;
	pSelf->font_border_r 		 = 0;
	pSelf->font_border_g 		 = 0;
	pSelf->font_border_b 		 = 0;
	pSelf->font_border_a 		 = 0;
	pSelf->font_border_thickness = 1;
	pSelf->textAlignment 	  	 = GUM_TEXT_ALIGN_CENTER;

	pSelf->texture = nullptr;

	GUM_drawQueue_push(GBL_OBJECT(pInstance));

	GBL_CONNECT(pInstance, "propertyChange", GUM_Widget_GblObject_onPropertyChange_);

	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_Object_instantiated_(GblObject *pSelf) {
	if(!GblObject_parent(pSelf)) {
		if GBL_LIKELY(GBL_TYPEOF(pSelf) != GUM_ROOT_TYPE) {

			static GUM_Root *pRoot = nullptr;

			GBL_REQUIRE_SCOPE(GUM_Root, &pRoot, "GUM_Root") {
				if (!pRoot) {
					GUM_LOG_ERROR("No root element found! Create one first.");
					return GBL_RESULT_ERROR;
				}
				GblObject_setParent(pSelf, GBL_OBJECT(pRoot));
			}

		}
	}

	// bump z-index of children (if any)
	GblObject *pChild = GblObject_childFirst(pSelf);
	if (!pChild)
		return GBL_RESULT_SUCCESS;

	while (pChild) {
		GUM_Widget *pWidget = GBL_AS(GUM_Widget, pChild);
		if (pWidget) pWidget->z_index++;
		pChild = GblObject_siblingNext(pChild);
	}

	GUM_drawQueue_sort();
	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_deactivate_(GUM_Widget *pSelf) {
	GblStringRef_unref(pSelf->label);
	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_GblObject_setProperty_(GblObject *pObject, const GblProperty *pProp, GblVariant *pValue) {
	GUM_Widget *pSelf = GUM_WIDGET(pObject);
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
		case GUM_Widget_Property_Id_color:
			uint32_t color_;
			GblVariant_valueCopy(pValue, &color_);
			pSelf->r = (color_ >> 24) & 0xFF;
			pSelf->g = (color_ >> 16) & 0xFF;
			pSelf->b = (color_ >> 8) & 0xFF;
			pSelf->a = color_ & 0xFF;
			break;
		case GUM_Widget_Property_Id_border_color:
			uint32_t border_color_;
			GblVariant_valueCopy(pValue, &border_color_);
			pSelf->border_r = (border_color_ >> 24) & 0xFF;
			pSelf->border_g = (border_color_ >> 16) & 0xFF;
			pSelf->border_b = (border_color_ >> 8) & 0xFF;
			pSelf->border_a = border_color_ & 0xFF;
			break;
		case GUM_Widget_Property_Id_font_border_color:
			uint32_t font_border_color_;
			GblVariant_valueCopy(pValue, &font_border_color_);
			pSelf->font_border_r = (font_border_color_ >> 24) & 0xFF;
			pSelf->font_border_g = (font_border_color_ >> 16) & 0xFF;
			pSelf->font_border_b = (font_border_color_ >> 8) & 0xFF;
			pSelf->font_border_a = font_border_color_ & 0xFF;
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
			GblVariant_valueCopy(pValue, &pSelf->border_radius);
			break;
		case GUM_Widget_Property_Id_border_highlight:
			GblVariant_valueCopy(pValue, &pSelf->border_highlight);
			break;
		case GUM_Widget_Property_Id_label:
			GblStringRef_unref(pSelf->label);
			pSelf->label = GblStringRef_create(GblVariant_string(pValue));
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
			GblBox *pFont = GblVariant_boxMove(pValue);
			pSelf->font = GUM_FONT(pFont);
			break;
		case GUM_Widget_Property_Id_texture:
			GblBox *pTexture = GblVariant_boxMove(pValue);
			pSelf->texture = GUM_TEXTURE(pTexture);
			break;
		default:
			return GBL_RESULT_ERROR_INVALID_PROPERTY;
	}

	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_GblObject_property_(const GblObject *pObject, const GblProperty *pProp, GblVariant *pValue) {
	GUM_Widget *pSelf = GUM_WIDGET(pObject);

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
		case GUM_Widget_Property_Id_color:
			GblVariant_setUint32(pValue, pSelf->r << 24 | pSelf->g << 16 | pSelf->b << 8 | pSelf->a);
			break;
		case GUM_Widget_Property_Id_border_color:
			GblVariant_setUint32(pValue, pSelf->border_r << 24 | pSelf->border_g << 16 | pSelf->border_b << 8 | pSelf->border_a);
			break;
		case GUM_Widget_Property_Id_font_border_color:
			GblVariant_setUint32(pValue, pSelf->font_border_r << 24 | pSelf->font_border_g << 16 | pSelf->font_border_b << 8 | pSelf->font_border_a);
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
			if (pSelf->texture)
				GblVariant_setBoxCopy(pValue, GBL_BOX(pSelf->texture));
			break;
		default:
			return GBL_RESULT_ERROR_INVALID_PROPERTY;
	}

	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_update_(GUM_Widget *pSelf) {
	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_Widget_draw_(GUM_Widget *pSelf, GUM_Renderer *pRenderer) {
	GUM_Rectangle rec = (GUM_Rectangle){ pSelf->x, pSelf->y, pSelf->w, pSelf->h };
	GUM_Widget *pParent = GBL_AS(GUM_Widget, GblObject_parent(GBL_OBJECT(pSelf)));

	if (pParent && pSelf->isRelative) {
		GUM_Vector2 parent_pos	= GUM_get_absolute_position_(GUM_WIDGET(pParent));
		rec.x				+= parent_pos.x;
		rec.y				+= parent_pos.y;
	}

	if (pSelf->a) {
		GUM_Backend_rectangleDraw(pRenderer, rec, pSelf->border_radius, (GUM_Color){ pSelf->r, pSelf->g, pSelf->b, pSelf->a });
	}

	GUM_Button *pButton = GBL_AS(GUM_Button, pSelf);

	bool isSelected = pButton && pButton->isSelected;

	if (pSelf->border_a && !isSelected) {
		GUM_Backend_rectangleLinesDraw(pRenderer, rec, pSelf->border_radius, pSelf->border_width, (GUM_Color){ pSelf->border_r, pSelf->border_g, pSelf->border_b, pSelf->border_a });

		if (pSelf->border_highlight) {
			float inner_thickness = 1;
			float inset = (pSelf->border_width - inner_thickness) / 2.0f;
			GUM_Rectangle inner = {
				rec.x - inset / 2,
				rec.y - inset / 2,
				rec.width  + inset,
				rec.height + inset
			};
			GUM_Backend_rectangleLinesDraw(pRenderer, inner, pSelf->border_radius, inner_thickness, (GUM_Color){ 255, 255, 255, 255 });
		}
	}

	// text and texture rendering
	GUM_Vector2 textSize	= { 0, 0 };
	GUM_Vector2 textPos		= { 0, 0 };
	const float margin	= 3.0f;

	if (GblStringRef_length(pSelf->label)) {
		textSize = GUM_Font_measureText(pSelf->font, pSelf->label, pSelf->font_size);

		switch (pSelf->textAlignment) {
			case GUM_TEXT_ALIGN_CENTER:
				textPos = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2, rec.y + rec.height / 2 - textSize.y / 2 };

				// can't align to the center if there's a texture, so default to bottom
				if (pSelf->texture) {
					textPos = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2, rec.y + rec.height - textSize.y - margin };
				}

				break;
			case GUM_TEXT_ALIGN_TOP:
				textPos = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2, rec.y + textSize.y / 2 + margin };
				break;
			case GUM_TEXT_ALIGN_RIGHT:
				textPos = (GUM_Vector2){ rec.x + rec.width - textSize.x - margin, rec.y + (rec.height - textSize.y) / 2 };
				break;
			case GUM_TEXT_ALIGN_BOTTOM:
				textPos = (GUM_Vector2){ rec.x + (rec.width - textSize.x) / 2, rec.y + rec.height - textSize.y - margin };
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
										  (GUM_Vector2){.x = textPos.x + dx, .y = textPos.y + dy},
		  								  (GUM_Color){ pSelf->font_border_r, pSelf->font_border_g, pSelf->font_border_b, pSelf->font_border_a },
										  pSelf->font_size, 1.2f);
				}
			}
		}

		GUM_Backend_Font_draw(pRenderer, pSelf->font, pSelf->label,
										  (GUM_Vector2){.x = textPos.x, .y = textPos.y},
		  								  (GUM_Color){ pSelf->font_r, pSelf->font_g, pSelf->font_b, pSelf->font_a },
										  pSelf->font_size, 1.2f);
	}


	if (pSelf->texture) {
		GUM_Vector2 textureSize;
		GUM_Vector2 texturePos		= { rec.x, rec.y };

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
				texturePos = (GUM_Vector2){ rec.x + (rec.width - textureSize.x) / 2, rec.y + (rec.height - textureSize.y) / 2 - margin - textSize.y / 2 };
				break;
			case GUM_TEXT_ALIGN_TOP:
				texturePos = (GUM_Vector2){ rec.x + (rec.width - textureSize.x) / 2, rec.y + (rec.height - textureSize.y) / 2 + margin + textSize.y / 2 };
				break;
			case GUM_TEXT_ALIGN_LEFT:
				texturePos = (GUM_Vector2){ rec.x + (rec.width - textureSize.x) / 2 + margin + textSize.x / 2, rec.y + (rec.height - textureSize.y) / 2 };
				break;
			case GUM_TEXT_ALIGN_RIGHT:
				texturePos = (GUM_Vector2){ rec.x + (rec.width - textureSize.x) / 2 - margin - textSize.x / 2, rec.y + (rec.height - textureSize.y) / 2 };
				break;
		}

		GUM_Rectangle rec = { texturePos.x, texturePos.y, textureSize.x, textureSize.y };

		GUM_Backend_Texture_draw(pRenderer, pSelf->texture, rec, (GUM_Color){ 255, 255, 255, 255 });
	}

	return GBL_RESULT_SUCCESS;
}

static GBL_RESULT GUM_WidgetClass_init_(GblClass *pClass, const void *pData) {
	GBL_UNUSED(pData);

	if (!GblType_classRefCount(GUM_WIDGET_TYPE)) GBL_PROPERTIES_REGISTER(GUM_Widget);

	GBL_OBJECT_CLASS(pClass)->pFnSetProperty  = GUM_Widget_GblObject_setProperty_;
	GBL_OBJECT_CLASS(pClass)->pFnProperty     = GUM_Widget_GblObject_property_;
	GBL_OBJECT_CLASS(pClass)->pFnInstantiated = GUM_Widget_Object_instantiated_;
	GUM_WIDGET_CLASS(pClass)->pFnActivate	  = nullptr;
	GUM_WIDGET_CLASS(pClass)->pFnDeactivate	  = GUM_Widget_deactivate_;
	GUM_WIDGET_CLASS(pClass)->pFnUpdate		  = GUM_Widget_update_;
	GUM_WIDGET_CLASS(pClass)->pFnDraw		  = GUM_Widget_draw_;

	return GBL_RESULT_SUCCESS;
}

GblType GUM_Widget_type(void) {
	static GblType type = GBL_INVALID_TYPE;

	if (type == GBL_INVALID_TYPE) {
		type =
			GblType_register(GblQuark_internStatic("GUM_Widget"),
							 GBL_OBJECT_TYPE,
							 &(static GblTypeInfo){.classSize = sizeof(GUM_WidgetClass),
												   .pFnClassInit = GUM_WidgetClass_init_,
												   .instanceSize = sizeof(GUM_Widget),
												   .pFnInstanceInit = GUM_Widget_init_},
							 GBL_TYPE_FLAG_TYPEINFO_STATIC);
	}

	return type;
}

GUM_Vector2 GUM_get_absolute_position_(GUM_Widget *pWidget) {
	GUM_Widget	*parent	= GBL_AS(GUM_Widget, GblObject_parent(GBL_OBJECT(pWidget)));
	GUM_Vector2		pos		= { pWidget->x, pWidget->y };

	if (!parent || !pWidget->isRelative) {
		return pos;
	}

	if (GBL_AS(GUM_Container, parent)) {
		if (!GBL_AS(GUM_Container, parent)->alignWidgets) return pos;
	}

	GUM_Vector2 parent_pos = GUM_get_absolute_position_(GUM_WIDGET(parent));

	pos.x += parent_pos.x;
	pos.y += parent_pos.y;

	return pos;
}

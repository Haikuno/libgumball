#include "gumball_itemview_.h"

#include <gumball/core/gumball_backend.h>

#include "gumball_widget_.h"

GBL_RESULT GUM_ItemView_drawText_(GUM_Widget* pWidget,
                                  GUM_Renderer* pRenderer,
                                  GUM_IItemModel* pModel,
                                  GUM_ModelIndex index,
                                  GUM_Vector2 position) {
    GBL_VARIANT(value);
    GBL_RESULT result = GUM_IItemModel_displayData(pModel, index, &value);
    if GBL_UNLIKELY (!GBL_RESULT_SUCCESS(result)) {
        GblVariant_destruct(&value);
        return result;
    }

    GBL_VARIANT(text);
    GblStringRef* pText = nullptr;
    const GblType valueType = GblVariant_typeOf(&value);

    if (GblType_check(valueType, GBL_STRING_TYPE)) {
        pText = GblVariant_string(&value);
    } else if (GblVariant_canConvert(valueType, GBL_STRING_TYPE)) {
        result = GblVariant_constructString(&text, "");
        if (GBL_RESULT_SUCCESS(result)) {
            result = GblVariant_convert(&value, &text);
            if (GBL_RESULT_SUCCESS(result))
                pText = GblVariant_string(&text);
        }
    } else {
        pText = (GblStringRef*)GblVariant_typeName(&value);
    }

    if (GBL_RESULT_SUCCESS(result) && pText) {
        result = GUM_Backend_Font_draw(pRenderer,
                                       GUM_Widget_font(pWidget),
                                       pText,
                                       position,
                                       (GUM_Color){ pWidget->font_r,
                                                    pWidget->font_g,
                                                    pWidget->font_b,
                                                    pWidget->font_a },
                                       pWidget->font_size,
                                       1.0f);
    }

    GblVariant_destruct(&text);
    GblVariant_destruct(&value);
    return result;
}

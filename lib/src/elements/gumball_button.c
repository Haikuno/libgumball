#include <gumball/elements/gumball_button.h>

#include "gumball_widget_.h"

static GBL_RESULT GUM_Button_init_(GblInstance* pInstance) {
    GUM_Widget* pWidget = GUM_WIDGET(pInstance);
    GUM_Widget_initActive_(pWidget, true);
    pWidget->isSelectable        = true;
    pWidget->isSelectedByDefault = false;
    return GBL_RESULT_SUCCESS;
}

GblType GUM_Button_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Button"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_ButtonClass),
                                                       .instanceSize    = sizeof(GUM_Button),
                                                       .pFnInstanceInit = GUM_Button_init_ },
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

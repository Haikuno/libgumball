#include <gumball/elements/gumball_button.h>
#include <gumball/elements/gumball_root.h>
#include <gumball/elements/gumball_common.h>

static GBL_RESULT GUM_Button_init_(GblInstance* pInstance) {
    GUM_Button* pButton = GUM_BUTTON(pInstance);

    GUM_WIDGET(pButton)->isActive            = true;
    GUM_WIDGET(pButton)->isSelectable        = true;
    GUM_WIDGET(pButton)->isSelectedByDefault = false;

    return GBL_RESULT_SUCCESS;
}

GblType GUM_Button_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblType_register(GblQuark_internStatic("GUM_Button"),
                                GUM_WIDGET_TYPE,
                                &(static GblTypeInfo){ .classSize       = sizeof(GUM_ButtonClass),
                                                       .instanceSize    = sizeof(GUM_Button),
                                                       .pFnInstanceInit = GUM_Button_init_},
                                GBL_TYPE_FLAG_TYPEINFO_STATIC);
    }

    return type;
}

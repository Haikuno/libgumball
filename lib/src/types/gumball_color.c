#include <gumball/types/gumball_color.h>

GblType GUM_Color_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblPointer_register("GUM_Color");
    }

    return type;
}

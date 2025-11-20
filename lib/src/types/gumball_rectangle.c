#include <gumball/types/gumball_rectangle.h>

GblType GUM_Rectangle_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblPointer_register("GUM_Rectangle");
    }

    return type;
}

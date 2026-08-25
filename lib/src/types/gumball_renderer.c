#include <gumball/types/gumball_renderer.h>

GblType GUM_Renderer_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE)
        type = GblPointer_register("GUM_Renderer");

    return type;
}

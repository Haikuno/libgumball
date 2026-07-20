#include <gumball/types/gumball_rectangle.h>

GUM_Rectangle GUM_Rectangle_intersect(GUM_Rectangle a, GUM_Rectangle b) {
    float x1 = GBL_MAX(a.x, b.x);
    float y1 = GBL_MAX(a.y, b.y);
    float x2 = GBL_MIN(a.x + a.width,  b.x + b.width);
    float y2 = GBL_MIN(a.y + a.height, b.y + b.height);

    GUM_Rectangle r = { x1, y1, x2 - x1, y2 - y1 };
    if (r.width  < 0.0f) r.width  = 0.0f;
    if (r.height < 0.0f) r.height = 0.0f;
    return r;
}

GblType GUM_Rectangle_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblPointer_register("GUM_Rectangle");
    }

    return type;
}

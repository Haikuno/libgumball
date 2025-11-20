#include <gumball/types/gumball_vector2.h>

GblType GUM_Vector2_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblPointer_register("GUM_vector2");
    }

    return type;
}

GUM_Vector2 GUM_Vector2_add(GUM_Vector2 a, GUM_Vector2 b) {
    return (GUM_Vector2) {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
}

GUM_Vector2 GUM_Vector2_subtract(GUM_Vector2 a, GUM_Vector2 b) {
    return (GUM_Vector2) {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
}

GUM_Vector2 GUM_Vector2_scale(GUM_Vector2 a, float scalar) {
    return (GUM_Vector2) {
        .x = a.x * scalar,
        .y = a.y * scalar
    };
}

float   GUM_Vector2_distance(GUM_Vector2 a, GUM_Vector2 b) {
    return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
float   GUM_Vector2_angle(GUM_Vector2 a, GUM_Vector2 b) {
    return atan2f(b.y - a.y, b.x - a.x);
}
#include <gumball/types/gumball_vector2.h>

GblType GUM_Vector2_type(void) {
    static GblType type = GBL_INVALID_TYPE;

    if (type == GBL_INVALID_TYPE) {
        type = GblPointer_register("GUM_vector2");
    }

    return type;
}

GBL_EXPORT GUM_Vector2 GUM_Vector2_add(GUM_Vector2 a, GUM_Vector2 b) {
    return (GUM_Vector2) {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
}

GBL_EXPORT GUM_Vector2 GUM_Vector2_subtract(GUM_Vector2 a, GUM_Vector2 b) {
    return (GUM_Vector2) {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
}

GBL_EXPORT GUM_Vector2 GUM_Vector2_scale(GUM_Vector2 a, float scalar) {
    return (GUM_Vector2) {
        .x = a.x * scalar,
        .y = a.y * scalar
    };
}

GBL_EXPORT float GUM_Vector2_distance(GUM_Vector2 from, GUM_Vector2 to) {
    float dx = from.x - to.x;
    float dy = from.y - to.y;
    return sqrtf(dx*dx + dy*dy);
}

GBL_EXPORT float GUM_Vector2_length(GUM_Vector2 a) {
    return sqrtf(a.x*a.x + a.y*a.y);
}

GBL_EXPORT float GUM_Vector2_dot(GUM_Vector2 a, GUM_Vector2 b) {
    return a.x * b.x + a.y * b.y;
}

GBL_EXPORT float GUM_Vector2_cross(GUM_Vector2 a, GUM_Vector2 b) {
    return a.x * b.y - a.y * b.x;
}

GBL_EXPORT float GUM_Vector2_angle(GUM_Vector2 from, GUM_Vector2 to) {
    float dx = to.x - from.x;
    float dy = to.y - from.y;
    return atan2f(dy, dx);
}

GBL_EXPORT float GUM_Vector2_angleTo(GUM_Vector2 from,
                          GUM_Vector2 to,
                          GUM_Vector2 dir)
{
    GUM_Vector2 target = GUM_Vector2_subtract(to, from);

    float dot = GUM_Vector2_dot(dir, target);
    float mag = GUM_Vector2_length(dir) * GUM_Vector2_length(target);

    if (mag == 0.0f) return 0.0f;

    float c = dot / mag;

    GBL_CLAMP(c, -1.0f, 1.0f);

    return acosf(c);     // returns 0->pi
}

GBL_EXPORT float GUM_Vector2_signedAngleTo(GUM_Vector2 from,
                                GUM_Vector2 to,
                                GUM_Vector2 dir)
{
    GUM_Vector2 target = GUM_Vector2_subtract(to, from);

    float cross = GUM_Vector2_cross(dir, target);
    float dot   = GUM_Vector2_dot(dir, target);

    return atan2f(cross, dot);   // returns −pi->pi
}

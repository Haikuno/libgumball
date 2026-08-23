#include <gumball/core/gumball_backend.h>
#include <raylib.h>
#include <math.h>

constexpr int GUM_RAYLIB_ROUNDED_SEGMENTS_ = 8;
constexpr int GUM_RAYLIB_CLIP_STACK_MAX_ = 16;

static GUM_Rectangle clipStack_[GUM_RAYLIB_CLIP_STACK_MAX_];
static uint8_t clipDepth_ = 0;

static void GUM_Raylib_beginScissor_(GUM_Rectangle clipRect) {
    const int x = (int)floorf(clipRect.x);
    const int y = (int)floorf(clipRect.y);
    const int right = (int)ceilf(clipRect.x + clipRect.width);
    const int bottom = (int)ceilf(clipRect.y + clipRect.height);
    BeginScissorMode(x, y, right - x, bottom - y);
}

void GUM_Raylib_Draw_deinit(void) {
    if (clipDepth_)
        EndScissorMode();
    clipDepth_ = 0;
}

GBL_EXPORT GUM_Vector2 GUM_Backend_screenSize(void) {
    return (GUM_Vector2){GetScreenWidth(), GetScreenHeight()};
}

GBL_EXPORT GBL_RESULT GUM_Backend_rectangleDraw(GUM_Renderer* pRenderer, GUM_Rectangle rectangle,
                                                float roundness, GUM_Color color) {
    GBL_UNUSED(pRenderer);

    Rectangle rec = { rectangle.x, rectangle.y, rectangle.width, rectangle.height };
    Color     col = { color.r, color.g, color.b, color.a };

    if (roundness <= 0.0f)
        DrawRectangle(rec.x, rec.y, rec.width, rec.height, col);
    else
        DrawRectangleRounded(rec, GBL_CLAMP(roundness, 0.0f, 1.0f),
                             GUM_RAYLIB_ROUNDED_SEGMENTS_, col);

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_Backend_rectangleLinesDraw(GUM_Renderer* pRenderer, GUM_Rectangle rectangle, float roundness,
                                                     float border_width, GUM_Color color) {
    GBL_UNUSED(pRenderer);
    if (border_width <= 0.0f) return GBL_RESULT_SUCCESS;

    Rectangle rec = { rectangle.x, rectangle.y, rectangle.width, rectangle.height };
    Color     col = { color.r, color.g, color.b, color.a };

    if (roundness <= 0.0f)
        DrawRectangleLinesEx(rec, border_width, col);
    else
        DrawRectangleRoundedLinesEx(rec, GBL_CLAMP(roundness, 0.0f, 1.0f),
                                    GUM_RAYLIB_ROUNDED_SEGMENTS_, border_width, col);

    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_Backend_beginScissor(GUM_Renderer* pRenderer, GUM_Rectangle clipRect) {
    GBL_UNUSED(pRenderer);
    if (clipDepth_ >= GUM_RAYLIB_CLIP_STACK_MAX_)
        return GBL_RESULT_ERROR_OUT_OF_RANGE;

    GUM_Raylib_beginScissor_(clipRect);
    clipStack_[clipDepth_++] = clipRect;
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_Backend_endScissor(GUM_Renderer* pRenderer) {
    GBL_UNUSED(pRenderer);
    if (!clipDepth_)
        return GBL_RESULT_ERROR_OUT_OF_RANGE;

    --clipDepth_;
    if (clipDepth_)
        GUM_Raylib_beginScissor_(clipStack_[clipDepth_ - 1]);
    else
        EndScissorMode();

    return GBL_RESULT_SUCCESS;
}

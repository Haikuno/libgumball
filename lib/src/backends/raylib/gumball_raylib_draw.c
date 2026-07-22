#include <gumball/core/gumball_backend.h>
#include <raylib.h>

constexpr int rounded_segments = 8;

GBL_EXPORT GUM_Vector2 GUM_Backend_screenSize(void) {
    return (GUM_Vector2){GetScreenWidth(), GetScreenHeight()};
}

GBL_EXPORT GBL_RESULT GUM_Backend_rectangleDraw(GUM_Renderer* pRenderer, GUM_Rectangle rectangle,
                                                float roundness, GUM_Color color) {
    DrawRectangleRounded((Rectangle){ rectangle.x, rectangle.y, rectangle.width, rectangle.height },
                         roundness, rounded_segments,
                         (Color)    { color.r, color.g, color.b, color.a });
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_Backend_rectangleLinesDraw(GUM_Renderer* pRenderer, GUM_Rectangle rectangle, float roundness,
                                                     float border_width, GUM_Color color) {
    DrawRectangleRoundedLinesEx((Rectangle){ rectangle.x, rectangle.y, rectangle.width, rectangle.height },
                                roundness, rounded_segments, border_width,
                                (Color)    { color.r, color.g, color.b, color.a });
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_Backend_beginScissor(GUM_Renderer* pRenderer, GUM_Rectangle clipRect) {
    BeginScissorMode((int)clipRect.x, (int)clipRect.y, (int)clipRect.width, (int)clipRect.height);
    return GBL_RESULT_SUCCESS;
}

GBL_EXPORT GBL_RESULT GUM_Backend_endScissor(GUM_Renderer* pRenderer) {
    EndScissorMode();
    return GBL_RESULT_SUCCESS;
}

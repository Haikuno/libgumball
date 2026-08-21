#include "gumball_sdl3_internal.h"
#include <gumball/core/gumball_backend.h>
#include <math.h>

constexpr int GUM_SDL3_ROUNDED_SEGMENTS_ = 8;
constexpr int GUM_SDL3_PERIMETER_POINTS_ = 4 * (GUM_SDL3_ROUNDED_SEGMENTS_ + 1);

static SDL_FColor GUM_SDL3_color_(GUM_Color color) {
    return (SDL_FColor){ color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };
}

static GBL_RESULT GUM_SDL3_result_(bool success) {
    return success ? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR_INTERNAL;
}

static float GUM_SDL3_radius_(GUM_Rectangle rectangle, float roundness) {
    return GBL_MIN(rectangle.width, rectangle.height) * 0.5f * GBL_CLAMP(roundness, 0.0f, 1.0f);
}

static int GUM_SDL3_perimeter_(GUM_Rectangle rectangle, float radius, SDL_FPoint* pPoints) {
    static const float startAngles[] = { -GBL_F_PI / 2.0f, 0.0f, GBL_F_PI / 2.0f, GBL_F_PI };
    const SDL_FPoint centers[] = {
        { rectangle.x + rectangle.width  - radius, rectangle.y + radius },
        { rectangle.x + rectangle.width  - radius, rectangle.y + rectangle.height - radius },
        { rectangle.x + radius, rectangle.y + rectangle.height - radius },
        { rectangle.x + radius, rectangle.y + radius }
    };

    int count = 0;
    for (int corner = 0; corner < 4; ++corner) {
        for (int segment = 0; segment <= GUM_SDL3_ROUNDED_SEGMENTS_; ++segment) {
            const float t     = (float)segment / GUM_SDL3_ROUNDED_SEGMENTS_;
            const float angle = startAngles[corner] + t * GBL_F_PI / 2.0f;
            pPoints[count++]  = (SDL_FPoint){ centers[corner].x + cosf(angle) * radius,
                                              centers[corner].y + sinf(angle) * radius };
        }
    }

    return count;
}

GBL_EXPORT GUM_Vector2 GUM_Backend_screenSize(void) {
    SDL_Renderer* pRenderer = GUM_SDL3_nativeRenderer_(nullptr);
    if (!pRenderer) return (GUM_Vector2){ 0 };

    int width  = 0;
    int height = 0;
    SDL_RendererLogicalPresentation mode = SDL_LOGICAL_PRESENTATION_DISABLED;

    if (SDL_GetRenderLogicalPresentation(pRenderer, &width, &height, &mode) &&
        mode != SDL_LOGICAL_PRESENTATION_DISABLED && width > 0 && height > 0)
        return (GUM_Vector2){ width, height };

    if (!SDL_GetCurrentRenderOutputSize(pRenderer, &width, &height))
        return (GUM_Vector2){ 0 };

    return (GUM_Vector2){ width, height };
}

GBL_EXPORT GBL_RESULT GUM_Backend_rectangleDraw(GUM_Renderer* pRenderer, GUM_Rectangle rectangle,
                                                float roundness, GUM_Color color) {
    SDL_Renderer* pSdlRenderer = GUM_SDL3_nativeRenderer_(pRenderer);
    if (!pSdlRenderer) return GBL_RESULT_ERROR_INVALID_POINTER;

    SDL_SetRenderDrawBlendMode(pSdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(pSdlRenderer, color.r, color.g, color.b, color.a);

    const float radius = GUM_SDL3_radius_(rectangle, roundness);
    if (radius <= 0.0f) {
        const SDL_FRect rect = { rectangle.x, rectangle.y, rectangle.width, rectangle.height };
        return GUM_SDL3_result_(SDL_RenderFillRect(pSdlRenderer, &rect));
    }

    SDL_FPoint points[GUM_SDL3_PERIMETER_POINTS_];
    const int count = GUM_SDL3_perimeter_(rectangle, radius, points);
    SDL_Vertex vertices[1 + GUM_SDL3_PERIMETER_POINTS_];
    int indices[GUM_SDL3_PERIMETER_POINTS_ * 3];
    const SDL_FColor vertexColor = GUM_SDL3_color_(color);

    vertices[0] = (SDL_Vertex){ .position = { rectangle.x + rectangle.width / 2.0f,
                                               rectangle.y + rectangle.height / 2.0f },
                                .color = vertexColor };

    for (int i = 0; i < count; ++i) {
        vertices[i + 1] = (SDL_Vertex){ .position = points[i], .color = vertexColor };
        indices[i * 3 + 0] = 0;
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = (i + 1) % count + 1;
    }

    return GUM_SDL3_result_(SDL_RenderGeometry(pSdlRenderer, nullptr, vertices, count + 1,
                                               indices, count * 3));
}

GBL_EXPORT GBL_RESULT GUM_Backend_rectangleLinesDraw(GUM_Renderer* pRenderer, GUM_Rectangle rectangle,
                                                     float roundness, float border_width, GUM_Color color) {
    SDL_Renderer* pSdlRenderer = GUM_SDL3_nativeRenderer_(pRenderer);
    if (!pSdlRenderer) return GBL_RESULT_ERROR_INVALID_POINTER;
    if (border_width <= 0.0f) return GBL_RESULT_SUCCESS;

    SDL_SetRenderDrawBlendMode(pSdlRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(pSdlRenderer, color.r, color.g, color.b, color.a);

    const float radius = GUM_SDL3_radius_(rectangle, roundness);
    if (radius <= 0.0f) {
        const float t = GBL_MIN(border_width, GBL_MIN(rectangle.width, rectangle.height) / 2.0f);
        const SDL_FRect rects[] = {
            { rectangle.x, rectangle.y, rectangle.width, t },
            { rectangle.x, rectangle.y + rectangle.height - t, rectangle.width, t },
            { rectangle.x, rectangle.y + t, t, rectangle.height - t * 2.0f },
            { rectangle.x + rectangle.width - t, rectangle.y + t, t, rectangle.height - t * 2.0f }
        };
        return GUM_SDL3_result_(SDL_RenderFillRects(pSdlRenderer, rects, 4));
    }

    GUM_Rectangle inner = { rectangle.x + border_width, rectangle.y + border_width,
                            rectangle.width - border_width * 2.0f, rectangle.height - border_width * 2.0f };

    if (inner.width <= 0.0f || inner.height <= 0.0f)
        return GUM_Backend_rectangleDraw(pRenderer, rectangle, roundness, color);

    SDL_FPoint outerPoints[GUM_SDL3_PERIMETER_POINTS_];
    SDL_FPoint innerPoints[GUM_SDL3_PERIMETER_POINTS_];
    const int count = GUM_SDL3_perimeter_(rectangle, radius, outerPoints);
    GUM_SDL3_perimeter_(inner, GBL_MAX(radius - border_width, 0.0f), innerPoints);

    SDL_Vertex vertices[GUM_SDL3_PERIMETER_POINTS_ * 2];
    int indices[GUM_SDL3_PERIMETER_POINTS_ * 6];
    const SDL_FColor vertexColor = GUM_SDL3_color_(color);

    for (int i = 0; i < count; ++i) {
        const int next = (i + 1) % count;
        vertices[i]         = (SDL_Vertex){ .position = outerPoints[i], .color = vertexColor };
        vertices[i + count] = (SDL_Vertex){ .position = innerPoints[i], .color = vertexColor };

        indices[i * 6 + 0] = i;
        indices[i * 6 + 1] = next;
        indices[i * 6 + 2] = i + count;
        indices[i * 6 + 3] = next;
        indices[i * 6 + 4] = next + count;
        indices[i * 6 + 5] = i + count;
    }

    return GUM_SDL3_result_(SDL_RenderGeometry(pSdlRenderer, nullptr, vertices, count * 2,
                                               indices, count * 6));
}

GBL_EXPORT GBL_RESULT GUM_Backend_beginScissor(GUM_Renderer* pRenderer, GUM_Rectangle clipRect) {
    SDL_Renderer* pSdlRenderer = GUM_SDL3_nativeRenderer_(pRenderer);
    if (!pSdlRenderer) return GBL_RESULT_ERROR_INVALID_POINTER;

    const SDL_Rect rect = { (int)floorf(clipRect.x), (int)floorf(clipRect.y),
                            (int)ceilf(clipRect.width), (int)ceilf(clipRect.height) };
    return GUM_SDL3_result_(SDL_SetRenderClipRect(pSdlRenderer, &rect));
}

GBL_EXPORT GBL_RESULT GUM_Backend_endScissor(GUM_Renderer* pRenderer) {
    SDL_Renderer* pSdlRenderer = GUM_SDL3_nativeRenderer_(pRenderer);
    if (!pSdlRenderer) return GBL_RESULT_ERROR_INVALID_POINTER;

    return GUM_SDL3_result_(SDL_SetRenderClipRect(pSdlRenderer, nullptr));
}

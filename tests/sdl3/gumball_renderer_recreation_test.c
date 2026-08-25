#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <gumball/gumball.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/types/gumball_texture.h>
#include <stdio.h>

static bool drawResources_(GUM_Renderer* pRenderer, GUM_Texture* pTexture, GUM_Font* pFont) {
    const GBL_RESULT textureResult = GUM_Backend_Texture_draw(
        pRenderer,
        pTexture,
        (GUM_Rectangle){ 0.0f, 0.0f, 32.0f, 32.0f },
        (GUM_Color){ 255, 255, 255, 255 });
    const GBL_RESULT fontResult = GUM_Backend_Font_draw(
        pRenderer,
        pFont,
        "restart",
        (GUM_Vector2){ 0.0f, 36.0f },
        (GUM_Color){ 255, 255, 255, 255 },
        12,
        0.0f);

    return textureResult == GBL_RESULT_SUCCESS && fontResult == GBL_RESULT_SUCCESS;
}

static bool rectEquals_(SDL_Rect a, SDL_Rect b) {
    return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
}

static bool rendererStateTransactions_(GUM_Renderer* pRenderer, SDL_Renderer* pSdlRenderer) {
    const SDL_BlendMode expectedBlend = SDL_BLENDMODE_NONE;
    const Uint8 expectedR = 17;
    const Uint8 expectedG = 34;
    const Uint8 expectedB = 51;
    const Uint8 expectedA = 68;

    if (!SDL_SetRenderDrawBlendMode(pSdlRenderer, expectedBlend) ||
        !SDL_SetRenderDrawColor(pSdlRenderer, expectedR, expectedG, expectedB, expectedA)) {
        return false;
    }

    if (GUM_Backend_rectangleDraw(pRenderer,
                                  (GUM_Rectangle){ 2.0f, 2.0f, 20.0f, 20.0f },
                                  0.25f,
                                  (GUM_Color){ 200, 150, 100, 220 }) != GBL_RESULT_SUCCESS ||
        GUM_Backend_rectangleLinesDraw(pRenderer,
                                       (GUM_Rectangle){ 4.0f, 4.0f, 24.0f, 24.0f },
                                       0.2f,
                                       2.0f,
                                       (GUM_Color){ 90, 100, 110, 210 }) != GBL_RESULT_SUCCESS) {
        return false;
    }

    SDL_BlendMode actualBlend;
    Uint8 actualR, actualG, actualB, actualA;
    if (!SDL_GetRenderDrawBlendMode(pSdlRenderer, &actualBlend) ||
        !SDL_GetRenderDrawColor(pSdlRenderer, &actualR, &actualG, &actualB, &actualA) ||
        actualBlend != expectedBlend ||
        actualR != expectedR || actualG != expectedG ||
        actualB != expectedB || actualA != expectedA) {
        return false;
    }

    const SDL_Rect original = { 1, 2, 40, 41 };
    const SDL_Rect outer = { 5, 6, 30, 31 };
    const SDL_Rect inner = { 9, 10, 20, 21 };
    SDL_Rect actual = { 0 };

    if (!SDL_SetRenderClipRect(pSdlRenderer, &original) ||
        GUM_Backend_beginScissor(pRenderer,
                                 (GUM_Rectangle){ outer.x, outer.y, outer.w, outer.h }) != GBL_RESULT_SUCCESS ||
        !SDL_GetRenderClipRect(pSdlRenderer, &actual) || !rectEquals_(actual, outer) ||
        GUM_Backend_beginScissor(pRenderer,
                                 (GUM_Rectangle){ inner.x, inner.y, inner.w, inner.h }) != GBL_RESULT_SUCCESS ||
        !SDL_GetRenderClipRect(pSdlRenderer, &actual) || !rectEquals_(actual, inner) ||
        GUM_Backend_endScissor(pRenderer) != GBL_RESULT_SUCCESS ||
        !SDL_GetRenderClipRect(pSdlRenderer, &actual) || !rectEquals_(actual, outer) ||
        GUM_Backend_endScissor(pRenderer) != GBL_RESULT_SUCCESS ||
        !SDL_GetRenderClipRect(pSdlRenderer, &actual) || !rectEquals_(actual, original)) {
        return false;
    }

    return SDL_SetRenderClipRect(pSdlRenderer, nullptr);
}

int main(void) {
    bool passed = false;
    GUM_Root* pRootA = nullptr;
    GUM_Root* pRootB = nullptr;
    GUM_Texture* pTexture = nullptr;
    GUM_Font* pFont = nullptr;
    SDL_Surface* pSurfaceA = nullptr;
    SDL_Surface* pSurfaceB = nullptr;
    SDL_Renderer* pSdlRendererA = nullptr;
    SDL_Renderer* pSdlRendererB = nullptr;
    GUM_Renderer* pRendererA = nullptr;
    GUM_Renderer* pRendererB = nullptr;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        goto cleanup;
    }

    pRootA = GUM_Root_create();
    pTexture = pRootA ? GUM_TEXTURE(GUM_Manager_load("koslogo.png")) : nullptr;
    pFont = pRootA ? GUM_FONT(GUM_Manager_load("gumball_test.ttf")) : nullptr;
    if (!pRootA || !pTexture || !pFont) {
        fprintf(stderr, "Failed to create first Root/load test resources: %s\n", SDL_GetError());
        goto cleanup;
    }

    const GUM_Vector2 originalSize = GUM_Texture_size(pTexture);
    if (originalSize.x <= 0 || originalSize.y <= 0) {
        fprintf(stderr, "Loaded texture has invalid dimensions\n");
        goto cleanup;
    }

    pSurfaceA = SDL_CreateSurface(64, 64, SDL_PIXELFORMAT_RGBA32);
    pSdlRendererA = pSurfaceA ? SDL_CreateSoftwareRenderer(pSurfaceA) : nullptr;
    pRendererA = pSdlRendererA ? GUM_Renderer_create(pSdlRendererA) : nullptr;
    if (!pRendererA ||
        !rendererStateTransactions_(pRendererA, pSdlRendererA) ||
        !drawResources_(pRendererA, pTexture, pFont)) {
        fprintf(stderr, "Initial SDL resource/state realization failed\n");
        goto cleanup;
    }

    GUM_Renderer_destroy(pRendererA);
    pRendererA = nullptr;
    SDL_DestroyRenderer(pSdlRendererA);
    pSdlRendererA = nullptr;
    SDL_DestroySurface(pSurfaceA);
    pSurfaceA = nullptr;

    GUM_unref(pRootA);
    pRootA = nullptr;

    const GUM_Vector2 afterBackendTeardown = GUM_Texture_size(pTexture);
    if (afterBackendTeardown.x != originalSize.x ||
        afterBackendTeardown.y != originalSize.y ||
        !TTF_WasInit()) {
        fprintf(stderr, "SDL resources did not survive Root/backend teardown correctly\n");
        goto cleanup;
    }

    pRootB = GUM_Root_create();
    pSurfaceB = SDL_CreateSurface(64, 64, SDL_PIXELFORMAT_RGBA32);
    pSdlRendererB = pSurfaceB ? SDL_CreateSoftwareRenderer(pSurfaceB) : nullptr;
    pRendererB = pSdlRendererB ? GUM_Renderer_create(pSdlRendererB) : nullptr;
    if (!pRootB || !pRendererB ||
        !drawResources_(pRendererB, pTexture, pFont)) {
        fprintf(stderr, "SDL resource use after Root/backend/renderer recreation failed\n");
        goto cleanup;
    }

    GUM_Renderer_destroy(pRendererB);
    pRendererB = nullptr;
    SDL_DestroyRenderer(pSdlRendererB);
    pSdlRendererB = nullptr;
    SDL_DestroySurface(pSurfaceB);
    pSurfaceB = nullptr;

    GUM_IResource_unref(GUM_IRESOURCE(pFont));
    pFont = nullptr;
    GUM_IResource_unref(GUM_IRESOURCE(pTexture));
    pTexture = nullptr;
    passed = true;

cleanup:
    if (pRendererB) GUM_Renderer_destroy(pRendererB);
    if (pRendererA) GUM_Renderer_destroy(pRendererA);
    if (pSdlRendererB) SDL_DestroyRenderer(pSdlRendererB);
    if (pSdlRendererA) SDL_DestroyRenderer(pSdlRendererA);
    if (pSurfaceB) SDL_DestroySurface(pSurfaceB);
    if (pSurfaceA) SDL_DestroySurface(pSurfaceA);
    if (pFont) GUM_IResource_unref(GUM_IRESOURCE(pFont));
    if (pTexture) GUM_IResource_unref(GUM_IRESOURCE(pTexture));
    if (pRootB) GUM_unref(pRootB);
    if (pRootA) GUM_unref(pRootA);

    SDL_Quit();
    return passed ? 0 : 1;
}
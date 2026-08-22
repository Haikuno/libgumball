#include <SDL3/SDL.h>
#include <gumball/backends/gumball_sdl3.h>
#include <gumball/core/gumball_backend.h>
#include <gumball/types/gumball_texture.h>
#include <gimbal/gimbal_strings.h>
#include <stdio.h>

static bool drawTexture_(GUM_Renderer* pRenderer, GUM_Texture* pTexture) {
    return GUM_Backend_Texture_draw(pRenderer,
                                    pTexture,
                                    (GUM_Rectangle){ 0.0f, 0.0f, 32.0f, 32.0f },
                                    (GUM_Color){ 255, 255, 255, 255 }) == GBL_RESULT_SUCCESS;
}

int main(void) {
    bool passed = false;
    bool textureLoaded = false;
    GblStringRef* pPath = nullptr;
    GUM_Texture* pTexture = nullptr;
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

    pTexture = GUM_TEXTURE(GblBox_create(GUM_TEXTURE_TYPE));
    pPath = GblStringRef_create("psyoplogo.png");
    if (!pTexture || !pPath ||
        GUM_Backend_Texture_load(GUM_IRESOURCE(pTexture), pPath) != GBL_RESULT_SUCCESS) {
        fprintf(stderr, "Failed to load test texture\n");
        goto cleanup;
    }
    textureLoaded = true;

    pSurfaceA = SDL_CreateSurface(64, 64, SDL_PIXELFORMAT_RGBA32);
    pSdlRendererA = pSurfaceA ? SDL_CreateSoftwareRenderer(pSurfaceA) : nullptr;
    pRendererA = pSdlRendererA ? GUM_SDL3_Renderer_create(pSdlRendererA) : nullptr;
    if (!pRendererA || !drawTexture_(pRendererA, pTexture) || !GUM_Texture_getTexture(pTexture)) {
        fprintf(stderr, "Initial texture realization failed\n");
        goto cleanup;
    }

    GUM_Renderer_destroy(pRendererA);
    pRendererA = nullptr;
    if (GUM_Texture_getTexture(pTexture)) {
        fprintf(stderr, "Texture cache survived renderer-wrapper destruction\n");
        goto cleanup;
    }

    SDL_DestroyRenderer(pSdlRendererA);
    pSdlRendererA = nullptr;
    SDL_DestroySurface(pSurfaceA);
    pSurfaceA = nullptr;

    pSurfaceB = SDL_CreateSurface(64, 64, SDL_PIXELFORMAT_RGBA32);
    pSdlRendererB = pSurfaceB ? SDL_CreateSoftwareRenderer(pSurfaceB) : nullptr;
    pRendererB = pSdlRendererB ? GUM_SDL3_Renderer_create(pSdlRendererB) : nullptr;
    if (!pRendererB || !drawTexture_(pRendererB, pTexture) || !GUM_Texture_getTexture(pTexture)) {
        fprintf(stderr, "Texture recreation on replacement renderer failed\n");
        goto cleanup;
    }

    GUM_Renderer_destroy(pRendererB);
    pRendererB = nullptr;
    if (GUM_Texture_getTexture(pTexture)) {
        fprintf(stderr, "Replacement renderer cache was not invalidated\n");
        goto cleanup;
    }

    passed = true;

cleanup:
    if (pRendererB) GUM_Renderer_destroy(pRendererB);
    if (pRendererA) GUM_Renderer_destroy(pRendererA);
    if (pSdlRendererB) SDL_DestroyRenderer(pSdlRendererB);
    if (pSdlRendererA) SDL_DestroyRenderer(pSdlRendererA);
    if (pSurfaceB) SDL_DestroySurface(pSurfaceB);
    if (pSurfaceA) SDL_DestroySurface(pSurfaceA);

    if (textureLoaded)
        GUM_Backend_Texture_unload(GUM_IRESOURCE(pTexture));
    if (pTexture)
        GBL_UNREF(pTexture);
    GblStringRef_unref(pPath);

    SDL_Quit();
    return passed ? 0 : 1;
}

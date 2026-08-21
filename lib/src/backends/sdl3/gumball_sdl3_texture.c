#include "gumball_sdl3_internal.h"
#include <SDL3_image/SDL_image.h>
#include <gumball/core/gumball_backend.h>
#include <stdlib.h>

typedef struct {
    SDL_Surface*  pSurface;
    SDL_Texture*  pTexture;
    SDL_Renderer* pRenderer;
} GUM_SDL3_Texture_;

GBL_EXPORT GUM_Vector2 GUM_Texture_size(GUM_Texture* pSelf) {
    GUM_Vector2 size = { 0 };
    GUM_SDL3_Texture_* pTexture = GUM_IResource_data(GUM_IRESOURCE(pSelf));

    if (pTexture && pTexture->pSurface)
        size = (GUM_Vector2){ pTexture->pSurface->w, pTexture->pSurface->h };

    return size;
}

GBL_EXPORT void* GUM_Texture_getTexture(GUM_Texture* pSelf) {
    GUM_SDL3_Texture_* pTexture = GUM_IResource_data(GUM_IRESOURCE(pSelf));
    return pTexture ? pTexture->pTexture : nullptr;
}

static SDL_Texture* GUM_SDL3_Texture_prepare_(GUM_SDL3_Texture_* pSelf, SDL_Renderer* pRenderer) {
    if (pSelf->pTexture && pSelf->pRenderer == pRenderer)
        return pSelf->pTexture;

    if (pSelf->pTexture)
        SDL_DestroyTexture(pSelf->pTexture);

    pSelf->pTexture  = SDL_CreateTextureFromSurface(pRenderer, pSelf->pSurface);
    pSelf->pRenderer = pSelf->pTexture ? pRenderer : nullptr;

    if (pSelf->pTexture)
        SDL_SetTextureBlendMode(pSelf->pTexture, SDL_BLENDMODE_BLEND);

    return pSelf->pTexture;
}

GBL_EXPORT GBL_RESULT GUM_Backend_Texture_draw(GUM_Renderer* pRenderer, GUM_Texture* pTexture,
                                               GUM_Rectangle rectangle, GUM_Color color) {
    SDL_Renderer* pSdlRenderer = GUM_SDL3_nativeRenderer_(pRenderer);
    if (!pSdlRenderer || !pTexture) return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_SDL3_Texture_* pData = GUM_IResource_data(GUM_IRESOURCE(pTexture));
    if (!pData || !GUM_SDL3_Texture_prepare_(pData, pSdlRenderer))
        return GBL_RESULT_ERROR_INTERNAL;

    SDL_SetTextureColorMod(pData->pTexture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(pData->pTexture, color.a);

    const SDL_FRect dst = { rectangle.x, rectangle.y, rectangle.width, rectangle.height };
    return SDL_RenderTexture(pSdlRenderer, pData->pTexture, nullptr, &dst)
        ? GBL_RESULT_SUCCESS
        : GBL_RESULT_ERROR_INTERNAL;
}

GBL_RESULT GUM_Backend_Texture_load(GUM_IResource* pSelf, GblStringRef* pPath) {
    if (!pSelf || !pPath) return GBL_RESULT_ERROR_INVALID_POINTER;

    SDL_Surface* pSurface = IMG_Load(pPath);
    if (!pSurface) return GBL_RESULT_ERROR_FILE_READ;

    GUM_SDL3_Texture_* pTexture = malloc(sizeof(*pTexture));
    if (!pTexture) {
        SDL_DestroySurface(pSurface);
        return GBL_RESULT_ERROR_MEM_ALLOC;
    }

    *pTexture = (GUM_SDL3_Texture_){ .pSurface = pSurface };
    GUM_IResource_setData(pSelf, pTexture);
    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Texture_unload(GUM_IResource* pSelf) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_SDL3_Texture_* pTexture = GUM_IResource_data(pSelf);
    if (!pTexture) return GBL_RESULT_SUCCESS;

    SDL_DestroyTexture(pTexture->pTexture);
    SDL_DestroySurface(pTexture->pSurface);
    free(pTexture);
    GUM_IResource_setData(pSelf, nullptr);

    return GBL_RESULT_SUCCESS;
}

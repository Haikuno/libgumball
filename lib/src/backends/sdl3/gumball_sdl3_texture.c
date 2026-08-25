#include "gumball_sdl3_internal.h"
#include "../../ifaces/gumball_iresource_.h"
#include <SDL3_image/SDL_image.h>
#include <gumball/core/gumball_backend.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef struct GUM_SDL3_Texture_ {
    SDL_Surface*              pSurface;
    SDL_Texture*              pTexture;
    SDL_Renderer*             pRenderer;
    struct GUM_SDL3_Texture_* pPrev;
    struct GUM_SDL3_Texture_* pNext;
} GUM_SDL3_Texture_;

static GUM_SDL3_Texture_* pTextures_ = nullptr;

static bool GUM_SDL3_suffixEqualsIgnoreCase_(const char* pPath, const char* pSuffix) {
    const size_t pathLen = strlen(pPath);
    const size_t suffixLen = strlen(pSuffix);
    if (suffixLen > pathLen)
        return false;

    const char* pCandidate = pPath + pathLen - suffixLen;
    for (size_t i = 0; i < suffixLen; ++i) {
        if (tolower((unsigned char)pCandidate[i]) != tolower((unsigned char)pSuffix[i]))
            return false;
    }
    return true;
}

GblType GUM_Backend_resourceType(GblStringRef* pPath) {
    if (!pPath || !pPath[0])
        return GBL_INVALID_TYPE;

    static const char* const textureExtensions[] = {
        ".bmp", ".gif", ".ico", ".cur", ".jpg", ".jpeg", ".lbm", ".pcx",
        ".png", ".pnm", ".qoi", ".svg", ".tga", ".tif", ".tiff", ".xcf",
        ".xpm", ".xv", ".webp", nullptr
    };
    static const char* const fontExtensions[] = {
        ".ttf", ".otf", nullptr
    };

    for (size_t i = 0; textureExtensions[i]; ++i) {
        if (GUM_SDL3_suffixEqualsIgnoreCase_(pPath, textureExtensions[i]))
            return GUM_TEXTURE_TYPE;
    }
    for (size_t i = 0; fontExtensions[i]; ++i) {
        if (GUM_SDL3_suffixEqualsIgnoreCase_(pPath, fontExtensions[i]))
            return GUM_FONT_TYPE;
    }
    return GBL_INVALID_TYPE;
}

static void GUM_SDL3_Texture_register_(GUM_SDL3_Texture_* pSelf) {
    pSelf->pPrev = nullptr;
    pSelf->pNext = pTextures_;
    if (pTextures_)
        pTextures_->pPrev = pSelf;
    pTextures_ = pSelf;
}

static void GUM_SDL3_Texture_unregister_(GUM_SDL3_Texture_* pSelf) {
    if (pSelf->pPrev)
        pSelf->pPrev->pNext = pSelf->pNext;
    else if (pTextures_ == pSelf)
        pTextures_ = pSelf->pNext;

    if (pSelf->pNext)
        pSelf->pNext->pPrev = pSelf->pPrev;

    pSelf->pPrev = nullptr;
    pSelf->pNext = nullptr;
}

void GUM_SDL3_Texture_rendererDestroyed_(SDL_Renderer* pRenderer) {
    if (!pRenderer) return;

    for (GUM_SDL3_Texture_* pTexture = pTextures_; pTexture; pTexture = pTexture->pNext) {
        if (pTexture->pRenderer != pRenderer)
            continue;

        SDL_DestroyTexture(pTexture->pTexture);
        pTexture->pTexture  = nullptr;
        pTexture->pRenderer = nullptr;
    }
}

GBL_EXPORT GUM_Vector2 GUM_Texture_size(GUM_Texture* pSelf) {
    GUM_Vector2 size = { 0 };
    GUM_SDL3_Texture_* pTexture = GUM_IResource_data_(GUM_IRESOURCE(pSelf));

    if (pTexture && pTexture->pSurface)
        size = (GUM_Vector2){ pTexture->pSurface->w, pTexture->pSurface->h };

    return size;
}

static SDL_Texture* GUM_SDL3_Texture_prepare_(GUM_SDL3_Texture_* pSelf, SDL_Renderer* pRenderer) {
    if (pSelf->pTexture && pSelf->pRenderer == pRenderer)
        return pSelf->pTexture;

    if (pSelf->pTexture)
        SDL_DestroyTexture(pSelf->pTexture);

    pSelf->pTexture  = SDL_CreateTextureFromSurface(pRenderer, pSelf->pSurface);
    pSelf->pRenderer = pSelf->pTexture ? pRenderer : nullptr;

    if (pSelf->pTexture && !SDL_SetTextureBlendMode(pSelf->pTexture, SDL_BLENDMODE_BLEND)) {
        SDL_DestroyTexture(pSelf->pTexture);
        pSelf->pTexture = nullptr;
        pSelf->pRenderer = nullptr;
    }

    return pSelf->pTexture;
}

GBL_EXPORT GBL_RESULT GUM_Backend_Texture_draw(GUM_Renderer* pRenderer, GUM_Texture* pTexture,
                                               GUM_Rectangle rectangle, GUM_Color color) {
    SDL_Renderer* pSdlRenderer = GUM_SDL3_nativeRenderer_(pRenderer);
    if (!pSdlRenderer || !pTexture) return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_SDL3_Texture_* pData = GUM_IResource_data_(GUM_IRESOURCE(pTexture));
    if (!pData || !GUM_SDL3_Texture_prepare_(pData, pSdlRenderer))
        return GBL_RESULT_ERROR_INTERNAL;

    if (!SDL_SetTextureColorMod(pData->pTexture, color.r, color.g, color.b) ||
        !SDL_SetTextureAlphaMod(pData->pTexture, color.a)) {
        return GBL_RESULT_ERROR_INTERNAL;
    }

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
    GUM_SDL3_Texture_register_(pTexture);
    GUM_IResource_setData_(pSelf, pTexture);
    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Texture_unload(GUM_IResource* pSelf) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    GUM_SDL3_Texture_* pTexture = GUM_IResource_data_(pSelf);
    if (!pTexture) return GBL_RESULT_SUCCESS;

    GUM_SDL3_Texture_unregister_(pTexture);
    SDL_DestroyTexture(pTexture->pTexture);
    SDL_DestroySurface(pTexture->pSurface);
    free(pTexture);
    GUM_IResource_setData_(pSelf, nullptr);

    return GBL_RESULT_SUCCESS;
}

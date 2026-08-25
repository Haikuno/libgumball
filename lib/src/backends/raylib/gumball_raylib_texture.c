#include "../../ifaces/gumball_iresource_.h"
#include <gumball/core/gumball_backend.h>
#include <raylib.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static bool GUM_Raylib_suffixEqualsIgnoreCase_(const char* pPath, const char* pSuffix) {
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
        ".png", ".bmp", ".tga", ".jpg", ".jpeg", ".gif", ".qoi", ".hdr",
        ".pic", ".psd", ".dds", ".ktx", ".ktx2", ".pkm", ".pvr", ".astc",
        nullptr
    };
    static const char* const fontExtensions[] = {
        ".ttf", ".otf", ".fnt", ".bdf", nullptr
    };

    for (size_t i = 0; textureExtensions[i]; ++i) {
        if (GUM_Raylib_suffixEqualsIgnoreCase_(pPath, textureExtensions[i]))
            return GUM_TEXTURE_TYPE;
    }
    for (size_t i = 0; fontExtensions[i]; ++i) {
        if (GUM_Raylib_suffixEqualsIgnoreCase_(pPath, fontExtensions[i]))
            return GUM_FONT_TYPE;
    }
    return GBL_INVALID_TYPE;
}

GBL_EXPORT GUM_Vector2 GUM_Texture_size(GUM_Texture* pSelf) {
    GUM_Vector2 size = { 0 };
    Texture2D* pTexture = GUM_IResource_data_(GUM_IRESOURCE(pSelf));

    if (pTexture) {
        size.x = (float)pTexture->width;
        size.y = (float)pTexture->height;
    }

    return size;
}

GBL_EXPORT GBL_RESULT GUM_Backend_Texture_draw(GUM_Renderer* pRenderer, GUM_Texture* pTexture,
                                               GUM_Rectangle rectangle, GUM_Color color) {
    if (!pTexture) return GBL_RESULT_ERROR_INVALID_POINTER;

    Texture2D* pRayTexture = GUM_IResource_data_(GUM_IRESOURCE(pTexture));
    if (!pRayTexture || pRayTexture->id == 0) return GBL_RESULT_ERROR_INTERNAL;

    GBL_UNUSED(pRenderer);
    Rectangle src = { 0, 0, (float)pRayTexture->width, (float)pRayTexture->height };
    Rectangle dst = { rectangle.x, rectangle.y, rectangle.width, rectangle.height };

    DrawTexturePro(*pRayTexture, src, dst, (Vector2){ 0, 0 }, 0.0f,
                   (Color){ color.r, color.g, color.b, color.a });

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Texture_load(GUM_IResource* pSelf, GblStringRef* pPath) {
    if (!pSelf || !pPath) return GBL_RESULT_ERROR_INVALID_POINTER;

    Texture2D* pTexture = malloc(sizeof(*pTexture));
    if (!pTexture) return GBL_RESULT_ERROR_MEM_ALLOC;

    const Texture2D loaded = LoadTexture(pPath);
    if (loaded.id == 0 || loaded.width <= 0 || loaded.height <= 0) {
        free(pTexture);
        return GBL_RESULT_ERROR_FILE_READ;
    }

    *pTexture = loaded;
    GUM_IResource_setData_(pSelf, pTexture);

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Texture_unload(GUM_IResource* pSelf) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    Texture2D* pTexture = GUM_IResource_data_(pSelf);
    if (!pTexture) return GBL_RESULT_SUCCESS;

    UnloadTexture(*pTexture);
    free(pTexture);
    GUM_IResource_setData_(pSelf, nullptr);

    return GBL_RESULT_SUCCESS;
}

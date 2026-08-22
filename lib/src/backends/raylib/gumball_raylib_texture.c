#include <gumball/core/gumball_backend.h>
#include <raylib.h>

GBL_EXPORT GUM_Vector2 GUM_Texture_size(GUM_Texture* pSelf) {
    GUM_Vector2 size = { 0 };
    Texture2D*  pTexture = (Texture2D*)GUM_IResource_data(GUM_IRESOURCE(pSelf));

    if (pTexture) {
        size.x = (float)pTexture->width;
        size.y = (float)pTexture->height;
    }

    return size;
}

GBL_EXPORT void* GUM_Texture_getTexture(GUM_Texture* pSelf) {
    return GUM_IResource_data(GUM_IRESOURCE(pSelf));
}

GBL_EXPORT GBL_RESULT GUM_Backend_Texture_draw(GUM_Renderer* pRenderer, GUM_Texture* pTexture,
                                               GUM_Rectangle rectangle, GUM_Color color) {
    if (!pTexture) return GBL_RESULT_ERROR_INVALID_POINTER;

    Texture2D* pRayTexture = GUM_IResource_data(GUM_IRESOURCE(pTexture));
    if (!pRayTexture || pRayTexture->id == 0) return GBL_RESULT_ERROR_INTERNAL;

    Rectangle src = { 0, 0, (float)pRayTexture->width, (float)pRayTexture->height };
    Rectangle dst = { rectangle.x, rectangle.y, rectangle.width, rectangle.height };

    DrawTexturePro(*pRayTexture, src, dst, (Vector2){ 0, 0 }, 0.0f, (Color){ color.r, color.g, color.b, color.a });

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Texture_load(GUM_IResource* pSelf, GblStringRef* path) {
    if (!pSelf || !path) return GBL_RESULT_ERROR_INVALID_POINTER;

    Texture2D* pTexture = malloc(sizeof(*pTexture));
    if (!pTexture) return GBL_RESULT_ERROR_MEM_ALLOC;

    const Texture2D loaded = LoadTexture(path);
    if (loaded.id == 0 || loaded.width <= 0 || loaded.height <= 0) {
        free(pTexture);
        return GBL_RESULT_ERROR_FILE_READ;
    }

    *pTexture = loaded;
    GUM_IRESOURCE_CLASSOF(pSelf)->pFnSetValue(pSelf, pTexture);

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Texture_unload(GUM_IResource* pSelf) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    Texture2D* pTexture = GUM_IResource_data(pSelf);
    if (!pTexture) return GBL_RESULT_SUCCESS;

    UnloadTexture(*pTexture);
    free(pTexture);
    GUM_IResource_setData(pSelf, nullptr);

    return GBL_RESULT_SUCCESS;
}

#include <gumball/core/gumball_backend.h>
#include <raylib.h>

GBL_EXPORT GUM_Vector2 GUM_Texture_size(GUM_Texture* pSelf) {
    GUM_Vector2 size;
    Texture2D*  pTexture = (Texture2D*)GUM_IResource_data(GUM_IRESOURCE(pSelf));

    if (pTexture) {
        size.x = (float)pTexture->width;
        size.y = (float)pTexture->height;
    }

    return size;
}

GBL_EXPORT GBL_RESULT GUM_Backend_Texture_draw(GUM_Renderer* pRenderer, GUM_Texture* pTexture,
                                               GUM_Rectangle rectangle, GUM_Color color) {
    if (!pTexture) return GBL_RESULT_ERROR_INVALID_POINTER;

    Texture2D texture = *(Texture2D*)GUM_IResource_data(GUM_IRESOURCE(pTexture));
    Rectangle src     = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle dst     = { rectangle.x, rectangle.y, rectangle.width, rectangle.height };

    DrawTexturePro(texture, src, dst, (Vector2){ 0, 0 }, 0.0f, (Color){ 255, 255, 255, 255 });

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Texture_load(GUM_IResource* pSelf, GblStringRef* path) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    Texture2D texture = LoadTexture(path);

    void* pTexture = malloc(sizeof(Texture2D));
    if (!pTexture) return GBL_RESULT_ERROR_MEM_ALLOC;

    memcpy(pTexture, &texture, sizeof(Texture2D));

    GUM_IRESOURCE_CLASSOF(pSelf)->pFnSetValue(pSelf, (void*)pTexture);

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Texture_unload(GUM_IResource* pSelf) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    void* pTexture = (Texture2D*)GUM_IResource_data(pSelf);

    UnloadTexture(*(Texture2D*)pTexture);
    free(pTexture);

    return GBL_RESULT_SUCCESS;
}

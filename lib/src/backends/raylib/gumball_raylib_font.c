#include <gumball/core/gumball_backend.h>
#include <raylib.h>

static GUM_Font* defaultFont_ = nullptr;

GBL_EXPORT GUM_Vector2 GUM_Backend_Font_measureText(GUM_Font* pFont, GblStringRef* pText, uint8_t fontSize) {
    GUM_Vector2 size    = { 0, 0 };
    Font        font    = *(Font*)GUM_IResource_data(GUM_IRESOURCE(pFont));
    Vector2     raySize = MeasureTextEx(font, pText, fontSize, 1.2f);
    size.x              = raySize.x;
    size.y              = raySize.y;
    return size;
}

GBL_EXPORT GBL_RESULT GUM_Backend_Font_draw(GUM_Renderer* pRenderer, GUM_Font* pFont, GblStringRef* pText,
                                            GUM_Vector2 position, GUM_Color color, int fontSize, float spacing) {
    if (!pFont) return GBL_RESULT_ERROR_INVALID_POINTER;

    Font font = *(Font*)GUM_IResource_data(GUM_IRESOURCE(pFont));

    DrawTextEx(font, pText,
               (Vector2){ position.x, position.y },
               fontSize, spacing,
               (Color){ color.r, color.g, color.b, color.a });

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Font_load(GUM_IResource* pSelf, GblStringRef* pPath) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    Font  font  = LoadFont(pPath);
    void* pFont = malloc(sizeof(Font));
    if (!pFont) return GBL_RESULT_ERROR_MEM_ALLOC;

    memcpy(pFont, &font, sizeof(Font));
    GUM_IResource_setData(pSelf, pFont);

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Font_unload(GUM_IResource* pSelf) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    void* pFont = (Font*)GUM_IResource_data(pSelf);
    if (!pFont) return GBL_RESULT_SUCCESS;

    UnloadFont(*(Font*)pFont);
    free(pFont);
    GUM_IResource_setData(pSelf, nullptr);

    return GBL_RESULT_SUCCESS;
}

GUM_Font* GUM_Backend_Font_default(void) {
    GUM_Font* pDefault = GUM_Font_default();
    if (pDefault) return pDefault;
    if (defaultFont_) return defaultFont_;

    Font  font     = GetFontDefault();
    void* pRayFont = malloc(sizeof(Font));
    if (!pRayFont) return nullptr;

    memcpy(pRayFont, &font, sizeof(Font));

    defaultFont_ = GUM_FONT(GblBox_create(GUM_Font_type()));
    GUM_IResource_setData(GUM_IRESOURCE(defaultFont_), pRayFont);
    return defaultFont_;
}

void GUM_Raylib_Font_deinit(void) {
    if (!defaultFont_) return;

    /* GetFontDefault() is owned by raylib, so only release libGumball's
     * copied Font struct and wrapper. Do not call UnloadFont(). */
    free(GUM_IResource_data(GUM_IRESOURCE(defaultFont_)));
    GUM_IResource_setData(GUM_IRESOURCE(defaultFont_), nullptr);
    GBL_UNREF(defaultFont_);
    defaultFont_ = nullptr;
}

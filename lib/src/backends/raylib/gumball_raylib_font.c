#include <gumball/core/gumball_backend.h>
#include <raylib.h>
//TODO: implement me!

static GUM_Font *defaultFont_ = nullptr;

GBL_EXPORT GUM_Vector2 GUM_Font_measureText(GUM_Font *pFont, GblStringRef *pText, uint8_t fontSize) {
	GUM_Vector2 size = {0, 0};
	Font font = *(Font*)GUM_IResource_data(GUM_IRESOURCE(pFont));
    Vector2 raySize = MeasureTextEx(font, pText, fontSize, 1.0f);
	size.x = raySize.x;
	size.y = raySize.y;
	return size;
}

GBL_EXPORT GBL_RESULT GUM_Backend_Font_draw(GUM_Renderer *pRenderer, GUM_Font *pFont,
                                            GblStringRef *pText, GUM_Vector2 position,
                                            GUM_Color color, int fontSize, float spacing)
{
    if (!pFont) return GBL_RESULT_ERROR_INVALID_POINTER;
    Font font = *(Font*)GUM_IResource_data(GUM_IRESOURCE(pFont));
    DrawTextEx(font, pText, (Vector2){ position.x, position.y }, fontSize, spacing, (Color){ color.r, color.g, color.b, color.a });
    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Font_load(GUM_IResource *pSelf, GblStringRef *pPath) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    Font font = LoadFont(pPath);
    void* pFont = malloc(sizeof(Font));
    memcpy(pFont, &font, sizeof(Font));
    GUM_IResource_setData(pSelf, pFont);
}

GBL_RESULT GUM_Backend_Font_unload(GUM_IResource* pSelf) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;
    void* pFont = (Font*)GUM_IResource_data(pSelf);
    UnloadFont(*(Font*)pFont);
    free(pFont);
    return GBL_RESULT_SUCCESS;
}

GUM_Font* GUM_Backend_Font_default(void) {
    if (defaultFont_) return defaultFont_;

    Font font = GetFontDefault();
    void *pRayFont = malloc(sizeof(Font));
    memcpy(pRayFont, &font, sizeof(Font));
    defaultFont_ = GUM_FONT(GblBox_create(GUM_Font_type()));
    GUM_IResource_setData(GUM_IRESOURCE(defaultFont_), pRayFont);
    return defaultFont_;
}
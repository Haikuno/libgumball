#include "../../ifaces/gumball_iresource_.h"
#include <gumball/core/gumball_backend.h>
#include <raylib.h>
#include <stdlib.h>

static bool GUM_Raylib_Font_isDefault_(Font font) {
    const Font fallback = GetFontDefault();
    return font.texture.id == fallback.texture.id &&
           font.glyphs     == fallback.glyphs &&
           font.recs       == fallback.recs;
}

GBL_EXPORT GUM_Vector2 GUM_Backend_Font_measureText(GUM_Font* pFont, GblStringRef* pText, uint8_t fontSize) {
    if (!pFont || !pText) return (GUM_Vector2){ 0 };

    Font* pRayFont = GUM_IResource_data_(GUM_IRESOURCE(pFont));
    if (!pRayFont) return (GUM_Vector2){ 0 };

    GUM_Vector2 size    = { 0, 0 };
    Vector2     raySize = MeasureTextEx(*pRayFont, pText, fontSize, 1.2f);
    size.x              = raySize.x;
    size.y              = raySize.y;
    return size;
}

GBL_EXPORT GBL_RESULT GUM_Backend_Font_draw(GUM_Renderer* pRenderer, GUM_Font* pFont, GblStringRef* pText,
                                            GUM_Vector2 position, GUM_Color color, int fontSize, float spacing) {
    if (!pFont || !pText) return GBL_RESULT_ERROR_INVALID_POINTER;

    GBL_UNUSED(pRenderer);

    Font* pRayFont = GUM_IResource_data_(GUM_IRESOURCE(pFont));
    if (!pRayFont) return GBL_RESULT_ERROR_INVALID_POINTER;

    DrawTextEx(*pRayFont, pText,
               (Vector2){ position.x, position.y },
               fontSize, spacing,
               (Color){ color.r, color.g, color.b, color.a });

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Font_load(GUM_IResource* pSelf, GblStringRef* pPath) {
    if (!pSelf || !pPath) return GBL_RESULT_ERROR_INVALID_POINTER;

    Font* pFont = malloc(sizeof(*pFont));
    if (!pFont) return GBL_RESULT_ERROR_MEM_ALLOC;

    const Font loaded = LoadFontEx(pPath, 22, nullptr, 0);
    if (loaded.texture.id == 0 || !loaded.glyphs || !loaded.recs || GUM_Raylib_Font_isDefault_(loaded)) {
        // LoadFont falls back to the default font on failure.
        free(pFont);
        return GBL_RESULT_ERROR_FILE_READ;
    }

    *pFont = loaded;
    GUM_IResource_setData_(pSelf, pFont);

    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Font_unload(GUM_IResource* pSelf) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    Font* pFont = GUM_IResource_data_(pSelf);
    if (!pFont) return GBL_RESULT_SUCCESS;

    UnloadFont(*pFont);
    free(pFont);
    GUM_IResource_setData_(pSelf, nullptr);

    return GBL_RESULT_SUCCESS;
}

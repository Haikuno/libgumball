#include "gumball_sdl3_internal.h"
#include "../../ifaces/gumball_iresource_.h"
#include <gumball/core/gumball_backend.h>
#include <math.h>

GBL_EXPORT GUM_Vector2 GUM_Backend_Font_measureText(GUM_Font* pFont, GblStringRef* pText, uint8_t fontSize) {
    if (!pFont || !pText) return (GUM_Vector2){ 0 };

    TTF_Font* pSdlFont = GUM_IResource_data_(GUM_IRESOURCE(pFont));
    if (!pSdlFont) return (GUM_Vector2){ 0 };

    if (!TTF_SetFontSize(pSdlFont, fontSize))
        return (GUM_Vector2){ 0 };

    int width  = 0;
    int height = 0;
    if (!TTF_GetStringSize(pSdlFont, pText, 0, &width, &height))
        return (GUM_Vector2){ 0 };

    return (GUM_Vector2){ width, height };
}

GBL_EXPORT GBL_RESULT GUM_Backend_Font_draw(GUM_Renderer* pRenderer, GUM_Font* pFont, GblStringRef* pText,
                                            GUM_Vector2 position, GUM_Color color, int fontSize, float spacing) {
    if (!pFont || !pText) return GBL_RESULT_ERROR_INVALID_POINTER;

    GBL_UNUSED(spacing);

    TTF_TextEngine* pTextEngine = GUM_SDL3_textEngine_(pRenderer);
    TTF_Font* pSdlFont = GUM_IResource_data_(GUM_IRESOURCE(pFont));
    if (!pTextEngine || !pSdlFont) return GBL_RESULT_ERROR_INVALID_POINTER;

    if (!TTF_SetFontSize(pSdlFont, (float)fontSize))
        return GBL_RESULT_ERROR_INTERNAL;

    TTF_Text* pSdlText = TTF_CreateText(pTextEngine, pSdlFont, pText, 0);
    if (!pSdlText) return GBL_RESULT_ERROR_INTERNAL;

    if (!TTF_SetTextColor(pSdlText, color.r, color.g, color.b, color.a)) {
        TTF_DestroyText(pSdlText);
        return GBL_RESULT_ERROR_INTERNAL;
    }

    const bool success = TTF_DrawRendererText(pSdlText, roundf(position.x), roundf(position.y));
    TTF_DestroyText(pSdlText);

    return success ? GBL_RESULT_SUCCESS : GBL_RESULT_ERROR_INTERNAL;
}

GBL_RESULT GUM_Backend_Font_load(GUM_IResource* pSelf, GblStringRef* pPath) {
    if (!pSelf || !pPath) return GBL_RESULT_ERROR_INVALID_POINTER;
    if (!GUM_SDL3_ttfEnsure_()) return GBL_RESULT_ERROR_INTERNAL;

    TTF_Font* pFont = TTF_OpenFont(pPath, 22.0f);
    if (!pFont) return GBL_RESULT_ERROR_FILE_READ;

    GUM_IResource_setData_(pSelf, pFont);
    return GBL_RESULT_SUCCESS;
}

GBL_RESULT GUM_Backend_Font_unload(GUM_IResource* pSelf) {
    if (!pSelf) return GBL_RESULT_ERROR_INVALID_POINTER;

    TTF_Font* pFont = GUM_IResource_data_(pSelf);
    if (pFont) TTF_CloseFont(pFont);
    GUM_IResource_setData_(pSelf, nullptr);
    return GBL_RESULT_SUCCESS;
}

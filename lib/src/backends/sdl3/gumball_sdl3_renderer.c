#include "gumball_sdl3_internal.h"
#include <gumball/backends/gumball_sdl3.h>
#include <stdlib.h>

static GUM_Renderer* pActiveRenderer_ = nullptr;

bool GUM_SDL3_ttfEnsure_(void) {
    return TTF_WasInit() || TTF_Init();
}

GUM_Renderer* GUM_SDL3_renderer_(GUM_Renderer* pRenderer) {
    return pRenderer ? pRenderer : pActiveRenderer_;
}

SDL_Renderer* GUM_SDL3_nativeRenderer_(GUM_Renderer* pRenderer) {
    pRenderer = GUM_SDL3_renderer_(pRenderer);
    return pRenderer ? pRenderer->pRenderer : nullptr;
}

TTF_TextEngine* GUM_SDL3_textEngine_(GUM_Renderer* pRenderer) {
    pRenderer = GUM_SDL3_renderer_(pRenderer);
    return pRenderer ? pRenderer->pTextEngine : nullptr;
}

GBL_EXPORT GUM_Renderer* GUM_Renderer_create(void* pRenderer) {
    if (!pRenderer || !GUM_SDL3_ttfEnsure_()) return nullptr;

    GUM_Renderer* pSelf = malloc(sizeof(*pSelf));
    if (!pSelf) return nullptr;

    *pSelf = (GUM_Renderer){ .pRenderer = pRenderer };
    pSelf->pTextEngine = TTF_CreateRendererTextEngine(pSelf->pRenderer);

    if (!pSelf->pTextEngine) {
        free(pSelf);
        return nullptr;
    }

    pActiveRenderer_ = pSelf;
    return pSelf;
}

GBL_EXPORT void GUM_Renderer_destroy(GUM_Renderer* pSelf) {
    if (!pSelf) return;

    if (pActiveRenderer_ == pSelf)
        pActiveRenderer_ = nullptr;

    TTF_DestroyRendererTextEngine(pSelf->pTextEngine);
    free(pSelf);
}

GBL_EXPORT GUM_Renderer* GUM_SDL3_Renderer_create(SDL_Renderer* pRenderer) {
    return GUM_Renderer_create(pRenderer);
}

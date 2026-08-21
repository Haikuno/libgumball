#ifndef GUM_SDL3_INTERNAL_H
#define GUM_SDL3_INTERNAL_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <gumball/types/gumball_renderer.h>

struct GUM_Renderer {
    SDL_Renderer*  pRenderer;
    TTF_TextEngine* pTextEngine;
};

GUM_Renderer*  GUM_SDL3_renderer_(GUM_Renderer* pRenderer);
SDL_Renderer*  GUM_SDL3_nativeRenderer_(GUM_Renderer* pRenderer);
TTF_TextEngine* GUM_SDL3_textEngine_(GUM_Renderer* pRenderer);
bool           GUM_SDL3_ttfEnsure_(void);
void           GUM_SDL3_gamepadsUpdate_(void);

#endif // GUM_SDL3_INTERNAL_H

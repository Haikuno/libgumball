#ifndef GUM_SDL3_INTERNAL_H
#define GUM_SDL3_INTERNAL_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <gumball/types/gumball_renderer.h>

constexpr int GUM_SDL3_CLIP_STACK_MAX_ = 16;

typedef struct {
    bool     enabled;
    SDL_Rect rect;
} GUM_SDL3_ClipState_;

struct GUM_Renderer {
    SDL_Renderer*      pRenderer;
    TTF_TextEngine*    pTextEngine;
    GUM_SDL3_ClipState_ clipStack[GUM_SDL3_CLIP_STACK_MAX_];
    uint8_t             clipDepth;
};

GUM_Renderer*   GUM_SDL3_renderer_(GUM_Renderer* pRenderer);
SDL_Renderer*   GUM_SDL3_nativeRenderer_(GUM_Renderer* pRenderer);
TTF_TextEngine* GUM_SDL3_textEngine_(GUM_Renderer* pRenderer);
bool            GUM_SDL3_ttfEnsure_(void);
void            GUM_SDL3_gamepadsUpdate_(void);
void            GUM_SDL3_gamepadsDeinit_(void);

#endif // GUM_SDL3_INTERNAL_H

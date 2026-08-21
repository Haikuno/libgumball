#include "gumball_backend_parity_scene.h"
#include <SDL3/SDL.h>
#include <gumball/backends/gumball_sdl3.h>

int main(int argc, char* pArgv[]) {
    if (argc != 2 || !SDL_Init(SDL_INIT_VIDEO)) return 1;

    SDL_Surface* pSurface = SDL_CreateSurface(GUM_BACKEND_PARITY_WIDTH,
                                              GUM_BACKEND_PARITY_HEIGHT,
                                              SDL_PIXELFORMAT_RGBA32);
    SDL_Renderer* pSdlRenderer = pSurface ? SDL_CreateSoftwareRenderer(pSurface) : nullptr;
    GUM_Renderer* pRenderer = pSdlRenderer ? GUM_SDL3_Renderer_create(pSdlRenderer) : nullptr;
    if (!pRenderer) return 1;

    GUM_Root* pRoot = GUM_BackendParityScene_create();

    SDL_SetRenderDrawColor(pSdlRenderer, 24, 24, 24, 255);
    SDL_RenderClear(pSdlRenderer);
    GUM_draw(pRenderer);
    SDL_RenderPresent(pSdlRenderer);

    const bool saved = SDL_SaveBMP(pSurface, pArgv[1]);

    GUM_unref(pRoot);
    GUM_Renderer_destroy(pRenderer);
    SDL_DestroyRenderer(pSdlRenderer);
    SDL_DestroySurface(pSurface);
    SDL_Quit();
    return saved ? 0 : 1;
}

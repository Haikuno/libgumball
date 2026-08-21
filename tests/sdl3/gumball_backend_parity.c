#include "gumball_backend_parity_scene.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <gumball/backends/gumball_sdl3.h>
#include <stdio.h>

#define GUM_PARITY_PATH_CAPACITY 4096

static const char* outputPath_(int argc, char* pArgv[], char path[GUM_PARITY_PATH_CAPACITY]) {
    if (argc > 2) {
        fprintf(stderr, "usage: %s [output.png]\n", pArgv[0]);
        return nullptr;
    }

    if (argc == 2) return pArgv[1];

    const char* pBasePath = SDL_GetBasePath();
    if (!pBasePath) return nullptr;

    const int length = SDL_snprintf(path, GUM_PARITY_PATH_CAPACITY,
                                    "%sbackend-parity-sdl3.png", pBasePath);
    return length >= 0 && length < GUM_PARITY_PATH_CAPACITY ? path : nullptr;
}

int main(int argc, char* pArgv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO)) return 1;

    char outputPath[GUM_PARITY_PATH_CAPACITY];
    const char* pOutputPath = outputPath_(argc, pArgv, outputPath);
    if (!pOutputPath) {
        SDL_Quit();
        return 1;
    }

    SDL_Surface* pSurface = SDL_CreateSurface(GUM_BACKEND_PARITY_WIDTH,
                                              GUM_BACKEND_PARITY_HEIGHT,
                                              SDL_PIXELFORMAT_RGBA32);
    SDL_Renderer* pSdlRenderer = pSurface ? SDL_CreateSoftwareRenderer(pSurface) : nullptr;
    GUM_Renderer* pRenderer = pSdlRenderer ? GUM_SDL3_Renderer_create(pSdlRenderer) : nullptr;
    if (!pRenderer) {
        SDL_DestroyRenderer(pSdlRenderer);
        SDL_DestroySurface(pSurface);
        SDL_Quit();
        return 1;
    }

    GUM_Root* pRoot = GUM_BackendParityScene_create();

    SDL_SetRenderDrawColor(pSdlRenderer, 24, 24, 24, 255);
    SDL_RenderClear(pSdlRenderer);
    GUM_draw(pRenderer);
    SDL_RenderPresent(pSdlRenderer);

    const bool saved = IMG_SavePNG(pSurface, pOutputPath);
    if (saved)
        printf("Wrote %s\n", pOutputPath);
    else
        fprintf(stderr, "Failed to write %s: %s\n", pOutputPath, SDL_GetError());

    GUM_unref(pRoot);
    GUM_Renderer_destroy(pRenderer);
    SDL_DestroyRenderer(pSdlRenderer);
    SDL_DestroySurface(pSurface);
    SDL_Quit();
    return saved ? 0 : 1;
}

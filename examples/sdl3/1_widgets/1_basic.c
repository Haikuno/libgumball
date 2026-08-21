#include <gumball/gumball.h>
#include <gumball/backends/gumball_sdl3.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) return 1;

    SDL_Window* pWindow = SDL_CreateWindow("Widgets - Basic", 640, 480, 0);
    SDL_Renderer* pSdlRenderer = pWindow ? SDL_CreateRenderer(pWindow, nullptr) : nullptr;
    GUM_Renderer* pRenderer = pSdlRenderer ? GUM_SDL3_Renderer_create(pSdlRenderer) : nullptr;
    if (!pRenderer) return 1;

    GUM_Root* pRoot = GUM_Root_create();
    GUM_Widget* pWidget = GUM_Widget_create();

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            GUM_SDL3_processEvent(&event);
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        GUM_update();

        SDL_SetRenderDrawColor(pSdlRenderer, 245, 245, 245, 255);
        SDL_RenderClear(pSdlRenderer);
        GUM_draw(pRenderer);
        SDL_RenderPresent(pSdlRenderer);
    }

    GUM_unref(pRoot);
    GUM_Renderer_destroy(pRenderer);
    SDL_DestroyRenderer(pSdlRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return 0;
}

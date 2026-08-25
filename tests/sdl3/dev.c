#include <gumball/gumball.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

void onPressCallback(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    GblType deviceType = GBL_TYPEOF(pEvent->pInputDevice);
    GUM_LOG_INFO("Button with label %s pressed   by a %s", GUM_Widget_label(pSelf), deviceType == GUM_MOUSE_TYPE    ? "mouse"    :
                                                                                    deviceType == GUM_KEYBOARD_TYPE ? "keyboard" :
                                                                                    deviceType == GUM_GAMEPAD_TYPE  ? "gamepad"  :
                                                                                   "unknown device type");
}

void onFocusGainedCallback(GUM_Widget* pSelf, GUM_InputDevice* pDevice) {
    GblType deviceType = GBL_TYPEOF(pDevice);
    GUM_LOG_INFO("Button with label %s focused   by a %s", GUM_Widget_label(pSelf), deviceType == GUM_MOUSE_TYPE    ? "mouse"    :
                                                                                    deviceType == GUM_KEYBOARD_TYPE ? "keyboard" :
                                                                                    deviceType == GUM_GAMEPAD_TYPE  ? "gamepad"  :
                                                                                   "unknown device type");
    GUM_Widget_animate(pSelf, "w", 250.0f, 0.25, GUM_EASE_SINE_IN_OUT);
    GUM_Widget_animate(pSelf, "h", 350.0f, 0.25, GUM_EASE_SINE_IN_OUT);
}

void onReleaseCallback(GUM_Widget* pSelf, GUM_Event_Input* pEvent) {
    GblType deviceType = GBL_TYPEOF(pEvent->pInputDevice);
    GUM_LOG_INFO("Button with label %s released  by a %s", GUM_Widget_label(pSelf),  deviceType == GUM_MOUSE_TYPE    ? "mouse"    :
                                                                                     deviceType == GUM_KEYBOARD_TYPE ? "keyboard" :
                                                                                     deviceType == GUM_GAMEPAD_TYPE  ? "gamepad"  :
                                                                                     "unknown device type");
}

void onFocusLostCallback(GUM_Widget* pSelf, GUM_InputDevice* pDevice) {
    GblType deviceType = GBL_TYPEOF(pDevice);
    GUM_LOG_INFO("Button with label %s unfocused by a %s", GUM_Widget_label(pSelf),  deviceType == GUM_MOUSE_TYPE    ? "mouse"    :
                                                                                     deviceType == GUM_KEYBOARD_TYPE ? "keyboard" :
                                                                                     deviceType == GUM_GAMEPAD_TYPE  ? "gamepad"  :
                                                                                     "unknown device type");
    if (!GUM_Widget_isFocused(pSelf)) {
        GUM_Widget_animate(pSelf, "w", 200.0f, 0.25, GUM_EASE_SINE_IN_OUT);
        GUM_Widget_animate(pSelf, "h", 300.0f, 0.25, GUM_EASE_SINE_IN_OUT);
    }
}

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) return 1;

    SDL_Window* pWindow = SDL_CreateWindow("libGumball SDL3 dev", 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* pSdlRenderer = pWindow ? SDL_CreateRenderer(pWindow, nullptr) : nullptr;
    GUM_Renderer* pRenderer = pSdlRenderer ? GUM_Renderer_create(pSdlRenderer) : nullptr;
    if (!pRenderer) return 1;

    GUM_Root* pRoot = GUM_Root_create();
    auto pFont = GUM_Manager_load("../lib/resources/fonts/default.ttf");
    GUM_Font_setDefault(GUM_FONT(pFont));


    GUM_Container* pOuter = GUM_Container_create("x", 20.0f, "y", 40.0f,
                                                 "w", 800.0f, "h", 1000.0f,
                                                 "padding", 5.0f,
                                                 "margin", 5.0f,
                                                 "border_color",   0x000000FF,
                                                 "color",          0xF0F0F0FF,
                                                 "resizeWidgets",  false);

    GUM_Container* pInner = GUM_Container_create("parent", pOuter,
                                                 "name", "Inner container 1",
                                                 "w", 600.0f, "h", 800.0f,
                                                 "border_color",  0x0000FFFF,
                                                 "margin", 10.0f,
                                                 "padding", 10.0f,
                                                 "resizeWidgets", false);

    GUM_Container* pInner2 = GUM_Container_create("parent", pOuter,
                                                "name", "Inner container 2",
                                                "w", 600.0f, "h", 800.0f,
                                                "border_color",  0x0000FFFF,
                                                "margin", 10.0f,
                                                "padding", 10.0f,
                                                "resizeWidgets", false);


    const unsigned colors[]  = { 0x7C3AEDFF, 0x18B7A5FF, 0xD94F6EFF, 0x5FA84BFF, 0xF2C14EFF };
    const char*    labels[]  = { "Inner A", "Inner B", "Inner C", "Inner D", "Inner E" };
    for (int i = 0; i < 5; ++i) {
        GUM_connect(GUM_Button_create("parent", pInner,
                                      "h",     300.0f,
                                      "w",     200.0f,
                                      "border_color", 0x000000FF,
                                      "color", colors[i],
                                      "label", labels[i]),
                    "onPress",       onPressCallback,
                    "onFocusGained", onFocusGainedCallback,
                    "onRelease",     onReleaseCallback,
                    "onFocusLost",   onFocusLostCallback);

        GUM_connect(GUM_Button_create("parent", pInner2,
                                      "h",     300.0f,
                                      "border_color", 0x000000FF,
                                      "color", colors[i],
                                      "label", labels[i]),
                    "onPress",       onPressCallback,
                    "onFocusGained", onFocusGainedCallback,
                    "onRelease",     onReleaseCallback,
                    "onFocusLost",   onFocusLostCallback);
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            GUM_processEvent(&event);
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        GUM_update();

        SDL_SetRenderDrawColor(pSdlRenderer, 240, 240, 240, 255);
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

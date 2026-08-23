#include "gumball_sdl3_internal.h"
#include <gumball/core/gumball_backend.h>

static Uint64 lastTicks_ = 0;
static float  frameTime_ = 0.0f;

void GUM_Backend_update(void) {
    const Uint64 ticks = SDL_GetTicksNS();

    if (lastTicks_)
        frameTime_ = (float)(ticks - lastTicks_) / 1000000000.0f;

    lastTicks_ = ticks;
    GUM_SDL3_gamepadsUpdate_();
}

void GUM_Backend_deinit(void) {
    // Resource payloads may outlive a Root; only backend-owned transient state resets here.
    GUM_SDL3_gamepadsDeinit_();
    lastTicks_ = 0;
    frameTime_ = 0.0f;
}

uint32_t GUM_Backend_timestamp(void) {
    return (uint32_t)SDL_GetTicks();
}

float GUM_Backend_frametime(void) {
    return frameTime_;
}

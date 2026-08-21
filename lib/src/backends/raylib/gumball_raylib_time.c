#include <gumball/core/gumball_backend.h>
#include <raylib.h>

void GUM_Backend_update(void) {}
void GUM_Backend_deinit(void) {}

uint32_t GUM_Backend_timestamp(void) {
    return (uint32_t)(GetTime() * 1000);
}

float GUM_Backend_frametime(void) {
    return GetFrameTime();
}

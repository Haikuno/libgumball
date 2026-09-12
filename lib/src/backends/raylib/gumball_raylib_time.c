#include <gumball/core/gumball_backend.h>
#include <gumball/elements/gumball_common.h>
#include <raylib.h>

void GUM_Raylib_Draw_deinit(void);

void GUM_Backend_update(void) {}

GBL_EXPORT void GUM_processEvent(const void* pEvent) {
    GBL_UNUSED(pEvent);
}

void GUM_Backend_deinit(void) {
    // Resource payloads may outlive a Root; only backend-owned state resets here.
    GUM_Raylib_Draw_deinit();
}

uint32_t GUM_Backend_timestamp(void) {
    return (uint32_t)(GetTime() * 1000);
}

float GUM_Backend_frametime(void) {
    return GetFrameTime();
}

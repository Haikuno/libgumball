#include <gumball/core/gumball_backend.h>

struct GUM_Renderer {};

GBL_EXPORT GUM_Renderer* GUM_Renderer_create(void* pRenderer) {
    GBL_UNUSED(pRenderer);
    return nullptr;
}

GBL_EXPORT void GUM_Renderer_destroy(GUM_Renderer* pSelf) {
    GBL_UNUSED(pSelf);
}

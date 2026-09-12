#include <gumball/gumball.h>
#include <gumball/core/gumball_backend.h>
#include <stdio.h>
#include <string.h>

#if defined(GUM_TEST_BACKEND_SDL3)
#include <SDL3/SDL.h>

static SDL_Surface*  pSurface_     = nullptr;
static SDL_Renderer* pSdlRenderer_ = nullptr;
static GUM_Renderer* pRenderer_    = nullptr;

static bool backendInit_(void) {
    if (!SDL_Init(SDL_INIT_VIDEO))
        return false;

    pSurface_ = SDL_CreateSurface(320, 160, SDL_PIXELFORMAT_RGBA32);
    pSdlRenderer_ = pSurface_ ? SDL_CreateSoftwareRenderer(pSurface_) : nullptr;
    pRenderer_ = pSdlRenderer_ ? GUM_Renderer_create(pSdlRenderer_) : nullptr;
    return pRenderer_ != nullptr;
}

static bool drawScene_(void) {
    if (!SDL_SetRenderDrawColor(pSdlRenderer_, 24, 24, 24, 255) ||
        !SDL_RenderClear(pSdlRenderer_)) {
        return false;
    }

    return GUM_draw(pRenderer_) == GBL_RESULT_SUCCESS;
}

static void backendDeinit_(void) {
    GUM_Renderer_destroy(pRenderer_);
    SDL_DestroyRenderer(pSdlRenderer_);
    SDL_DestroySurface(pSurface_);
    SDL_Quit();
}
#elif defined(GUM_TEST_BACKEND_RAYLIB)
#include <raylib.h>

static bool backendInit_(void) {
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(320, 160, "libGumball default font");
    return IsWindowReady();
}

static bool drawScene_(void) {
    BeginDrawing();
    ClearBackground((Color){ 24, 24, 24, 255 });
    const GBL_RESULT result = GUM_draw();
    EndDrawing();
    return result == GBL_RESULT_SUCCESS;
}

static void backendDeinit_(void) {
    CloseWindow();
}
#else
#error "GumballDefaultFont requires a configured backend"
#endif

static bool defaultResourceName_(GUM_Font* pFont) {
    GBL_VARIANT(resource);
    GBL_VARIANT(name);
    bool valid = false;

    if (GBL_RESULT_SUCCESS(GblVariant_setBoxCopy(&resource, GBL_BOX(pFont))) &&
        GBL_RESULT_SUCCESS(GblVariant_constructString(&name, "")) &&
        GBL_RESULT_SUCCESS(GblVariant_convert(&resource, &name))) {
        const char* pName = GblVariant_string(&name);
        valid = pName && strcmp(pName, "default.ttf") == 0;
    }

    GblVariant_destruct(&name);
    GblVariant_destruct(&resource);
    return valid;
}

int main(void) {
    bool passed = false;
    bool defaultARetained = false;
    GUM_Root* pRootA = nullptr;
    GUM_Root* pRootB = nullptr;
    GUM_Font* pDefaultA = nullptr;
    GUM_IResource* pBundledA = nullptr;
    GUM_IResource* pBundledB = nullptr;
    GUM_IResource* pCustom = nullptr;

    if (!backendInit_()) {
        fprintf(stderr, "Backend initialization failed for default-font test\n");
        return 1;
    }

    GUM_Font_setDefault(nullptr);
    pRootA = GUM_Root_create();
    if (!pRootA) {
        fprintf(stderr, "Failed to create first Root\n");
        goto cleanup;
    }

    if (GUM_Font_default()) {
        fprintf(stderr, "Application default should begin cleared\n");
        goto cleanup;
    }

    pDefaultA = GUM_Backend_Font_default();
    pBundledA = GUM_Manager_load(GUM_TEST_DEFAULT_FONT_PATH);
    if (!pDefaultA || !pBundledA || pDefaultA != GUM_FONT(pBundledA)) {
        fprintf(stderr, "Framework default is not the bundled default.ttf resource\n");
        goto cleanup;
    }

    if (!defaultResourceName_(pDefaultA)) {
        fprintf(stderr, "Bundled default font did not retain default.ttf display metadata\n");
        goto cleanup;
    }

    const GUM_Vector2 defaultSize = GUM_Backend_Font_measureText(pDefaultA,
                                                                 "Bundled default",
                                                                 18);
    if (defaultSize.x <= 0.0f || defaultSize.y <= 0.0f) {
        fprintf(stderr, "Bundled default font is not measurable\n");
        goto cleanup;
    }

    GUM_Widget* pFrameworkWidget = GUM_Widget_create("x", 8.0f,
                                                     "y", 8.0f,
                                                     "w", 304.0f,
                                                     "h", 36.0f,
                                                     "label", "Bundled default");
    if (!pFrameworkWidget || GUM_Widget_font(pFrameworkWidget) != pDefaultA || !drawScene_()) {
        fprintf(stderr, "Widget did not render with the bundled framework default\n");
        goto cleanup;
    }

    pCustom = GUM_Manager_load(GUM_TEST_CUSTOM_FONT_PATH);
    if (!pCustom || pCustom == pBundledA) {
        fprintf(stderr, "Failed to load distinct custom font\n");
        goto cleanup;
    }

    GUM_Font_setDefault(GUM_FONT(pCustom));
    if (GUM_Font_default() != GUM_FONT(pCustom) ||
        GUM_Backend_Font_default() != GUM_FONT(pCustom)) {
        fprintf(stderr, "Explicit application font override was not selected\n");
        goto cleanup;
    }

    GUM_Widget* pCustomWidget = GUM_Widget_create("x", 8.0f,
                                                  "y", 52.0f,
                                                  "w", 304.0f,
                                                  "h", 36.0f,
                                                  "label", "Custom override");
    if (!pCustomWidget || GUM_Widget_font(pCustomWidget) != GUM_FONT(pCustom)) {
        fprintf(stderr, "Widget did not retain the explicit font override\n");
        goto cleanup;
    }

    GUM_Font_setDefault(nullptr);
    if (GUM_Font_default() || GUM_Backend_Font_default() != pDefaultA) {
        fprintf(stderr, "Clearing the override did not restore the bundled default\n");
        goto cleanup;
    }

    GUM_Widget* pClearedWidget = GUM_Widget_create("x", 8.0f,
                                                   "y", 96.0f,
                                                   "w", 304.0f,
                                                   "h", 36.0f,
                                                   "label", "Bundled again");
    if (!pClearedWidget || GUM_Widget_font(pClearedWidget) != pDefaultA || !drawScene_()) {
        fprintf(stderr, "Widget did not return to the bundled default after clearing override\n");
        goto cleanup;
    }

    GUM_Font_setDefault(GUM_FONT(pCustom));
    if (GUM_Font_default() != GUM_FONT(pCustom)) {
        fprintf(stderr, "Failed to reinstall override for Root teardown coverage\n");
        goto cleanup;
    }

    GUM_IResource_ref(GUM_IRESOURCE(pDefaultA));
    defaultARetained = true;

    GUM_IResource_unref(pBundledA);
    pBundledA = nullptr;
    GUM_IResource_unref(pCustom);
    pCustom = nullptr;

    GUM_unref(pRootA);
    pRootA = nullptr;

    if (GUM_Font_default()) {
        fprintf(stderr, "Root teardown did not clear the application font override\n");
        goto cleanup;
    }

    const GUM_Vector2 retainedSize = GUM_Backend_Font_measureText(pDefaultA,
                                                                  "retained",
                                                                  18);
    if (retainedSize.x <= 0.0f || retainedSize.y <= 0.0f) {
        fprintf(stderr, "Caller-retained bundled font was unloaded during Root teardown\n");
        goto cleanup;
    }

    pRootB = GUM_Root_create();
    GUM_Font* pDefaultB = pRootB ? GUM_Backend_Font_default() : nullptr;
    pBundledB = pRootB ? GUM_Manager_load(GUM_TEST_DEFAULT_FONT_PATH) : nullptr;
    if (!pRootB || !pDefaultB || !pBundledB ||
        pDefaultB != GUM_FONT(pBundledB) || pDefaultB == pDefaultA) {
        fprintf(stderr, "Framework default did not recreate from the bundled font\n");
        goto cleanup;
    }

    GUM_Widget* pRestartWidget = GUM_Widget_create("x", 8.0f,
                                                   "y", 8.0f,
                                                   "w", 304.0f,
                                                   "h", 36.0f,
                                                   "label", "Restarted default");
    if (!pRestartWidget || GUM_Widget_font(pRestartWidget) != pDefaultB || !drawScene_()) {
        fprintf(stderr, "Recreated Root did not render with the bundled default\n");
        goto cleanup;
    }

    GUM_IResource_unref(pBundledB);
    pBundledB = nullptr;
    GUM_IResource_unref(GUM_IRESOURCE(pDefaultA));
    defaultARetained = false;
    passed = true;

cleanup:
    GUM_Font_setDefault(nullptr);
    if (pBundledB) GUM_IResource_unref(pBundledB);
    if (pBundledA) GUM_IResource_unref(pBundledA);
    if (pCustom) GUM_IResource_unref(pCustom);
    if (defaultARetained) GUM_IResource_unref(GUM_IRESOURCE(pDefaultA));
    if (pRootB) GUM_unref(pRootB);
    if (pRootA) GUM_unref(pRootA);
    backendDeinit_();
    return passed ? 0 : 1;
}

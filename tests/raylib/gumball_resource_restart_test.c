#include <gumball/gumball.h>
#include <gumball/core/gumball_backend.h>
#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <string.h>

#ifndef GUM_TEST_BITMAP_FONT_PATH
#define GUM_TEST_BITMAP_FONT_PATH "../examples/resources/Nimbus.fnt"
#endif

static int hostTraceCount_ = 0;

static void hostTraceLog_(int logLevel, const char* pText, va_list args) {
    GBL_UNUSED(logLevel, args);
    if (pText && strcmp(pText, "libGumball host logger probe") == 0)
        ++hostTraceCount_;
}

static bool hostLoggerAlive_(void) {
    const int previous = hostTraceCount_;
    TraceLog(LOG_INFO, "libGumball host logger probe");
    return hostTraceCount_ == previous + 1;
}

static bool colorEquals_(Color actual, Color expected) {
    return actual.r == expected.r && actual.g == expected.g &&
           actual.b == expected.b && actual.a == expected.a;
}

static bool nestedScissorRestores_(void) {
    const Color background = { 12, 18, 24, 255 };
    const Color inner = { 180, 60, 40, 255 };
    const Color outer = { 40, 180, 80, 255 };

    BeginDrawing();
    ClearBackground(background);

    const bool operations =
        GUM_Backend_beginScissor(nullptr,
                                 (GUM_Rectangle){ 0.0f, 0.0f, 32.0f, 32.0f }) == GBL_RESULT_SUCCESS &&
        GUM_Backend_beginScissor(nullptr,
                                 (GUM_Rectangle){ 0.0f, 0.0f, 8.0f, 8.0f }) == GBL_RESULT_SUCCESS &&
        GUM_Backend_rectangleDraw(nullptr,
                                  (GUM_Rectangle){ 0.0f, 0.0f, 64.0f, 64.0f },
                                  0.0f,
                                  (GUM_Color){ inner.r, inner.g, inner.b, inner.a }) == GBL_RESULT_SUCCESS &&
        GUM_Backend_endScissor(nullptr) == GBL_RESULT_SUCCESS &&
        GUM_Backend_rectangleDraw(nullptr,
                                  (GUM_Rectangle){ 0.0f, 0.0f, 64.0f, 64.0f },
                                  0.0f,
                                  (GUM_Color){ outer.r, outer.g, outer.b, outer.a }) == GBL_RESULT_SUCCESS &&
        GUM_Backend_endScissor(nullptr) == GBL_RESULT_SUCCESS &&
        GUM_Backend_endScissor(nullptr) == GBL_RESULT_ERROR_OUT_OF_RANGE;

    rlDrawRenderBatchActive();
    Image image = LoadImageFromScreen();
    EndDrawing();

    if (!operations || !image.data) {
        if (image.data) UnloadImage(image);
        return false;
    }

    const Color insideOuter = GetImageColor(image, 16, 16);
    const Color outsideOuter = GetImageColor(image, 48, 48);
    UnloadImage(image);

    return colorEquals_(insideOuter, outer) && colorEquals_(outsideOuter, background);
}

static bool drawResources_(GUM_Texture* pTexture, GUM_Font* pFont, GUM_Font* pDefaultFont) {
    BeginDrawing();
    ClearBackground((Color){ 0, 0, 0, 255 });

    const GBL_RESULT textureResult = GUM_Backend_Texture_draw(
        nullptr,
        pTexture,
        (GUM_Rectangle){ 0.0f, 0.0f, 32.0f, 32.0f },
        (GUM_Color){ 255, 255, 255, 255 });
    const GBL_RESULT fontResult = GUM_Backend_Font_draw(
        nullptr,
        pFont,
        "restart",
        (GUM_Vector2){ 0.0f, 36.0f },
        (GUM_Color){ 255, 255, 255, 255 },
        12,
        0.0f);
    const GBL_RESULT defaultFontResult = GUM_Backend_Font_draw(
        nullptr,
        pDefaultFont,
        "default",
        (GUM_Vector2){ 0.0f, 50.0f },
        (GUM_Color){ 255, 255, 255, 255 },
        12,
        0.0f);

    EndDrawing();
    return textureResult == GBL_RESULT_SUCCESS &&
           fontResult == GBL_RESULT_SUCCESS &&
           defaultFontResult == GBL_RESULT_SUCCESS;
}

int main(void) {
    bool passed = false;
    GUM_Root* pRootA = nullptr;
    GUM_Root* pRootB = nullptr;
    GUM_Texture* pTexture = nullptr;
    GUM_Font* pFont = nullptr;
    GUM_Font* pDefaultA = nullptr;

    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(64, 64, "libGumball resource restart");
    if (!IsWindowReady()) {
        fprintf(stderr, "raylib window/context initialization failed\n");
        return 1;
    }

    SetTraceLogCallback(hostTraceLog_);
    if (!hostLoggerAlive_()) {
        fprintf(stderr, "raylib host logger probe setup failed\n");
        goto cleanup;
    }

    pRootA = GUM_Root_create();
    pTexture = pRootA ? GUM_TEXTURE(GUM_Manager_load("koslogo.png")) : nullptr;
    pFont = pRootA ? GUM_FONT(GUM_Manager_load(GUM_TEST_BITMAP_FONT_PATH)) : nullptr;
    pDefaultA = GUM_Backend_Font_default();
    if (pDefaultA)
        GUM_IResource_ref(GUM_IRESOURCE(pDefaultA));
    const GUM_Vector2 originalSize = pTexture ? GUM_Texture_size(pTexture) : (GUM_Vector2){ 0 };

    if (!pRootA || !pTexture || !pFont || !pDefaultA ||
        !hostLoggerAlive_() ||
        !nestedScissorRestores_() ||
        originalSize.x <= 0 || originalSize.y <= 0 ||
        !drawResources_(pTexture, pFont, pDefaultA)) {
        fprintf(stderr, "Initial raylib resource/logger/scissor use failed\n");
        goto cleanup;
    }

    GUM_unref(pRootA);
    pRootA = nullptr;

    const GUM_Vector2 afterBackendTeardown = GUM_Texture_size(pTexture);
    if (!hostLoggerAlive_() ||
        afterBackendTeardown.x != originalSize.x ||
        afterBackendTeardown.y != originalSize.y) {
        fprintf(stderr, "raylib resources/logger did not survive Root/backend teardown\n");
        goto cleanup;
    }

    pRootB = GUM_Root_create();
    GUM_Font* pDefaultB = GUM_Backend_Font_default();
    if (!pRootB || !pDefaultB ||
        !hostLoggerAlive_() ||
        !nestedScissorRestores_() ||
        !drawResources_(pTexture, pFont, pDefaultA) ||
        !drawResources_(pTexture, pFont, pDefaultB)) {
        fprintf(stderr, "raylib resource/logger/scissor use after Root/backend recreation failed\n");
        goto cleanup;
    }

    GUM_IResource_unref(GUM_IRESOURCE(pDefaultA));
    pDefaultA = nullptr;
    GUM_IResource_unref(GUM_IRESOURCE(pFont));
    pFont = nullptr;
    GUM_IResource_unref(GUM_IRESOURCE(pTexture));
    pTexture = nullptr;
    passed = true;

cleanup:
    if (pDefaultA) GUM_IResource_unref(GUM_IRESOURCE(pDefaultA));
    if (pFont) GUM_IResource_unref(GUM_IRESOURCE(pFont));
    if (pTexture) GUM_IResource_unref(GUM_IRESOURCE(pTexture));
    if (pRootB) GUM_unref(pRootB);
    if (pRootA) GUM_unref(pRootA);
    SetTraceLogCallback(nullptr);
    CloseWindow();
    return passed ? 0 : 1;
}

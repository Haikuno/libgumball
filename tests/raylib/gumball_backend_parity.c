#include "gumball_backend_parity_scene.h"
#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <string.h>

#define GUM_PARITY_PATH_CAPACITY 4096

static const char* outputPath_(int argc, char* pArgv[], char path[GUM_PARITY_PATH_CAPACITY]) {
    if (argc > 2) {
        fprintf(stderr, "usage: %s [output.png]\n", pArgv[0]);
        return nullptr;
    }

    if (argc == 2) return pArgv[1];

    const char* pBasePath = GetApplicationDirectory();
    if (!pBasePath || !*pBasePath) return nullptr;

    const size_t length = strlen(pBasePath);
    const bool hasSeparator = pBasePath[length - 1] == '/' || pBasePath[length - 1] == '\\';
    const int written = snprintf(path, GUM_PARITY_PATH_CAPACITY,
                                 "%s%sbackend-parity-raylib.png",
                                 pBasePath, hasSeparator ? "" : "/");
    return written >= 0 && written < GUM_PARITY_PATH_CAPACITY ? path : nullptr;
}

int main(int argc, char* pArgv[]) {
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(GUM_BACKEND_PARITY_WIDTH, GUM_BACKEND_PARITY_HEIGHT, "libGumball parity");
    if (!IsWindowReady()) return 1;

    char outputPath[GUM_PARITY_PATH_CAPACITY];
    const char* pOutputPath = outputPath_(argc, pArgv, outputPath);
    if (!pOutputPath) {
        CloseWindow();
        return 1;
    }

    GUM_Root* pRoot = GUM_BackendParityScene_create();
    Image image = { 0 };

    BeginDrawing();
    ClearBackground((Color){ 24, 24, 24, 255 });
    const bool rendered = pRoot && GUM_draw() == GBL_RESULT_SUCCESS;
    if (rendered) {
        rlDrawRenderBatchActive();
        image = LoadImageFromScreen();
    }
    EndDrawing();

    const bool saved = image.data && ExportImage(image, pOutputPath);
    if (saved)
        printf("Wrote %s\n", pOutputPath);
    else
        fprintf(stderr, "Failed to render/write %s\n", pOutputPath);

    if (image.data) UnloadImage(image);
    if (pRoot) GUM_unref(pRoot);
    CloseWindow();
    return saved ? 0 : 1;
}

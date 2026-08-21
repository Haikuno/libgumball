#include "gumball_backend_parity_scene.h"
#include <raylib.h>

int main(int argc, char* pArgv[]) {
    if (argc != 2) return 1;

    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(GUM_BACKEND_PARITY_WIDTH, GUM_BACKEND_PARITY_HEIGHT, "libGumball parity");
    if (!IsWindowReady()) return 1;

    RenderTexture2D target = LoadRenderTexture(GUM_BACKEND_PARITY_WIDTH,
                                               GUM_BACKEND_PARITY_HEIGHT);
    GUM_Root* pRoot = GUM_BackendParityScene_create();

    BeginTextureMode(target);
    ClearBackground((Color){ 24, 24, 24, 255 });
    GUM_draw();
    EndTextureMode();

    Image image = LoadImageFromTexture(target.texture);
    ImageFlipVertical(&image);
    const bool saved = ExportImage(image, pArgv[1]);

    UnloadImage(image);
    GUM_unref(pRoot);
    UnloadRenderTexture(target);
    CloseWindow();
    return saved ? 0 : 1;
}

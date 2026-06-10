#include <gumball/gumball.h>
#include <raylib.h>

int main(int argc, char* argv[]) {
    GUM_Root* pRoot = GUM_Root_create();

    InitWindow(1000, 1080, "Containers - Basic");
    SetTargetFPS(60);
    GUM_Container* pContainer = GUM_Container_create("name", "Container", "x", 0.0f, "y", 0.0f,
                                                     "w", 1000.0f, "h", 1080.0f, "minChildSize", 0.01f,
                                                     "scrollable", false, "color", 0x0A0A0AFF);
    GUM_ObjectViewer_create("parent", pContainer, "object", pContainer);

    while (!WindowShouldClose()) {
        GUM_update();
        BeginDrawing();
        ClearBackground(RAYWHITE);

        GUM_draw();

        EndDrawing();
    }

    GUM_unref(pRoot);
    CloseWindow();
    return 0;
}

#include <gumball/gumball.h>
#include <raylib.h>

int main(int argc, char* argv[]) {
    GUM_Root* pRoot = GUM_Root_create();

    InitWindow(800, 1000, "Clipping test");
    SetTargetFPS(144);


    GUM_Container* pOuter = GUM_Container_create("x", 0.0f, "y", 0.0f,
                                                 "w", 800.0f, "h", 1000.0f,
                                                 "border_color",   0x000000FF,
                                                 "color",          0xF0F0F0FF,
                                                 "resizeWidgets",  false,
                                                 "alignWidgets",   true,
                                                 "scrollable",     true);

    GUM_Container* pInner = GUM_Container_create("parent", pOuter,
                                                 "w", 600.0f, "h", 1000.0f,
                                                 "border_color",  0x0000FFFF,
                                                 "resizeWidgets", false,
                                                 "alignWidgets",  true,
                                                 "scrollable",    true);

    GUM_Container* pInner2 = GUM_Container_create("parent", pOuter,
                                                  "w", 600.0f, "h", 1000.0f,
                                                  "border_color",  0x000000FF,
                                                  "color",         0x0F0F6FFF,
                                                  "resizeWidgets", false,
                                                  "alignWidgets",  true,
                                                  "scrollable",    true);

    const unsigned colors[]  = { 0x7C3AEDFF, 0x18B7A5FF, 0xD94F6EFF, 0x5FA84BFF, 0xF2C14EFF };
    const char*    labels[]  = { "Inner A", "Inner B", "Inner C", "Inner D", "Inner E" };
    const char*    labels2[] = { "Inner 1", "Inner 2", "Inner 3", "Inner 4", "Inner 5" };

    for (int i = 0; i < 5; ++i) {
        GUM_Button_create("parent", pInner,
                        "h",     300.0f,
                        "color", colors[i],
                        "label", labels[i]);
    }

    for (int i = 0; i < 5; ++i) {
        GUM_Button_create("parent", pInner2,
                        "h",     300.0f,
                        "color", colors[i],
                        "label", labels2[i]);
    }

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
#include <gumball/gumball.h>
#include <raylib.h>

int main(int argc, char* argv[]) {
    GUM_Root* pRoot = GUM_Root_create();

    InitWindow(640, 480, "Clipping test");
    SetTargetFPS(60);


    GUM_Container* pOuter = GUM_Container_create("x", 70.0f, "y", 40.0f,
                                                 "w", 800.0f, "h", 600.0f,
                                                 "border_color",   0x000000FF,
                                                 "resizeWidgets",  false,
                                                 "alignWidgets",   false,
                                                 "scrollable",     true);

    GUM_Container* pInner = GUM_Container_create("parent", pOuter,
                                                 "x", 100.0f, "y", 60.0f,
                                                 "w", 600.0f, "h", 600.0f,
                                                 "border_color",  0x0000FFFF,
                                                 "resizeWidgets", false,
                                                 "alignWidgets",  true,
                                                 "scrollable",    true);

    const unsigned colors[] = { 0xFF0000FF, 0x00A000FF, 0xFF00FFFF };
    const char*    labels[] = { "Inner A", "Inner B", "Inner C" };

    for (int i = 0; i < 3; ++i) {
        GUM_Widget_create("parent", pInner,
                          "h",     300.0f,
                          "color", colors[i],
                          "label", labels[i]);
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        GUM_draw();

        EndDrawing();
    }

    GUM_unref(pRoot);
    CloseWindow();
    return 0;
}
#include <gumball/gumball.h>
#include <raylib.h>

int main(int argc, char* argv[]) {
    GUM_Root* pRoot = GUM_Root_create();

    InitWindow(640, 480, "Widgets - Custom");
    SetTargetFPS(60);

    // Parameters are passed in as key-value pairs
    GUM_Widget* pWidget = GUM_Widget_create("color", 0xFF0000FF,      // Here we set the widget color to red (RGBA: 255,0,0,255)
                                            "x", 220.0f, "y", 140.0f, // We center the widget in the window
                                            "label", "Hello, World"); // And we add a label

    // REMEMBER: x, y, w (width) and h (height) are float values!
    // So don't forget to add the .0f suffix
    // To check value types and default values, check the documentation

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

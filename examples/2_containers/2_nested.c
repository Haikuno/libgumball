#include <gumball/gumball.h>
#include <raylib.h>

int main(int argc, char* argv[]) {
    GUM_Root* pRoot = GUM_Root_create();

    InitWindow(640, 480, "Containers - Nested");
    SetTargetFPS(60);

    // Containers can be nested inside other containers!
    // Here we create a parent container that holds two child containers,
    // each of which holds three widgets.
    GUM_Container* pParentContainer = GUM_Container_create("x", 70.0f, "y", 40.0f,
                                                           "w", 500.0f, "h", 400.0f,
                                                           "margin", 10.0f); // Add some margin between child containers

    GUM_Container* pContainer1 = GUM_Container_create("parent", pParentContainer,
                                                      "border_color", 0x000000FF, // Add a black border
                                                      "orientation", 'h',         // We set the orientation to horizontal
                                                      "padding", 10.0f);          // Add some padding between the container border
                                                                                  // and its children

    GUM_Container* pContainer2 = GUM_Container_create("parent", pParentContainer,
                                                      "border_color", 0x000000FF,
                                                      "orientation", 'v'); // Vertical is already the default, but no harm in
                                                                           // specifying it again for clarity
    GUM_Widget* pWidget1 = GUM_Widget_create("parent", pContainer1,
                                             "color",  0xFF0000FF,
                                             "label",  "I'm red!");
    GUM_Widget* pWidget2 = GUM_Widget_create("parent", pContainer1,
                                             "color",  0x0000FFFF,
                                             "label",  "I'm blue!");
    GUM_Widget* pWidget3 = GUM_Widget_create("parent", pContainer1,
                                             "color",  0x00A000FF,
                                             "label",  "I'm green!");

    GUM_Widget* pWidget4 = GUM_Widget_create("parent", pContainer2,
                                             "color",  0xFFFF00FF,
                                             "label",  "I'm yellow!");
    GUM_Widget* pWidget5 = GUM_Widget_create("parent", pContainer2,
                                             "color",  0xFF00FFFF,
                                             "label",  "I'm magenta!");
    GUM_Widget* pWidget6 = GUM_Widget_create("parent", pContainer2,
                                             "color",  0x00FFFFFF,
                                             "label",  "I'm cyan!");

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

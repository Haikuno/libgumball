#include <gumball/gumball.h>
#include <raylib.h>

int main(int argc, char* argv[]) {
    GUM_Root* pRoot = GUM_Root_create();

    InitWindow(640, 480, "Test");
    SetTargetFPS(60);

    auto controller = GUM_Controller_create("isKeyboard", true, "color", 0xFFFFFFFFF);

    GUM_Container* pParentContainer = GUM_Container_create(
        "x", 70.0f,  "y", 40.0f,
        "w", 500.0f, "h", 400.0f,
        "color", 0,  "border_color", 0x000000AF,
        "margin", 10.0f, "children",
        GblRingList_create(

            GUM_Container_create(
                "border_color", 0x000000FF, "orientation", 'h',
                "color", 0xF0F000FF,
                "margin", 5.0f, "padding", 10.0f, "children",
                GblRingList_create(
                    GUM_Button_create("color", 0xFF0000FF, "label", "I'm red!",   "border_color", 0xA0A000FF),
                    GUM_Button_create("color", 0x0000FFFF, "label", "I'm blue!",  "border_color", 0xA0A000FF),
                    GUM_Button_create("color", 0x00FF00FF, "label", "I'm green!", "border_color", 0xA0A000FF)
                )
            ),

            GUM_Container_create(
                "border_color", 0x000000FF, "orientation", 'v',
                "color", 0X0F0F0FFF,
                "margin", 3.0f, "children",
                GblRingList_create(
                    GUM_Button_create("color", 0xFFFF00FF, "label", "I'm yellow!",  "font_color", 0x000000FF),
                    GUM_Button_create("color", 0xFF00FFFF, "label", "I'm magenta!", "font_color", 0x000000FF),
                    GUM_Button_create("color", 0x00FFFFFF, "label", "I'm cyan!",    "font_color", 0x000000FF)
                )
            )
        )
    );

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

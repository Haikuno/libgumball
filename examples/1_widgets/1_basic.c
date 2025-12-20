#include <gumball/gumball.h>
#include <raylib.h>

/*
 *	Welcome to the first Gumball example!
 *	This example demonstrates how to create a basic libGumball application
 *	using the GUM_Widget element, the fundamental element of libGumball
 *
 * 	Always remember to check the documentation for more information : https://libgumball.psyops.studio/
 *	Every header file is documented, so you can read them instead of going to the website if you prefer
 *  If your editor supports it, just hovering over functions/macros should give you a brief description of what they do
 */

int main(int argc, char* argv[]) {
    /*
     * Creates the Gumball Root element
     * This is the element that holds all other elements in the scene,
     * and must be created before any other Gumball functions are called
     */
    GUM_Root* pRoot = GUM_Root_create();

    // Raylib window initialization
    InitWindow(640, 480, "Widgets - Basic");
    SetTargetFPS(60);

    // Create a basic widget
    GUM_Widget* pWidget = GUM_Widget_create(); // No parameters pased in -- uses default values
                                               // We'll see how to set properties in chapter 2

    // Main loop
    while (!WindowShouldClose()) {
        // Raylib drawing
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw Gumball scene
        // This automatically draws all elements that you have created
        GUM_draw();

        // End Raylib drawing
        EndDrawing();
    }

    GUM_unref(pRoot); // Always remember to unref your Gumball objects when you're done with them!
                      // Since root holds all other elements, unreffing it will unref all children too
    CloseWindow();
    return 0;
}

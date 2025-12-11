#include <gumball/gumball.h>
#include <raylib.h>

int main(int argc, char *argv[]) {
	GUM_Root *root = GUM_Root_create();

	InitWindow(640, 480, "Containers - Basic");
    SetTargetFPS(60);

	// Manually setting the size and position of each widget gets tiring fast
	// So we can use a GUM_Container to do it for us.
	GUM_Container *container = GUM_Container_create("x", 70.0f, "y", 40.0f,		// Since libGumball is an Object-Oriented library,
													"w", 500.0f, "h", 400.0f);	// We can use properties from parent elements too!
																				// GUM_Container inherits from GUM_Widget,
																				// so all widget properties are valid

	GUM_Widget *widget1 = GUM_Widget_create("parent", container, // Set the container as parent, so it can manage this widget
										    "color", 0xFF0000FF, "label", "I'm red!");

	GUM_Widget *widget2 = GUM_Widget_create("parent", container,
											"color", 0x0000FFFF, "label", "I'm blue!");

	// When running this program, you should see two widgets inside a container perfectly sized and positioned inside the green GUM_Container,
	// even though we never set their x, y, w or h properties!

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);

		GUM_draw();

		EndDrawing();
	}

	GUM_unref(root);
	CloseWindow();
	return 0;
}

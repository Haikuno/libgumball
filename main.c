#include <gumball/gumball.h>
#include <raylib.h>

int main(int argc, char *argv[]) {
	auto root 		= GUM_Root_create();

	InitWindow(320, 240, "UILib Example");
	SetTargetFPS(60);

    auto controller = GUM_Controller_create("controllerId", 1, "color", 0xFF0000FF, "isKeyboard", true, "name", "main controller");

	auto container_parent = GUM_Container_create("a", 0, "alignWidgets", false, "resizeWidgets", false, "isRelative", false, "name", "parent container");

    auto container_1 = GUM_Container_create("x", 10.0f, "y", 10.0f, "w", 120.0f, "h", 150.0f,
										   "color", 0x000000FF,
										   "orientation", 'v',
										   "border_color", 0x000000FF,
										   "parent", container_parent,
											"name", "container 1");

    auto container_2 = GUM_Container_create("x", 140.0f, "y", 10.0f, "w", 170.0f, "h", 150.0f,
										   "color", 0xE08616FF, "border_color", 0x000000FF,
										   "orientation", 'v',
										   "parent", container_parent, "name", "container 2");

	auto container_3 = GUM_Container_create("children", GblRingList_create(
												GUM_Button_create("label", "X", "color", 0xE08616FF, "border_radius", 0.5f, "border_color", 0x000000FF),
												GUM_Button_create("label", "Y", "color", 0xE08616FF, "border_radius", 0.5f, "border_color", 0x000000FF)
										   ));

	// main loop
	while (!WindowShouldClose()) {
		GUM_update(root);

		BeginDrawing();
		ClearBackground(RAYWHITE);
		GUM_draw(nullptr);
		EndDrawing();
	}

	// GUM_Manager_unload(pRes);

	GUM_unref(root);
	CloseWindow();
	return 0;
}

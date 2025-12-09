#include <gumball/gumball.h>
#include <raylib.h>

int main(int argc, char *argv[]) {
	auto root             = GUM_Root_create();
	auto controller       = GUM_Controller_create("isKeyboard", true);

    InitWindow(320, 240, "libGumball");
    SetTargetFPS(60);

	auto texture          = GUM_Manager_load("Vmu.png");

	auto parent_container = GUM_Container_create("color", 0, "w", 320.0f, "h", 240.0f, "orientation", 'h', "alignWidgets", false);

	auto container        = GUM_Container_create("color", 0, "parent", parent_container,
		"children", GblRingList_create(
		GUM_Button_create("name", "1", "border_radius", 2.0f, "color", 0x0000FFFF, "isSelectable", false),
		GUM_Button_create("name", "2", "border_radius", 2.0f, "color", 0x0000FFFF),
		GUM_Button_create("name", "3", "border_radius", 2.0f, "color", 0x0000FFFF),
		GUM_Button_create("name", "3", "border_radius", 2.0f, "color", 0x0000FFFF),
		GUM_Button_create("name", "3", "border_radius", 2.0f, "color", 0x0000FFFF),
		GUM_Button_create("name", "3", "border_radius", 2.0f, "color", 0x0000FFFF),
		GUM_Button_create("name", "3", "border_radius", 2.0f, "color", 0x0000FFFF),
		GUM_Button_create("name", "3", "border_radius", 2.0f, "color", 0x0000FFFF),
		GUM_Button_create("name", "3", "border_radius", 2.0f, "color", 0x0000FFFF),
		GUM_Button_create("name", "3", "border_radius", 2.0f, "color", 0x0000FFFF)
	));

	auto container_2      = GUM_Container_create("color", 0, "parent", parent_container,
	"x", 400.0f, "border_radius", 0.8f, "orientation", 'h',
	"children", GblRingList_create(
		GUM_Button_create("color", 0x0000FFFF),
		GUM_Button_create("color", 0x0000FFFF, "isSelectedByDefault", true),
		GUM_Button_create("color", 0x0000FFFF),
		GUM_Button_create("color", 0x0000FFFF),
		GUM_Button_create("color", 0x0000FFFF),
		GUM_Button_create("color", 0x0000FFFF),
		GUM_Button_create("color", 0x0000FFFF)
	));

	// main loop
	while (!WindowShouldClose()) {
	    // update
		GUM_update();

		// drawing
		GBL_SCOPE(BeginDrawing(), EndDrawing()) {
    		ClearBackground(RAYWHITE);
    		GUM_draw();
		}
	}

	GUM_unref(container);
	GUM_Manager_unload(texture);
	CloseWindow();
	return 0;
}

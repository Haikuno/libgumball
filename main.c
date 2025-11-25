#include <gumball/gumball.h>
#include <raylib.h>

void initRaylib(void) {
	InitWindow(320, 240, "libGumball");
}

int main(int argc, char *argv[]) {
	initRaylib();

	auto root = GUM_Root_create();
	auto texture = GUM_Manager_load("../Vmu.png");
	auto controller = GUM_Controller_create("isKeyboard", true);

	auto parent_container = GUM_Container_create("color", 0, "w", 320.0f, "h", 240.0f, "orientation", 'h');

	auto container = GUM_Container_create("parent", parent_container,
		"children", GblRingList_create(
		GUM_Button_create("name", "1", "border_radius", 2.0f, "color", 0x0000FFFF, "isSelectable", false),
		GUM_Button_create("name", "2", "border_radius", 2.0f, "color", 0xF0F0F0FF),
		GUM_Button_create("name", "3", "border_radius", 2.0f, "color", 0xFF0000FF)
	));


	auto container_2 = GUM_Container_create("x", 200.0f, "parent", parent_container, "border_radius", 0.8f, "orientation", 'v',
		"children", GblRingList_create(
		GUM_Button_create("color", 0x0000FFFF),
		GUM_Button_create("color", 0xF0F0F0FF, "isSelectedByDefault", true),
		GUM_Button_create("color", 0xFF0000FF)
	));

	// GUM_update_disableAll(container_2);
	// GUM_draw_disableAll(container_2);

	// main loop
	while (!WindowShouldClose()) {
		GUM_update();

		BeginDrawing();
		ClearBackground(RAYWHITE);
		GUM_draw();

		static float timer = 0;
		static int frames = 0;
		static float avgFps = 0;

		float dt = GetFrameTime();
		timer += dt;
		frames++;

		if (timer >= 2.0f) {
			avgFps = frames / timer;
			timer = 0;
			frames = 0;
		}

		DrawText(TextFormat("FPS: %.1f", avgFps), 10, 10, 20, WHITE);



		EndDrawing();
	}

	GUM_Manager_unload(texture);
	GUM_unref(root);
	CloseWindow();
	return 0;
}


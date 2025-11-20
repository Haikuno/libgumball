#include <gumball/gumball.h>
#include <raylib.h>

void initRaylib(void) {
	InitWindow(640, 480, "UILib Example");
	SetTargetFPS(60);
}

int main(int argc, char *argv[]) {
	auto root = GUM_Root_create();

	initRaylib();
	auto font    = GUM_Manager_load("../Nimbus.fnt");
	auto texture = GUM_Manager_load("../babyfish_test.png");

	#define prop_list(x) "label", x, "font_size", 32, "color", 0xFF000FFF, "border_color", 0xAAAAAAFF

	auto widget = GUM_Widget_create(prop_list("test"), "font", font, "texture", texture);

	auto container   = GUM_Container_create("x", 160.0f, "y", 120.0f, "w", 320.0f, "h", 240.0f,
										    "color", 0x1E1E1EFF,
											"children",
											GblRingList_create(
												widget,
												GUM_Widget_create(prop_list("test")))
											);
	#undef prop_list

	int count = 0;

	// main loop
	while (!WindowShouldClose()) {
		GUM_update(root);


		BeginDrawing();
		ClearBackground(RAYWHITE);
		GUM_draw();
		EndDrawing();

		GUM_setProperty(widget, "border_color", 0x666666FF);

		if ((int)GetTime() % 2 == 0)
			GUM_setProperty(widget, "border_color", 0xEEFFFFFF);

	}

	GUM_unref(root);
	CloseWindow();
	return 0;
}


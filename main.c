#include <gumball/gumball.h>
#include <raylib.h>

void initRaylib(void) {
	InitWindow(320, 240, "UILib Example");
	SetTargetFPS(60);
}

GUM_Root *scene_a_root = nullptr;
GUM_Root *scene_b_root = nullptr;
GUM_Root *current_root = nullptr;

void init_scene_a(void) {
    scene_a_root = GUM_Root_create();
}

void deinit_scene_a(void) {
    GUM_unref(scene_a_root);
}

void init_scene_b(void) {
    scene_b_root = GUM_Root_create();
}

void deinit_scene_b(void) {
    GUM_unref(scene_b_root);
}

void a_callback(GUM_Button *pSelf) {
	deinit_scene_b();
	init_scene_a();
	current_root = scene_a_root;
}

void b_callback(GUM_Button *pSelf) {
	deinit_scene_a();
	init_scene_b();
	current_root = scene_b_root;
}

int main(int argc, char *argv[]) {


	initRaylib();
    init_scene_a();
	current_root = scene_a_root;


	auto controller = GUM_Controller_create("isKeyboard", true);
    auto container = GUM_Container_create();
    auto button_a = GUM_Button_create("parent", container, "label", "A");
    auto button_b = GUM_Button_create("parent", container, "label", "B", "x", 100.0f);

	GUM_connect(button_a, "onPressPrimary", a_callback);
	GUM_connect(button_b, "onPressPrimary", b_callback);


	// main loop
	while (!WindowShouldClose()) {
		GUM_update(current_root);

		if ((int)GetTime() % 2 == 0) {
			GUM_LOG_DEBUG("current root is %p", current_root);
			GUM_LOG_DEBUG("scene_a_root is %p", scene_a_root);
			GUM_LOG_DEBUG("scene_b_root is %p", scene_b_root);
			GUM_LOG_DEBUG("use count of current root: %zu", GblModule_useCount(GBL_MODULE(current_root)));
			GUM_LOG_DEBUG("-----------------------------------------------------------------------------");
		}

		BeginDrawing();
		ClearBackground(RAYWHITE);
		GUM_draw();
		EndDrawing();
	}

	GUM_unref(current_root);
	CloseWindow();
	return 0;
}


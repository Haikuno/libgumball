#include <gumball/gumball.h>
#include <raylib.h>

void initRaylib(void) {
	InitWindow(320, 240, "UILib Example");
	SetTargetFPS(60);
}

int main(int argc, char *argv[]) {
	initRaylib();

	// main loop
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}


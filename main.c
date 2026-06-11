#include <raylib.h>

int main(void) {
	const int screenWidth = 800;
	const int screenHeight = 600;
	InitWindow(screenWidth, screenHeight, "rubixed - 3d rubik's cube visualizer & solver");

	SetTargetFPS(60);

	while(!WindowShouldClose()) {
		BeginDrawing();
		EndDrawing();
	}
	
	CloseWindow();

	return 0;
}
#include <raylib.h>
#include "cube.h"

int main(void) {
	RubiksCube myCube;
	init_cube(&myCube);

	const int screenWidth = 800;
	const int screenHeight = 600;
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "rubixed - 3d rubik's cube visualizer & solver");
	SetWindowMinSize(800, 600);

	Camera3D camera = { 0 };
	camera.position = (Vector3){ 0.0f, 4.5f, 4.5f };
	camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	SetTargetFPS(240);

	while(!WindowShouldClose()) {
		int currentWidth = GetScreenWidth();
		int currentHeight = GetScreenHeight();
		if (currentHeight <= 0) currentHeight = 1;

		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		EndMode3D();

		DrawText("rubixed", 10, 40, 20 * (currentHeight / 600), DARKBLUE);
		DrawFPS(10, 10);

		EndDrawing();
	}
	
	CloseWindow();

	return 0;
}
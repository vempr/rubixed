#include <math.h>
#include <raylib.h>
#include "cube.h"

int main(void) {
	RubiksCube myCube;
	init_cube(&myCube);

	const int screenWidth = 800;
	const int screenHeight = 600;
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "rubixed - 3d rubik's cube visualizer & solver");
	SetWindowMinSize(800, 600);

	Camera3D camera = { 0 };
	camera.position = (Vector3){ 0.0f, 5.5f, 5.5f };
	camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
	camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	float cameraRadius = sqrt(pow(5.5, 2.0) + pow(5.5, 2.0));
	float cameraAlpha = 0.0f;
	float cameraBeta = PI / 4;
	Vector2 previousMousePosition = {0.0f, 0.0f};

	SetTargetFPS(240);

	while(!WindowShouldClose()) {
		int currentWidth = GetScreenWidth();
		int currentHeight = GetScreenHeight();
		if (currentHeight <= 0) currentHeight = 1;

		Vector2 currentMousePosition = GetMousePosition();

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
			float deltaX = currentMousePosition.x - previousMousePosition.x;
			float deltaY = currentMousePosition.y - previousMousePosition.y;

			cameraAlpha -= deltaX * 0.004f;
			cameraBeta += deltaY * 0.004f;

			if (cameraBeta > PI / 2) cameraBeta = PI / 2;
			if (cameraBeta < -PI / 2) cameraBeta = -PI / 2;
		}

		previousMousePosition = currentMousePosition;

		// cartesian to spherical conversion
		camera.position.x = cameraRadius * cosf(cameraBeta) * sinf(cameraAlpha);
		camera.position.y = cameraRadius * sinf(cameraBeta);
		camera.position.z = cameraRadius * cosf(cameraAlpha) * cosf(cameraBeta);

		float wheelMove = GetMouseWheelMove();
		if (wheelMove != 0.0f) {
			cameraRadius -= wheelMove * 0.4f;
			if (cameraRadius < 4.0f) cameraRadius = 4.0f;
			if (cameraRadius > 16.0f) cameraRadius = 16.0f;
		}

		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode3D(camera);
		draw_cube(&myCube);
		EndMode3D();

		DrawText("rubixed", 10, 40, 20 * (currentHeight / 600), DARKBLUE);
		DrawFPS(10, 10);

		EndDrawing();
	}
	
	CloseWindow();

	return 0;
}
#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include "cube.h"
#include "scramble.h"

int main(void) {
	RubiksCube myCube;
	init_cube(&myCube);
	scramble_init("cache");
	apply_scramble(&myCube, "U D");
	char* currentScramble = NULL;

	CubeAnim anim = {0};
	anim.active = 0;

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

		if (anim.active) {
			anim.angle += 360.0f * GetFrameTime();
			if (anim.angle >= 90.0f) {
				anim.angle = 90.0f;
				anim.active = 0;
			}
		}

		handle_cube_inputs(&myCube, &anim);

		if (IsKeyPressed(KEY_ENTER)) {
			free(currentScramble);
			init_cube(&myCube);
			anim = (CubeAnim){ 0 };

			char* solutionStr = generate_scramble(21);
			currentScramble = invert_scramble(solutionStr);

			printf("solution: [%s]\n", solutionStr);
			printf("inverse: [%s]\n", currentScramble);
			
			apply_scramble(&myCube, currentScramble);
			free(solutionStr);
		} else if (IsKeyPressed(KEY_BACKSPACE)) {
			free(currentScramble);
			init_cube(&myCube);
			anim = (CubeAnim){ 0 };

			currentScramble = NULL;
		}

		Vector2 currentMousePosition = GetMousePosition();

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
			float deltaX = currentMousePosition.x - previousMousePosition.x;
			float deltaY = currentMousePosition.y - previousMousePosition.y;

			cameraAlpha -= deltaX * 0.004f;
			cameraBeta += deltaY * 0.004f;

			float lim = PI / 2 - 0.01;
			if (cameraBeta > lim) cameraBeta = lim;
			if (cameraBeta < -lim) cameraBeta = -lim;
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

		ClearBackground((Color){238, 255, 204});

		BeginMode3D(camera);
		draw_cube(&myCube, &anim);
		EndMode3D();

		int font_multi = currentHeight / 600;

		DrawText(
			"rubixed",
			10,
			40,
			20 * font_multi,
			BLACK
		);
		DrawText(
			currentScramble ? currentScramble : "",
			(currentWidth - MeasureText(currentScramble, 30 * font_multi)) / 2,
			20,
			30 * font_multi,
			BLACK
		);
		DrawFPS(10, 10);

		EndDrawing();
	}
	
	CloseWindow();

	return 0;
}
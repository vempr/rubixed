#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cube.h"
#include "scramble.h"
#include "anim.h"

int main(void) {
	RubiksCube myCube;
	init_cube(&myCube);
	scramble_init("cache");
	char* currentScramble = NULL;

	CubeAnim anim = {0};
	anim.active = 0;
	ScrambleAnim scrAnim = {0};
	scrAnim.active = 0;
	PendingMove pendingMove = {0};

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
			anim.angle += 720.0f * GetFrameTime();
			if (anim.angle >= 90.0f) {
				anim.angle = 90.0f;
				anim.active = 0;

				if (pendingMove.active) {
					rotate_face(&myCube, pendingMove.face, pendingMove.clockwise);
					pendingMove.active = 0;
				}
			}
		}

		handle_cube_inputs(&myCube, &anim);

		if (scrAnim.active && !anim.active) {
			if (scrAnim.current < scrAnim.moveCount) {
				char* move = scrAnim.moves[scrAnim.current];
				
				int face;
				int clockwise = 1;

				switch(move[0]) {
					case 'R': face = FACE_RIGHT; break;
					case 'L': face = FACE_LEFT; break;
					case 'U': face = FACE_UP; break;
					case 'D': face = FACE_DOWN; break;
					case 'F': face = FACE_FRONT; break;
					case 'B': face = FACE_BACK; break;
					default:
						fprintf(stderr, "Critical move: %s\n", move[0]);
						break;
				}

				if (move[1] == '\'') clockwise = 0;

				anim.active = 1;
				anim.dir = clockwise;
				anim.angle = 0.0f;
				anim.pieceCount = 0;
				anim.axis =
					face == FACE_RIGHT || face == FACE_LEFT ? AXIS_X :
					face == FACE_UP || face == FACE_DOWN ? AXIS_Y :
					AXIS_Z;
				
				int layer = face == FACE_RIGHT || face == FACE_UP || face == FACE_FRONT ? 1 : -1;

				for (int i = 0; i < 27; i++) {
					Cube *p = &myCube.pieces[i];

					int selected = 0;

					if (anim.axis == AXIS_X && p->x == layer) selected = 1;
					if (anim.axis == AXIS_Y && p->y == layer) selected = 1;
					if (anim.axis == AXIS_Z && p->z == layer) selected = 1;

					if (selected) {
						int idx = anim.pieceCount++;
						anim.indices[idx] = i;
						anim.startPos[idx] = (Vector3){(float)p->x, (float)p->y, (float)p->z};
						anim.startOrient[idx] = p->orient;
						memcpy(anim.startColors[idx], p->colors, 6*sizeof(int));
					}
				}

				pendingMove.active = 1;
				pendingMove.face = face;
				pendingMove.clockwise = clockwise;

				scrAnim.current++;
			} else {
				scrAnim.active = 0;
			}
		}

		if (IsKeyPressed(KEY_ENTER)) {
			free(currentScramble);
			init_cube(&myCube);
			anim = (CubeAnim){ 0 };

			char* solutionStr = generate_scramble(21);
			currentScramble = invert_scramble(solutionStr);

			printf("solution: [%s]\n", solutionStr);
			printf("inverse: [%s]\n", currentScramble);
			
			parse_scramble(currentScramble, &scrAnim);

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
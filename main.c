#include <math.h>
#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cube/cube.h"
#include "cube/anim.h"
#include "cube/draw.h"
#include "camera/camera.h"
#include "app/app.h"
#include "scramble/scramble.h"
#include "scramble/scramble_engine.h"

int main(void) {
	App app = {0};
	OrbitCamera appCamera;

	char* title = "rubixed - 3d rubik's cube visualizer & solver";
	init_app_window(800, 600, title);
	init_camera(&appCamera);
	init_app_cube(&app);

	ScrambleEngine engine = {
		.cube = &app.cube,
		.anim = &app.anim,
		.scrAnim = &app.scrAnim,
		.pendingMove = &app.pendingMove,
	};

	SetTargetFPS(240);

	while(!WindowShouldClose()) {
		update_animation(&app);
		handle_cube_inputs(&app.cube, &app.anim);

		scramble_engine_update(&engine);

		handle_app_kb_shortcuts(&app);
		camera_update(&appCamera);
		app_draw(&app, &appCamera);
	}
	
	CloseWindow();

	return 0;
}
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

#if defined(_WIN32)
	#include <direct.h>
	#define mkdir_cache() _mkdir("cache")
#else
	#include <sys/stat.h>
	#define mkdir_cache() mkdir("cache", 0777)
#endif

int main(int argc, char* argv[]) {
	ChangeDirectory(GetApplicationDirectory());
	mkdir_cache();
	
	App app = {0};
	OrbitCamera appCamera;

	char* title = "rubixed - 3d rubik's cube timer & solver";
	init_app_window(1400, 1000, title);
	init_camera(&appCamera);
	init_app_cube(&app);

	SetTargetFPS(60);

	while(!WindowShouldClose()) {
		handle_app_kb_shortcuts(&app);
		app_update(&app);
		camera_update(&appCamera);
		app_draw(&app, &appCamera);
	}
	
	CloseWindow();

	return 0;
}
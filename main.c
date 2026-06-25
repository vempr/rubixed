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
	#include <windows.h>
	#define mkdir_cache() _mkdir("cache")
#else
	#include <unistd.h>
	#include <libgen.h>
	#include <sys/stat.h>
	#define mkdir_cache() mkdir("cache", 0777)
#endif

void set_working_directory_to_exe(int argc, char* argv[]) {
	#if defined(_WIN32)
		char exe_path[MAX_PATH];
		if (GetModuleFileNameA(NULL, exe_path, MAX_PATH) != 0) {
			char* last_slash = strrchr(exe_path, '\\');
			if (last_slash != NULL) {
				*last_slash = '\0';
				_chdir(exe_path);
			}
		}
	#else
		if (argc > 0 && argv[0] != NULL) {
			char* dir = strdup(argv[0]);
			if (dir) {
				char* dir_path = dirname(dir);
				int chdir_res = chdir(dir_path);
				(void)chdir_res;
				free(dir);
			}
		}
	#endif
}

int main(int argc, char* argv[]) {
	set_working_directory_to_exe(argc, argv);
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
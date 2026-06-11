#include "cube.h"

const int COLOR_INTERNAL = 0; // black (non-stickered internals are black in cubes)
const int COLOR_UP = 1; // white
const int COLOR_DOWN = 2; // yellow
const int COLOR_LEFT = 3; // orange
const int COLOR_RIGHT = 4; // red
const int COLOR_FRONT = 5; // green
const int COLOR_BACK = 6; // blue

void init_cube(RubiksCube *cube) {
	for (int i = 0; i < 27; i++) {
		int grid_x = i % 3;
		int grid_y = (i / 3) % 3;
		int grid_z = i / 9;
		
		// transform all coordinates by -1 so center is (0,0,0)
		cube->pieces[i].x = grid_x - 1;
		cube->pieces[i].y = grid_y - 1;
		cube->pieces[i].z = grid_z - 1;

		for (int j = 0; j < 6; j++) {
			cube->pieces[i].colors[j] = COLOR_INTERNAL;
		}

		// x is left-right, y is up-down, z is front-back (because raylib conventions...)
		if (cube->pieces[i].x == 1) cube->pieces[i].colors[FACE_RIGHT] = COLOR_RIGHT;
		if (cube->pieces[i].x == -1) cube->pieces[i].colors[FACE_LEFT] = COLOR_LEFT;
		if (cube->pieces[i].y == 1) cube->pieces[i].colors[FACE_UP] = COLOR_UP;
		if (cube->pieces[i].y == -1) cube->pieces[i].colors[FACE_DOWN] = COLOR_DOWN;
		if (cube->pieces[i].z == 1) cube->pieces[i].colors[FACE_FRONT] = COLOR_FRONT;
		if (cube->pieces[i].z == -1) cube->pieces[i].colors[FACE_BACK] = COLOR_BACK;
	}
}

void rotate_face(RubiksCube *cube, enum FACE face, int clockwise) {
	
}

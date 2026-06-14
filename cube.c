#include <raylib.h>
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

void cycle_colors(int colors[6], enum FACE f1,  enum FACE f2,  enum FACE f3,  enum FACE f4, int clockwise) {
	if (clockwise == 1) {
		int temp = colors[f4];
		colors[f4] = colors[f3];
		colors[f3] = colors[f2];
		colors[f2] = colors[f1];
		colors[f1] = temp;
	} else {
		int temp = colors[f1];
		colors[f1] = colors[f2];
		colors[f2] = colors[f3];
		colors[f3] = colors[f4];
		colors[f4] = temp;
	}
}

static void rotate_piece(Cube *p, Axis axis, int dir) {
	int x = p->x;
	int y = p->y;
	int z = p->z;

	switch(axis) {
		case AXIS_X:
			p->y = dir ? -z : z;
			p->z = dir ? y : -y;

			cycle_colors(
				p->colors,
				FACE_UP, 
				FACE_BACK,
				FACE_DOWN,
				FACE_FRONT,
				dir
			);
			break;

		case AXIS_Y:
			p->x = dir ? -z : z;
			p->z = dir ? x : -x;

			cycle_colors(
				p->colors,
				FACE_FRONT,
				FACE_RIGHT,
				FACE_BACK,
				FACE_LEFT,
				dir
			);
			break;

		case AXIS_Z:
			p->x = dir ? -y : y;
			p->y = dir ? x : -x;

			cycle_colors(
				p->colors,
				FACE_UP,
				FACE_RIGHT,
				FACE_DOWN,
				FACE_LEFT,
				dir
			);
			break;
	}
}

void rotate_face(RubiksCube *cube, enum FACE face, int clockwise) {
	Axis axis;
	int layer;
	int dir;

	switch(face) {
		case FACE_RIGHT:
			axis = AXIS_X;
			layer = 1;
			dir = clockwise;
			break;
		
		case FACE_LEFT:
			axis = AXIS_X;
			layer = -1;
			dir = -clockwise;
			break;
		
		case FACE_UP:
			axis = AXIS_Y;
			layer = 1;
			dir = clockwise;
			break;
		
		case FACE_DOWN:
			axis = AXIS_Y;
			layer = -1;
			dir = !clockwise;
			break;

		case FACE_FRONT:
			axis = AXIS_Z;
			layer = 1;
			dir = clockwise;
			break;
		
		case FACE_BACK:
			axis = AXIS_Z;
			layer = -1;
			dir = !clockwise;
			break;
	}

	for (int i = 0; i < 27; i++) {
		Cube *p = &cube->pieces[i];

		int selected = 0;

		if (axis == AXIS_X && p->x == layer) selected = 1;
		if (axis == AXIS_Y && p->y == layer) selected = 1;
		if (axis == AXIS_Z && p->z == layer) selected = 1;

		if (selected) rotate_piece(p, axis, dir);
	}
}

static void rotate_cube_axis(RubiksCube *cube, Axis axis, int clockwise) {
	int dir = clockwise ? 1 : 0;
	for (int i = 0; i < 27; i++) {
		rotate_piece(&cube->pieces[i], axis, dir);
	}
}

void handle_cube_inputs(RubiksCube *cube) {
	int shiftActive = 0;

	if (IsKeyDown(KEY_LEFT_SHIFT)) {
		shiftActive = 1;
		if (IsKeyPressed(KEY_I)) { rotate_cube_axis(cube, AXIS_X, 1); }
		if (IsKeyPressed(KEY_F)) { rotate_cube_axis(cube, AXIS_X, 1); }
		if (IsKeyPressed(KEY_H)) { rotate_cube_axis(cube, AXIS_Y, 1); }
	}

	if (IsKeyDown(KEY_RIGHT_SHIFT)) {
		shiftActive = 1;
		if (IsKeyPressed(KEY_I)) { rotate_cube_axis(cube, AXIS_Y, 0); }
		if (IsKeyPressed(KEY_F)) { rotate_cube_axis(cube, AXIS_Z, 0); }
		if (IsKeyPressed(KEY_H)) { rotate_cube_axis(cube, AXIS_Z, 0); }
	}

	if (shiftActive) return;

	int activeTarget = -1;
	int isClockwise = 0;

	if (IsKeyPressed(KEY_I)) { activeTarget = FACE_RIGHT; isClockwise = 1; }
	else if (IsKeyPressed(KEY_K)) { activeTarget = FACE_RIGHT; isClockwise = 0; }
	else if (IsKeyPressed(KEY_E)) { activeTarget = FACE_LEFT; isClockwise = 1; }
	else if (IsKeyPressed(KEY_D)) { activeTarget = FACE_LEFT; isClockwise = 0; }
	else if (IsKeyPressed(KEY_F)) { activeTarget = FACE_UP; isClockwise = 1; }
	else if (IsKeyPressed(KEY_J)) { activeTarget = FACE_UP; isClockwise = 0; }
	else if (IsKeyPressed(KEY_S)) { activeTarget = FACE_DOWN; isClockwise = 1; }
	else if (IsKeyPressed(KEY_W)) { activeTarget = FACE_DOWN; isClockwise = 0; }
	else if (IsKeyPressed(KEY_H)) { activeTarget = FACE_FRONT; isClockwise = 1; }
	else if (IsKeyPressed(KEY_G)) { activeTarget = FACE_FRONT; isClockwise = 0; }
	else if (IsKeyPressed(KEY_R)) { activeTarget = FACE_BACK; isClockwise = 1; }
	else if (IsKeyPressed(KEY_Y)) { activeTarget = FACE_BACK; isClockwise = 0; }
	else { return; }

	rotate_face(cube, activeTarget, isClockwise);
}
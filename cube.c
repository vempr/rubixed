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
	if (clockwise) {
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

void rotate_face(RubiksCube *cube, enum FACE face, int clockwise) {
	for (int i = 0; i < 27; i++) {
		Cube *p = &cube->pieces[i];

		int should_rotate = 0;
		if (face == FACE_RIGHT && p->x == 1) should_rotate = 1;
		if (face == FACE_LEFT && p->x == -1) should_rotate = 1;
		if (face == FACE_UP && p->y == 1) should_rotate = 1;
		if (face == FACE_DOWN && p->y == -1) should_rotate = 1;
		if (face == FACE_FRONT && p->z == 1) should_rotate = 1;
		if (face == FACE_BACK && p->z == -1) should_rotate = 1;

		if (!should_rotate) continue;

		// update piece positions, then colors

		int old_x = p->x;
		int old_y = p->y;
		int old_z = p->z;

		if (face == FACE_RIGHT) {
			p->y = clockwise ? -old_z : old_z;
			p->z = clockwise ? old_y : -old_y;
			cycle_colors(p->colors, FACE_UP, FACE_BACK, FACE_DOWN, FACE_FRONT, clockwise);
		} else if (face == FACE_LEFT) {
			p->y = clockwise ? -old_z : old_z;
			p->z = clockwise ? old_y : -old_y;
			cycle_colors(p->colors, FACE_UP, FACE_FRONT, FACE_DOWN, FACE_BACK, clockwise);
		} else if (face == FACE_UP) {
			p->x = clockwise ? old_z : -old_z;
			p->z = clockwise ? -old_x : old_x;
			cycle_colors(p->colors, FACE_FRONT, FACE_LEFT, FACE_BACK, FACE_RIGHT, clockwise);
		} else if (face == FACE_DOWN) {
			p->x = clockwise ? old_z : -old_z;
			p->z = clockwise ? -old_x : old_x;
			cycle_colors(p->colors, FACE_FRONT, FACE_RIGHT, FACE_BACK, FACE_LEFT, clockwise);
		} else if (face == FACE_FRONT) {
			p->x = clockwise ? -old_y : old_y;
			p->y = clockwise ? old_x : -old_x;
			cycle_colors(p->colors, FACE_UP, FACE_RIGHT, FACE_DOWN, FACE_LEFT, clockwise);
		} else if (face == FACE_BACK) {
			p->x = clockwise ? -old_y : old_y;
			p->y = clockwise ? old_x : -old_x;
			cycle_colors(p->colors, FACE_UP, FACE_LEFT, FACE_DOWN, FACE_RIGHT, clockwise);
		}
	}
}

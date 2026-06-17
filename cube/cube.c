#include <raylib.h>
#include <raymath.h>
#include <string.h>
#include <stdio.h>
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
	
		cube->pieces[i].orient = QuaternionIdentity();
	}
}

// cycle_colors is deprecated. new system does not use manual sticker rotation

// void cycle_colors(int colors[6], enum FACE f1,  enum FACE f2,  enum FACE f3,  enum FACE f4, int clockwise) {
// 	if (clockwise == 1) {
// 		int temp = colors[f4];
// 		colors[f4] = colors[f3];
// 		colors[f3] = colors[f2];
// 		colors[f2] = colors[f1];
// 		colors[f1] = temp;
// 	} else {
// 		int temp = colors[f1];
// 		colors[f1] = colors[f2];
// 		colors[f2] = colors[f3];
// 		colors[f3] = colors[f4];
// 		colors[f4] = temp;
// 	}
// }

// deprecated: replaced with quaternion system
// static int rotate_face_index(int f, Axis axis, int dir) {
// 	switch (axis) {
// 		case AXIS_X:
// 			if (f == FACE_UP) return dir ? FACE_BACK : FACE_FRONT;
// 			if (f == FACE_BACK) return dir ? FACE_DOWN : FACE_UP;
// 			if (f == FACE_DOWN) return dir ? FACE_FRONT : FACE_BACK;
// 			if (f == FACE_FRONT) return dir ? FACE_UP : FACE_DOWN;
// 			return f;
		
// 		case AXIS_Y:
// 			if (f == FACE_FRONT) return dir ? FACE_RIGHT : FACE_LEFT;
// 			if (f == FACE_RIGHT) return dir ? FACE_BACK : FACE_FRONT;
// 			if (f == FACE_BACK) return dir ? FACE_LEFT : FACE_RIGHT;
// 			if (f == FACE_LEFT) return dir ? FACE_FRONT : FACE_BACK;
// 			return f;
		
// 		case AXIS_Z:
// 			if (f == FACE_UP) return dir ? FACE_RIGHT : FACE_LEFT;
// 			if (f == FACE_LEFT) return dir ? FACE_UP : FACE_DOWN;
// 			if (f == FACE_DOWN) return dir ? FACE_LEFT : FACE_RIGHT;
// 			if (f == FACE_RIGHT) return dir ? FACE_DOWN : FACE_UP;
// 			return f;
// 	}

// 	return f;
// }

static Quaternion get_rotation_quaternion(Axis axis, int dir) {
	Vector3 axisVec;
	switch (axis) {
		case AXIS_X: axisVec = (Vector3){1.0f, 0.0f, 0.0f}; break;
		case AXIS_Y: axisVec = (Vector3){0.0f, 1.0f, 0.0f}; break;
		case AXIS_Z: axisVec = (Vector3){0.0f, 0.0f, 1.0f}; break;
	}

	return QuaternionFromAxisAngle(axisVec, dir ? -PI/2 : PI/2);
}

static void rotate_piece(Cube *p, Axis axis, int dir) {
	// int x = p->x, y = p->y, z = p->z;

	// switch(axis) {
	// 	case AXIS_X:
	// 		p->y = dir ? z : -z;
	// 		p->z = dir ? -y : y;
	// 		break;

	// 	case AXIS_Y:
	// 		p->x = dir ? -z : z;
	// 		p->z = dir ? x : -x;
	// 		break;

	// 	case AXIS_Z:
	// 		p->x = dir ? y : -y;
	// 		p->y = dir ? -x : x;
	// 		break;
	// }

	Quaternion q = get_rotation_quaternion(axis, dir);

	Vector3 pos = {(float)p->x, (float)p->y, (float)p->z};
	Vector3 rotated = Vector3Transform(pos, QuaternionToMatrix(q));

	p->x = (int)lroundf(rotated.x);
	p->y = (int)lroundf(rotated.y);
	p->z = (int)lroundf(rotated.z);

	p->orient = QuaternionNormalize(QuaternionMultiply(q, p->orient));
	
	// deprecated: replaced with quaternion system
	// printf("Piece at (%d,%d,%d) rotating on axis %d. Result: (%d,%d,%d)\n",
	// 	x, y, z, axis, p->x, p->y, p->z);

	// int temp[6];
	// for (int i = 0; i < 6; i++) {
	// 	temp[i] = p->colors[i];
	// }

	// for (int i = 0; i < 6; i++) {
	// 	int new_face = rotate_face_index(i, axis, dir);
	// 	p->colors[i] = temp[new_face];
	// }
}

void rotate_face(RubiksCube *cube, Face face, int clockwise) {
	Axis axis;
	int layer, dir;

	switch(face) {
		case FACE_RIGHT:
			axis = AXIS_X;
			layer = 1;
			dir = clockwise ? 1 : 0;
			break;
		
		case FACE_LEFT:
			axis = AXIS_X;
			layer = -1;
			dir = clockwise ? 0 : 1;
			break;
		
		case FACE_UP:
			axis = AXIS_Y;
			layer = 1;
			dir = clockwise ? 1 : 0;
			break;
		
		case FACE_DOWN:
			axis = AXIS_Y;
			layer = -1;
			dir = clockwise ? 0 : 1;
			break;

		case FACE_FRONT:
			axis = AXIS_Z;
			layer = 1;
			dir = clockwise ? 1 : 0;
			break;
		
		case FACE_BACK:
			axis = AXIS_Z;
			layer = -1;
			dir = clockwise ? 0 : 1;
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
	for (int i = 0; i < 27; i++) {
		rotate_piece(&cube->pieces[i], axis, clockwise);
	}
}

void handle_cube_inputs(RubiksCube *cube, CubeAnim *anim) {
	if (anim->active) return;

	int shiftActive = 0;
	if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
		shiftActive = 1;
	}

	if (shiftActive) {
		Axis axis;
		int clockwise = -1;

		if (IsKeyPressed(KEY_I) && IsKeyDown(KEY_LEFT_SHIFT)) {
			axis = AXIS_X;
			clockwise = 1;
		}
		if (IsKeyPressed(KEY_F) && IsKeyDown(KEY_LEFT_SHIFT)) {
			axis = AXIS_Y;
			clockwise = 1;
		}
		if (IsKeyPressed(KEY_H) && IsKeyDown(KEY_LEFT_SHIFT)) {
			axis = AXIS_Z;
			clockwise = 1;
		}
		if (IsKeyPressed(KEY_I) && IsKeyDown(KEY_RIGHT_SHIFT)) {
			axis = AXIS_X;
			clockwise = 0;
		}
		if (IsKeyPressed(KEY_F) && IsKeyDown(KEY_RIGHT_SHIFT)) {
			axis = AXIS_Y;
			clockwise = 0;
		}
		if (IsKeyPressed(KEY_H) && IsKeyDown(KEY_RIGHT_SHIFT)) {
			axis = AXIS_Z;
			clockwise = 0;
		}

		if (clockwise != -1) {
			anim->active = 1;
			anim->angle = 0.0f;
			anim->pieceCount = 27;
			anim->dir = clockwise;
			anim->axis = axis;

			for (int i = 0; i < 27; i++) {
				anim->indices[i] = i;
				Cube *p = &cube->pieces[i];
				anim->startPos[i] = (Vector3){(float)p->x, (float)p->y, (float)p->z};
				anim->startOrient[i] = p->orient;

				// int newColors[6];
				// for (int j = 0; j < 6; j++) {
				// 	int src = rotate_face_index(j, axis, clockwise);
				// 	newColors[j] = p->colors[src];
				// }

				// memcpy(anim->startColors[i], newColors, 6*sizeof(int));
				memcpy(anim->startColors[i], p->colors, 6*sizeof(int));
			}

			rotate_cube_axis(cube, axis, clockwise);
		}

		return;
	}

	int activeTarget = -1, isClockwise = 0;

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

	Axis axis;
	int layer, dir;

	switch (activeTarget) {
		case FACE_RIGHT:
			axis = AXIS_X;
			layer = 1;
			dir = isClockwise ? 1 : 0;
			break;
		case FACE_LEFT:
			axis = AXIS_X;
			layer = -1;
			dir = isClockwise ? 0 : 1;
			break;
		case FACE_UP:
			axis = AXIS_Y;
			layer = 1;
			dir = isClockwise ? 1 : 0;
			break;
		case FACE_DOWN:
			axis = AXIS_Y;
			layer = -1;
			dir = isClockwise ? 0 : 1;
			break;
		case FACE_FRONT:
			axis = AXIS_Z;
			layer = 1;
			dir = isClockwise ? 1 : 0;
			break;
		case FACE_BACK:
			axis = AXIS_Z;
			layer = -1;
			dir = isClockwise ? 0 : 1;
			break;
	}

	anim->active = 1;
	anim->axis = axis;
	anim->dir = dir;
	anim->angle = 0.0f;
	anim->pieceCount = 0;

	for (int i = 0; i < 27; i++) {
		Cube *p = &cube->pieces[i];

		int selected = 0;

		if (axis == AXIS_X && p->x == layer) selected = 1;
		if (axis == AXIS_Y && p->y == layer) selected = 1;
		if (axis == AXIS_Z && p->z == layer) selected = 1;

		if (selected) {
			int idx = anim->pieceCount++;
			anim->indices[idx] = i;
			anim->startPos[idx] = (Vector3){(float)p->x, (float)p->y, (float)p->z};
			anim->startOrient[idx] = p->orient;
			memcpy(anim->startColors[idx], p->colors, 6*sizeof(int));
		}
	}

	rotate_face(cube, activeTarget, isClockwise);
}
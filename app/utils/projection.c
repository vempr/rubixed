#include <raylib.h>
#include <raymath.h>
#include "../app.h"
#include "../../cube/draw.h"
#include "../../cube/cube.h"

static int get_face_color(Cube *p, Face face) {
	// typedef enum {
	// 	FACE_UP,
	// 	FACE_DOWN,
	// 	FACE_LEFT,
	// 	FACE_RIGHT,
	// 	FACE_FRONT,
	// 	FACE_BACK
	// } Face;

	Vector3 faceDirs[6] = {
		{0, 1, 0}, // up
		{0, -1, 0}, // down
		{-1, 0, 0}, // left
		{1, 0, 0}, // right
		{0, 0, 1}, // front
		{0, 0, -1} // back
	};
	Vector3 targetDir = faceDirs[face];

	Quaternion invOrient = QuaternionInvert(p->orient);
	Vector3 localDir = Vector3RotateByQuaternion(targetDir, invOrient);

	// too imprecise
	// float maxDot = -1;
	// int f = 0;

	// for (int i = 0; i < 6; i++) {
	// 	float dot = Vector3DotProduct(localDir, faceDirs[i]);
	// 	if (dot > maxDot) {
	// 		maxDot = dot;
	// 		f = i;
	// 	}
	// }

	float ax = fabsf(localDir.x);
	float ay = fabsf(localDir.y);
	float az = fabsf(localDir.z);

	if (ax >= ay && ax >= az) return localDir.x > 0 ? p->colors[FACE_RIGHT] : p->colors[FACE_LEFT];
	if (ay >= ax && ay >= az) return localDir.y > 0 ? p->colors[FACE_UP] : p->colors[FACE_DOWN];
	return localDir.z > 0 ? p->colors[FACE_FRONT] : p->colors[FACE_BACK];
}

void draw_face(App *app, Face face, int x0, int y0, int cell) {
  int axis, layer;
	int u, v;

	switch(face) {
		case FACE_RIGHT: axis = 0; layer = 1; break;
		case FACE_LEFT: axis = 0; layer = -1; break;
		case FACE_UP: axis = 1; layer = 1; break;
		case FACE_DOWN: axis = 1; layer = -1; break;
		case FACE_FRONT: axis = 2; layer = 1; break;
		case FACE_BACK: axis = 2; layer = -1; break;
		default: return;
	}

	RubiksCube *c = &app->cube;
	
	for (int i = 0; i < 27; i++) {
		Cube *p = &c->pieces[i];

		int on_face = 0;
		if (axis == 0 && p->x == layer) on_face = 1;
		if (axis == 1 && p->y == layer) on_face = 1;
		if (axis == 2 && p->z == layer) on_face = 1;
		if (!on_face) continue;

		// if (axis == 0) {
		// 	u = p->z;
		// 	v = -p->y;
		// } else if (axis == 1) {
		// 	u = p->x;
		// 	v = -p->z;
		// } else {
		// 	u = p->x;
		// 	v = -p->y;
		// }

		switch(face) {
			case FACE_RIGHT: u = -p->z; v = -p->y; break;
			case FACE_LEFT: u = p->z; v = -p->y; break;
			case FACE_UP: u = p->x; v = p->z; break;
			case FACE_DOWN: u = p->x; v = -p->z; break;
			case FACE_FRONT: u = p->x; v = -p->y; break;
			case FACE_BACK: u = -p->x; v = -p->y; break;
			default: return;
		}

		int cid = get_face_color(p, face);
		Color c = GetRaylibColor(cid, app->stickerFade);

		DrawRectangle(
			x0 + (u + 1) * cell,
			y0 + (v + 1) * cell,
			cell - 3,
			cell - 3,
			c
		);
	}
}
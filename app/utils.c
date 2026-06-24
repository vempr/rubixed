#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "theme.h"
#include "app.h"
#include "../cube/draw.h"
#include "../cube/cube.h"
#include "../storage/log.h"

// move history

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

static void draw_face(App *app, Face face, int x0, int y0, int cell) {
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

void draw_cube_projection(App *app) {
	int cell = 15 * ui_scale();
	int startX = GetScreenWidth() - 250 * ui_scale();
	int startY = 150;
	int gap = 50 * ui_scale();

	draw_face(app, FACE_UP, startX + gap * 2, startY + gap * 0, cell); // up

  draw_face(app, FACE_LEFT, startX + gap * 1, startY + gap * 1, cell); // left
  draw_face(app, FACE_FRONT, startX + gap * 2, startY + gap * 1, cell); // front
  draw_face(app, FACE_RIGHT, startX + gap * 3, startY + gap * 1, cell); // right
	draw_face(app, FACE_BACK, startX + gap * 4, startY + gap * 1, cell); // back

  draw_face(app, FACE_DOWN, startX + gap * 2, startY + gap * 2, cell); // down
}

static int load_solves(SolveEntry *out, AppMode mode) {
	FILE *f = fopen(SOLVES_FILE, "r");
  if (!f) return 0;

	char line[512];
	int count = 0;

	fgets(line, sizeof(line), f);

	while (fgets(line, sizeof(line), f)) {
		char *tmp = strdup(line);
		char *token = strtok(tmp, ",");

		SolveEntry s = {0};

		s.timestamp = strdup(token);

		token = strtok(NULL, ",");
		s.scramble = strdup(token);

		token = strtok(NULL, ",");
		s.time = strtod(token, NULL);

		token = strtok(NULL, ",");
		s.dnf = atoi(token);

		token = strtok(NULL, ",\n");
		s.mode = strdup(token);

		if (
			strcmp(s.mode, "physical") == 0 && mode == MODE_PHYSICAL_SOLVE ||
			strcmp(s.mode, "virtual") == 0 && mode == MODE_VIRTUAL_SOLVE
		) {
			out[count++] = s;
		} else {
			free(s.timestamp);
			free(s.scramble);
			free(s.mode);
		}
		free(tmp);
	}

	fclose(f);
	return count;
}

static int comp(const void *a, const void *b) {
	float fa = *(const float *)a;
	float fb = *(const float *)b;

	if (fa < fb) return -1;
	if (fa > fb) return 1;
	return 0;
}

void draw_statistics_bar(App *app) {
	if (app->mode == MODE_FREE || app->mode == MODE_SELF_SOLVE) return;

	float cell = 25.0 * ui_scale();
	float rectWidth = 60.0 * ui_scale();
	int startX = 10;
	int startY = 60;
	float gap = 20.0 * ui_scale();
	float fontSize = 13.0 * ui_scale();
	int borderSize = 2;
	Color borderColor = BLACK;

	SolveEntry solves[MAX_SOLVES];
	int numOfSolves = load_solves(solves, app->mode);
	int row = 0;

	for (int i = numOfSolves - 1; i > -1; i--) {
		row++;

		int x = startX;
		int y = startY + row * cell;
		int rowOffset = 0;

		// i
		const char* ti = TextFormat("%d", i + 1);
		int indexCellWidth = cell * 1.5;

		DrawRectangle(
			x,
			y,
			indexCellWidth,
			cell,
			borderColor
		);

		DrawRectangle(
			x + borderSize,
			y + borderSize,
			indexCellWidth - borderSize * 2,
			cell - borderSize * 2,
			COLOR_ACCENT
		);

		DrawText(
			ti,
			x + indexCellWidth / 2 - MeasureText(ti, fontSize) / 2,
			y + MeasureTextEx(GetFontDefault(), ti, fontSize, 0).y / 2,
			fontSize,
			COLOR_TEXT
		);
		
		rowOffset += indexCellWidth;

		
		// time
		const char* tt = solves[i].dnf ? "DNF" : TextFormat("%.2f", solves[i].time);

		DrawRectangle(
			x + rowOffset,
			y,
			rectWidth,
			cell,
			borderColor
		);

		DrawRectangle(
			x + borderSize + rowOffset,
			y + borderSize,
			rectWidth - borderSize * 2,
			cell - borderSize * 2,
			COLOR_ACCENT
		);

		DrawText(
			tt,
			x + rectWidth / 2 - MeasureText(tt, fontSize) / 2 + rowOffset,
			y + MeasureTextEx(GetFontDefault(), tt, fontSize, 0).y / 2,
			fontSize,
			solves[i].dnf ? RED : COLOR_TEXT
		);

		rowOffset += rectWidth;


		// ao5
		int dnfCount = 0;
		float lastFiveTimes[5];

		if (i >= 4) {
			for (int j = 0; j < 5; j++) {
				SolveEntry sol = solves[i - j];

				if (sol.dnf) {
					dnfCount++;

					if (dnfCount >= 2) {
						break;
					}

					lastFiveTimes[j] = __FLT_MAX__;
				} else {
					lastFiveTimes[j] = sol.time;
				}
			}
		}

		char ao5Buffer[32];
		if (i < 4) {
			strcpy(ao5Buffer, "-");
		} else if (dnfCount >= 2) {
			strcpy(ao5Buffer, "DNF");
		} else {
			int n = sizeof(lastFiveTimes) / sizeof(lastFiveTimes)[0];
			qsort(lastFiveTimes, n, sizeof(lastFiveTimes[0]), comp);
		
			float ao5 = lastFiveTimes[1] + lastFiveTimes[2] + lastFiveTimes[3];
			ao5 /= 3.0f;

			snprintf(ao5Buffer, sizeof(ao5Buffer), "%.2f", ao5);
		}

		DrawRectangle(
			x + rowOffset,
			y,
			rectWidth,
			cell,
			borderColor
		);

		DrawRectangle(
			x + borderSize + rowOffset,
			y + borderSize,
			rectWidth - borderSize * 2,
			cell - borderSize * 2,
			COLOR_ACCENT
		);

		DrawText(
			ao5Buffer,
			x + rectWidth / 2 - MeasureText(ao5Buffer, fontSize) / 2 + rowOffset,
			y + MeasureTextEx(GetFontDefault(), ao5Buffer, fontSize, 0).y / 2,
			fontSize,
			solves[i].dnf ? RED : COLOR_TEXT
		);
	}
}
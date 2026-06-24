#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "projection.h"
#include "table.h"
#include "../theme.h"
#include "../app.h"
#include "../../cube/draw.h"
#include "../../cube/cube.h"
#include "../../storage/log.h"

// move history

static void draw_cube_projection(App *app) {
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

static int draw_statistics_bar(App *app) {
	if (app->mode == MODE_FREE || app->mode == MODE_SELF_SOLVE) return -1;

	float cell = 25.0 * ui_scale();
	int indexCellWidth = cell * 1.5;
	float rectWidth = 60.0 * ui_scale();
	int startX = 10;
	int startY = 90;
	float gap = 20.0 * ui_scale();
	float fontSize = 13.0 * ui_scale();

	SolveEntry solves[MAX_SOLVES];
	int numOfSolves = load_solves(solves, app->mode);
	int row = 0;
	int rowOffset = 0;

	int usableH = GetScreenHeight() - startY - cell - 100;
	int rowsPerPage = usableH / cell;
	int maxPage = (numOfSolves + rowsPerPage - 1) / rowsPerPage;
	int startIndex = numOfSolves - app->tablePage * rowsPerPage - 1;
	int endIndex = startIndex - rowsPerPage + 1;
	if (endIndex < 0) endIndex = 0;

	draw_cell(
		startX,
		startY,
		indexCellWidth,
		cell,
		"#",
		COLOR_BG,
		COLOR_TEXT,
		fontSize
	);
	
	rowOffset += indexCellWidth;

	draw_cell(
		startX + rowOffset,
		startY,
		rectWidth,
		cell,
		"TIME",
		COLOR_BG,
		COLOR_TEXT,
		fontSize
	);

	rowOffset += rectWidth;

	draw_cell(
		startX + rowOffset,
		startY,
		rectWidth,
		cell,
		"AO5",
		COLOR_BG,
		COLOR_TEXT,
		fontSize
	);

	rowOffset += rectWidth;

	draw_cell(
		startX + rowOffset,
		startY,
		rectWidth,
		cell,
		"AO12",
		COLOR_BG,
		COLOR_TEXT,
		fontSize
	);

	rowOffset += rectWidth;

	for (int i = startIndex; i >= endIndex; i--) {
		row++;

		int x = startX;
		int y = startY + row * cell;
		rowOffset = 0;

		// i
		const char* ti = TextFormat("%d", i + 1);

		draw_cell(
			x,
			y,
			indexCellWidth,
			cell,
			ti,
			COLOR_ACCENT,
			COLOR_TEXT,
			fontSize
		);

		rowOffset += indexCellWidth;
		
		// time
		const char* tt = solves[i].dnf ? "DNF" : TextFormat("%.2f", solves[i].time);

		draw_cell(
			x + rowOffset,
			y,
			rectWidth,
			cell,
			tt,
			COLOR_ACCENT,
			solves[i].dnf ? RED : COLOR_TEXT,
			fontSize
		);

		rowOffset += rectWidth;

		// ao5
		float ao5 = compute_average(solves, i, 5);
		char ao5Buffer[32];

		if (ao5 < -1.5f) strcpy(ao5Buffer, "-");
		else if (ao5 < -0.5f) strcpy(ao5Buffer, "DNF");
		else snprintf(ao5Buffer, sizeof(ao5Buffer), "%.2f", ao5);

		draw_cell(
			x + rowOffset,
			y,
			rectWidth,
			cell,
			ao5Buffer,
			COLOR_ACCENT,
			ao5 > -1.5f && ao5 < -0.5f ? RED : COLOR_TEXT,
			fontSize
		);

		rowOffset += rectWidth;

		// ao12
		float ao12 = compute_average(solves, i, 12);
		char ao12Buffer[32];

		if (ao12 < -1.5f) strcpy(ao12Buffer, "-");
		else if (ao12 < -0.5f) strcpy(ao12Buffer, "DNF");
		else snprintf(ao12Buffer, sizeof(ao12Buffer), "%.2f", ao12);

		draw_cell(
			x + rowOffset,
			y,
			rectWidth,
			cell,
			ao12Buffer,
			COLOR_ACCENT,
			ao12 > -1.5f && ao12 < -0.5f ? RED : COLOR_TEXT,
			fontSize
		);
	}

	return maxPage;
}

void draw_utils(App *app) {
	draw_cube_projection(app);
  int totalPages = draw_statistics_bar(app);
	handle_arrow_key_pagination(app, totalPages);
	draw_ui_pagination(&app->tablePage, totalPages);
}
#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "projection.h"
#include "table.h"
#include "stats.h"
#include "../theme.h"
#include "../app.h"
#include "../../cube/draw.h"
#include "../../cube/cube.h"
#include "../../storage/log.h"
#include "../../ui/ui.h"

// move history

static UITabDialog statsDialog = {
	.open = 0,
	.title = "Solve Statistics",
	.activeTab = 0
};

static UITab statsTabs[3] = {
	{"Time", draw_time_tab},
	{"Average of 5", draw_ao5_tab},
	{"Average of 12", draw_ao12_tab},
};
static StatsTabCtx statsCtx;

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

	if (!numOfSolves) return 0;

	int row = 0;
	int rowOffset = 0;

	int usableH = GetScreenHeight() - startY - cell - 100;
	int rowsPerPage = usableH / cell;
	int maxPage = (numOfSolves + rowsPerPage - 1) / rowsPerPage;
	int startIndex = numOfSolves - app->tablePage * rowsPerPage - 1;
	int endIndex = startIndex - rowsPerPage + 1;
	if (endIndex < 0) endIndex = 0;

	// best stats
	float validTimes[MAX_SOLVES];
	float validAo5[MAX_SOLVES];
	float validAo12[MAX_SOLVES];
	int timeCount = 0, ao5Count = 0, ao12Count = 0;

	float ao5vals[MAX_SOLVES];
	float ao12vals[MAX_SOLVES];

	for (int i = 0; i < numOfSolves; i++) {
		if (!solves[i].dnf) validTimes[timeCount++] = solves[i].time;

		float a5 = compute_average(solves, i, 5);
		ao5vals[i] = a5;
		if (a5 >= 0.0f) validAo5[ao5Count++] = a5;

		float a12 = compute_average(solves, i, 12);
		ao12vals[i] = a12;
		if (a12 >= 0.0f) validAo12[ao12Count++] = a12;
	}

	float timeThreshold = compute_top_threshold(validTimes, timeCount);
	float ao5Threshold = compute_top_threshold(validAo5, ao5Count);
	float ao12Threshold = compute_top_threshold(validAo12, ao12Count);
	
	// header
	draw_cell(
		startX,
		startY,
		indexCellWidth,
		cell,
		"#",
		COLOR_BG,
		COLOR_TEXT,
		fontSize,
		0
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
		fontSize,
		0
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
		fontSize,
		0
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
		fontSize,
		0
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
			fontSize,
			0
		);

		rowOffset += indexCellWidth;
		
		// time
		const char* tt = solves[i].dnf ? "DNF" : TextFormat("%.2f", solves[i].plus2 ? solves[i].time + 2.0 : solves[i].time);
		int effectiveTime = solves[i].plus2 ? solves[i].time + 2.0 : solves[i].time;
		int highlightTime = !solves[i].dnf && effectiveTime <= timeThreshold;

		draw_cell(
			x + rowOffset,
			y,
			rectWidth,
			cell,
			tt,
			COLOR_ACCENT,
			solves[i].dnf ? RED : solves[i].plus2 ? ORANGE : COLOR_TEXT,
			fontSize,
			highlightTime
		);

		rowOffset += rectWidth;

		// ao5
		float ao5 = compute_average(solves, i, 5);
		char ao5Buffer[32];

		if (ao5 < -1.5f) strcpy(ao5Buffer, "-");
		else if (ao5 < -0.5f) strcpy(ao5Buffer, "DNF");
		else snprintf(ao5Buffer, sizeof(ao5Buffer), "%.2f", ao5);

		int highlightAo5 = ao5vals[i] >= 0.0f && ao5vals[i] <= ao5Threshold;

		draw_cell(
			x + rowOffset,
			y,
			rectWidth,
			cell,
			ao5Buffer,
			COLOR_ACCENT,
			ao5 > -1.5f && ao5 < -0.5f ? RED : COLOR_TEXT,
			fontSize,
			highlightAo5
		);

		rowOffset += rectWidth;

		// ao12
		float ao12 = compute_average(solves, i, 12);
		char ao12Buffer[32];

		if (ao12 < -1.5f) strcpy(ao12Buffer, "-");
		else if (ao12 < -0.5f) strcpy(ao12Buffer, "DNF");
		else snprintf(ao12Buffer, sizeof(ao12Buffer), "%.2f", ao12);

		int highlightAo12 = ao12vals[i] >= 0.0f && ao12vals[i] <= ao12Threshold;

		draw_cell(
			x + rowOffset,
			y,
			rectWidth,
			cell,
			ao12Buffer,
			COLOR_ACCENT,
			ao12 > -1.5f && ao12 < -0.5f ? RED : COLOR_TEXT,
			fontSize,
			highlightAo12
		);

		rowOffset += rectWidth;

		// edit row
		if (ui_button((Rectangle){x + rowOffset + 2, y + 2, indexCellWidth - 4, cell - 4}, "e.", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false) && !app->isInDialogView) {
			statsDialog.open = 1;
			app->isInDialogView = 1;
			
			statsCtx.app = app;
			statsCtx.solves = solves;
			statsCtx.count = numOfSolves;
			statsCtx.current = i;
		}
	}

	return maxPage;
}

void draw_utils(App *app) {
	draw_cube_projection(app);
  int totalPages = draw_statistics_bar(app);
	handle_arrow_key_pagination(app, totalPages, app->isInDialogView);
	draw_ui_pagination(&app->tablePage, totalPages);
	ui_tab_dialog(&statsDialog, &app->isInDialogView, 3, statsTabs, &statsCtx);
}
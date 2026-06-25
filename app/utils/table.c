#include <raylib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "table.h"
#include "../theme.h"
#include "../app.h"
#include "../../cube/cube.h"
#include "../../storage/log.h"
#include "../../ui/ui.h"

int load_solves(SolveEntry *out, AppMode mode) {
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

int comp(const void *a, const void *b) {
	float fa = *(const float *)a;
	float fb = *(const float *)b;

	if (fa < fb) return -1;
	if (fa > fb) return 1;
	return 0;
}

void draw_cell(
	int x,
	int y,
	int w,
	int h,
	const char* text,
	Color bg,
	Color textColor,
	float fontSize,
  int highlight
) {
	int borderSize = 2;
	Color borderColor = BLACK;

	DrawRectangle(
		x,
		y,
		w,
		h,
		borderColor
	);

	DrawRectangle(
		x + borderSize,
		y + borderSize,
		w - borderSize * 2,
		h - borderSize * 2,
		bg
	);

	DrawText(
		text,
		x + w / 2 - MeasureText(text, fontSize) / 2,
		y + MeasureTextEx(GetFontDefault(), text, fontSize, 0).y / 2,
		fontSize,
		highlight ? GREEN : textColor
	);
}

float compute_average(SolveEntry *solves, int endIndex, int of) {
	if (endIndex < of - 1) return -2.0f;

	int dnfCount = 0;
	float lastNTimes[of];

	for (int j = 0; j < of; j++) {
		SolveEntry sol = solves[endIndex - j];

		if (sol.dnf) {
			dnfCount++;
			lastNTimes[j] = __FLT_MAX__;
		} else {
			lastNTimes[j] = sol.time;
		}
	}

	if (dnfCount >= 2) return -1.0f;

	int n = sizeof(lastNTimes) / sizeof(lastNTimes)[0];
	qsort(lastNTimes, n, sizeof(lastNTimes[0]), comp);
	
	float sum = 0.0f;
	for (int i = 1; i < of - 1; i++) {
		sum += lastNTimes[i];
	}

	return sum / (float)(of - 2);
}

static int clamp_int(int i, int min, int max) {
  if (i < min) i = min;
  if (i > max) i = max;
  return i;
}

void handle_arrow_key_pagination(App *app, int totalPages, int navigationDisabled) {
  if (totalPages == -1 || navigationDisabled) return;

  if (IsKeyPressed(KEY_RIGHT)) app->tablePage++;
  if (IsKeyPressed(KEY_LEFT)) app->tablePage--;
  if (IsKeyPressed(KEY_UP)) app->tablePage = 0;
  if (IsKeyPressed(KEY_DOWN)) app->tablePage = totalPages - 1;

  app->tablePage = clamp_int(app->tablePage, 0, totalPages - 1);
}

void draw_ui_pagination(int *page, int maxPage) {
  if (maxPage <= 1) return;

  Rectangle paginationArea = {
    10,
    GetScreenHeight() - 50,
    GetScreenWidth() - 50,
    ui_scale() > 1.0 ? 38 : 32
  };

  float x = paginationArea.x;
  float y = paginationArea.y;
  float h = paginationArea.height;

  float btnW = h;
  float btnH = h;

  if (ui_button((Rectangle){x, y, btnW, btnH}, "<<", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false)) {
    *page = 0;
  }
  x += btnW + 5;

  if (ui_button((Rectangle){x, y, btnW, btnH}, "<", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false)) {
    *page = clamp_int(*page - 1, 0, maxPage - 1);
  }
  x += btnW + 5;

  int firstPage = 0;
  int lastPage = maxPage - 1;
  int start = *page - 2;
  int end = *page + 2;

  if (start < 0) {
    end += -start;
    start = 0;
  }
  if (end > maxPage - 1) {
    start -= end - (maxPage - 1);
    end = maxPage - 1;
  }
  if (start < 0) {
    start = 0;
  }

  char buf[16];
  snprintf(buf, sizeof(buf), "%d", firstPage + 1);

  if (ui_button((Rectangle){x, y, btnW, btnH}, buf, COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, *page == firstPage)) {
    *page = firstPage;
  }
  x += btnW + 5;

  if (start > 0) {
    ui_button((Rectangle){x, y, btnW, btnH}, "...", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false);
    x += btnW + 5;
  }

  for (int i = start + 1; i <= end - 1; i++) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", i + 1);

    if (ui_button((Rectangle){x, y, btnW, btnH}, buf, COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, i == *page)) {
      *page = i;
    }

    x += btnW + 5;
  }

  if (end < maxPage - 1) {
    ui_button((Rectangle){x, y, btnW, btnH}, "...", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false);
    x += btnW + 5;
  }

  if (lastPage > 0) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", lastPage + 1);

    if (ui_button((Rectangle){x, y, btnW, btnH}, buf, COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, *page == lastPage)) {
      *page = lastPage;
    }
    x += btnW + 5;
  }

  if (start > 0) {
    ui_button((Rectangle){x, y, btnW, btnH}, "...", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false);
    x += btnW + 5;
  }

  if (ui_button((Rectangle){x, y, btnW, btnH}, ">", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false)) {
    *page = clamp_int(*page + 1, 0, maxPage - 1);
  }
  x += btnW + 5;

  if (ui_button((Rectangle){x, y, btnW, btnH}, ">>", COLOR_BG, COLOR_TEXT, COLOR_ACCENT, COLOR_ACTIVE, false)) {
    *page = maxPage - 1;
  }
  x += btnW + 5;
}

float compute_top_threshold(const float *values, int count) {
  if (count == 0) return __FLT_MAX__;

  int top_n = (int)(count * 0.01);
  if (top_n < 1) top_n = 1;

  float sorted[MAX_SOLVES];
  memcpy(sorted, values, count * sizeof(float));
  qsort(sorted, count, sizeof(float), comp);

  int idx = (top_n - 1 < count - 1) ? top_n - 1 : count - 1;
  return sorted[idx];
}
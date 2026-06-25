#include "../app.h"
#include "../../storage/log.h"

#ifndef TABLE_H
#define TABLE_H

int comp(const void *a, const void *b);
int load_solves(SolveEntry *out, AppMode mode);
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
);
float compute_average(SolveEntry *solves, int endIndex, int of);
void handle_arrow_key_pagination(App *app, int totalPages, int navigationDisabled);
void draw_ui_pagination(int *page, int maxPage);
float compute_top_threshold(const float *values, int count);

#endif
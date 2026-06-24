#include "../app.h"
#include "../../storage/log.h"

#ifndef TABLE_H
#define TABLE_H

int load_solves(SolveEntry *out, AppMode mode);
void draw_cell(
	int x,
	int y,
	int w,
	int h,  
	const char* text,
	Color bg,
	Color textColor,
	float fontSize
);
float compute_average(SolveEntry *solves, int endIndex, int of);
void handle_arrow_key_pagination(App *app, int totalPages);
void draw_ui_pagination(int *page, int maxPage);

#endif
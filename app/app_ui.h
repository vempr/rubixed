#include "app.h"
#include "../ui/ui.h"

#ifndef APP_UI_H
#define APP_UI_H

void draw_elapsed_time(App *app);
void draw_buttons(App *app, UIRow *row);
void draw_solve_steps(App *app);

#endif
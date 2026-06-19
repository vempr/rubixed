#include "app.h"
#include "../ui/ui.h"

#ifndef APP_UI
#define APP_UI

void draw_elapsed_time(App *app);
void draw_buttons(App *app, UIRow *row);
void draw_solve_steps(App *app);

#endif
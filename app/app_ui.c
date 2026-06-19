#include "app.h"
#include "raylib.h"
#include "theme.h"
#include "../ui/ui.h"
#include "app_modes.h"
#include "../timer/timer.h"

void draw_elapsed_time(App *app) {
  if (app->mode == MODE_VIRTUAL_SOLVE && app->timer.running) {
    double elapsed = get_time_elapsed(app->timer.startSolveTime);
    DrawText(
      TextFormat("%.2f", elapsed),
      (GetScreenWidth() - MeasureText(TextFormat("%.2f", elapsed), (int)(35 * ui_scale()))) / 2,
      GetScreenHeight() - 100,
      (int)(35 * ui_scale()),
      COLOR_TEXT
    );
  }
}

void draw_buttons(App *app, UIRow *row) {
  if (ui_button(
    ui_next(row),
    "(1) Freestyle",
    COLOR_BG,
    COLOR_TEXT,
    COLOR_ACCENT,
    COLOR_ACTIVE,
    app->mode == MODE_FREE
  )) {
    set_mode(app, MODE_FREE);
  }
  if (ui_button(
    ui_next(row),
    "(2) Instant Solve",
    COLOR_BG,
    COLOR_TEXT,
    COLOR_ACCENT,
    COLOR_ACTIVE,
    app->mode == MODE_SELF_SOLVE
  )) {
    set_mode(app, MODE_SELF_SOLVE);
  }
  if (ui_button(
    ui_next(row),
    "(3) Physical Solve",
    COLOR_BG,
    COLOR_TEXT,
    COLOR_ACCENT,
    COLOR_ACTIVE,
    app->mode == MODE_PHYSICAL_SOLVE
  )) {
    set_mode(app, MODE_PHYSICAL_SOLVE);
  }
  if (ui_button(
    ui_next(row),
    "(4) Virtual Solve",
    COLOR_BG,
    COLOR_TEXT,
    COLOR_ACCENT,
    COLOR_ACTIVE,
    app->mode == MODE_VIRTUAL_SOLVE
  )) {
    set_mode(app, MODE_VIRTUAL_SOLVE);
  }
}
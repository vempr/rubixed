#include "app.h"
#include "raylib.h"
#include "theme.h"
#include "../ui/ui.h"
#include "app_modes.h"
#include "../timer/timer.h"

static void draw_centered_text(const char* text, int fontSize, Color fontColor, int y_offset) {
  DrawText(
    text,
    (GetScreenWidth() - MeasureText(text, (int)(fontSize * ui_scale()))) / 2,
    GetScreenHeight() - y_offset,
    (int)(fontSize * ui_scale()),
    fontColor
  );
}

void draw_elapsed_time(App *app) {
  if (
    (app->mode == MODE_VIRTUAL_SOLVE || app->mode == MODE_PHYSICAL_SOLVE) &&
    app->timer.running
  ) {
    double elapsed = get_time_elapsed(app->timer.startSolveTime);
    draw_centered_text(TextFormat("%.2f", elapsed), 35, COLOR_TEXT, 100);
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

void draw_solve_steps(App *app) {
  if (
    app->mode != MODE_PHYSICAL_SOLVE &&
    app->mode != MODE_VIRTUAL_SOLVE
  ) return;

  switch(get_solve_state(app)) {
    case STATE_NO_SCRAMBLE: draw_centered_text("ENTER to generate scramble", 15, COLOR_TEXT, 100); break;
    case STATE_IDLE:        draw_centered_text("Press SPACE to inspect", 20, COLOR_TEXT, 100); break;
    case STATE_INSPECT: {
      double remaining = 15.0 - get_time_elapsed(app->timer.startInspectionTime);
      draw_centered_text("INSPECTION", 30, BLUE, 100);
      draw_centered_text(TextFormat("%.1f", remaining), 30, BLUE, 175);
      break;
    }
    case STATE_HOLD: {
      double remaining = 15.0 - get_time_elapsed(app->timer.startInspectionTime);
      draw_centered_text("Hold SPACE...", 30, RED, 100);
      draw_centered_text(TextFormat("%.1f", remaining), 30, BLUE, 175);
      break;
    }
    case STATE_ARMED: {
      double remaining = 15.0 - get_time_elapsed(app->timer.startInspectionTime);
      draw_centered_text("RELEASE to START", 25, GREEN, 100);
      draw_centered_text(TextFormat("%.1f", remaining), 30, BLUE, 175);
      break;
    }
    case STATE_RUNNING:     draw_elapsed_time(app); break;
    case STATE_DNF: {
      double remaining = 15.0 - get_time_elapsed(app->timer.startInspectionTime);
      if (remaining < 0) {
        draw_centered_text("DNF: Exceeded inspection limit", 30, RED, 100);
      } else {
        draw_centered_text("DNF: Solve attempt aborted", 30, RED, 100);
      }

      draw_centered_text("ENTER to generate scramble", 20, COLOR_TEXT, 150);
      break;
    }
  }
}

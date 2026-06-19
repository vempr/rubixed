#include <stdio.h>
#include "app.h"
#include "../timer/timer.h"

static void exit_mode(App *app) {
  if (app->mode == MODE_VIRTUAL_SOLVE && app->timer.running) {
    double elapsed = get_time_elapsed(app->timer.startSolveTime);
    printf(
      "DNF: scramble=%s time=%.3f\n",
      app->currentScramble,
      elapsed
    );
  }
}

void set_mode(App *app, AppMode mode) {
  if (app->mode == mode) return;

  exit_mode(app);
  reset_session(app);
  app->mode = mode;
}


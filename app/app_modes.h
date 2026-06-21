#include "app.h"

#ifndef APP_MODES_H
#define APP_MODES_H

typedef enum {
  STATE_NO_SCRAMBLE,
  STATE_IDLE,
  STATE_INSPECT,
  STATE_HOLD,
  STATE_ARMED,
  STATE_RUNNING,
  STATE_DNF,
} SolveState;

void set_mode(App *app, AppMode mode);
void handle_solve_space(App *app);
SolveState get_solve_state(App *app);
void trigger_self_solve(App *app);

#endif
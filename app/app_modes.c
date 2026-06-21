#include <stdio.h>
#include <stdlib.h>
#include "app.h"
#include "../timer/timer.h"
#include "../cube/anim.h"
#include "app_modes.h"
#include "facecube.h"
#include "../ckociemba/include/search.h"
#include "../storage/log.h"

SolveState get_solve_state(App *app) {
  SolveTimer *t = &app->timer;

  if (!app->currentScramble) return STATE_NO_SCRAMBLE;
  if (t->dnf) return STATE_DNF;
  if (t->running) return STATE_RUNNING;
  if (t->armed) return STATE_ARMED;
  if (t->spaceHeld) return STATE_HOLD;
  if (t->inspectionActive) return STATE_INSPECT;
  return STATE_IDLE;
}

static void exit_mode(App *app) {
  if ((app->mode == MODE_VIRTUAL_SOLVE || app->mode == MODE_PHYSICAL_SOLVE) && app->timer.running) {
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

void handle_solve_space(App *app) {
  if (app->mode == MODE_SELF_SOLVE) {
    if (IsKeyPressed(KEY_SPACE) || IsKeyDown(KEY_SPACE)) {
      trigger_self_solve(app);
    }
    return;
  }

  if (
    (app->mode != MODE_PHYSICAL_SOLVE &&
    app->mode != MODE_VIRTUAL_SOLVE) ||
    !app->currentScramble
  ) return;

  SolveTimer *t = &app->timer;
  if (app->timer.dnf) return;

  if (t->inspectionActive && !t->dnf) {
    if (get_time_elapsed(t->startInspectionTime) > 15.0) {
      printf("DNF (inspection time exceeded)\n");

      log_solve(app->currentScramble, 0.0, 1, (app->mode == MODE_PHYSICAL_SOLVE) ? "physical" : "virtual");
      
      t->dnf = 1;
      t->running = 0;
      t->inspectionActive = 0;
      t->armed = 0;
      t->spaceHeld = 0;
    }
  }

  if (IsKeyPressed(KEY_SPACE)) {
    if (!t->inspectionActive && !t->running) {
      t->dnf = 0;
      t->inspectionActive = 1;
      t->startInspectionTime = GetTime();
      printf("Inspection started\n");
    }

    if (t->inspectionActive && !t->running && !t->dnf) {
      t->spaceHeld = 1;
      t->startHoldTime = GetTime();
    }

    if (
      (app->mode == MODE_PHYSICAL_SOLVE && t->running && !t->dnf) ||
      (app->mode == MODE_VIRTUAL_SOLVE && t->running && !t->dnf && virtual_cube_is_solved(&app->cube))) {
      double elapsed = get_time_elapsed(t->startSolveTime);

      printf(
        "SOLVED: scramble=%s time=%.3f\n",
        app->currentScramble,
        elapsed
      );
      log_solve(app->currentScramble, elapsed, 0, (app->mode == MODE_PHYSICAL_SOLVE) ? "physical" : "virtual");

      free(app->currentScramble);
      app->currentScramble = NULL;
      
      t->running = 0;
      t->inspectionActive = 0;
      t->armed = 0;
      t->spaceHeld = 0;
    }
  }

  if (IsKeyDown(KEY_SPACE)) {
    if (
      t->inspectionActive &&
      t->spaceHeld &&
      !t->armed &&
      !t->running &&
      !t->dnf
    ) {
      if (GetTime() - t->startHoldTime >= 1.0) {
        t->armed = 1;
        printf("Armed, release to start solve\n");
      }
    }
  }

  if (IsKeyReleased(KEY_SPACE)) {
    if (t->armed && !t->dnf) {
      t->running = 1;
      t->startSolveTime = GetTime();
      t->inspectionActive = 0;
      t->armed = 0;
      t->spaceHeld = 0;

      printf("Solve started\n");
    }

    t->spaceHeld = 0;
  }
}

void trigger_self_solve(App *app) {
  if (
    app->mode != MODE_SELF_SOLVE ||
    app->scrAnim.active ||
    app->selfSolveRequested
  ) return;

  app->selfSolveRequested = 1;

  facecube_t *fc = toFaceCube(app->solverCube);
  char state[55];
  to_String(fc, state);

  char *sol = NULL;
  char *inverted = NULL;

  if (app->currentScramble) {
    for (int max_moves = 22; max_moves <=30; max_moves++) {
      sol = solution(
        state,
        max_moves,
        10000,
        0,
        "cache"
      );

      if (!sol) continue;

      inverted = invert_scramble(sol);
      if (strcmp(inverted, app->currentScramble) != 0) break;
    
      free(sol);
      sol = NULL;
      free(inverted);
      inverted = NULL;
    }
  }

  if (inverted) free(inverted);
  free(fc);

  if (!sol) {
    fc = toFaceCube(app->solverCube);
    to_String(fc, state);
    sol = solution(
      state,
      21,
      10000,
      0,
      "cache"
    );
    free(fc);

    if (!sol) {
      app->selfSolveRequested = 0;
      return;
    }
  }

  printf("%s\n", sol);

  parse_scramble(sol, &app->scrAnim);
  free(sol);

  app->selfSolveRequested = 0;
}
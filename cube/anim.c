#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../cube/cube.h"
#include "anim.h"
#include "../app/app.h"
#include "../app/app_modes.h"
#include "../timer/timer.h"
#include "../solver/solver.h"

// parse_scramble is in anim.c because instructions don't require scramble logic
// like other functions in scramble.c; only parses scramble for scrAnim to use
void parse_scramble(const char* scramble, ScrambleAnim* scrAnim) {
  scrAnim->moveCount = 0;
  scrAnim->current = 0;
  scrAnim->active = 1;
  
  char* sc = strdup(scramble);
  if (!sc) return;
  char* token = strtok(sc, " ");

  while (token != NULL) {
    if (token[1] == '2') {
      char temp[3] = {token[0], '\0', '\0'};

      strncpy(scrAnim->moves[scrAnim->moveCount], temp, 7);
      scrAnim->moves[scrAnim->moveCount][7] = '\0';
      scrAnim->moveCount++;

      strncpy(scrAnim->moves[scrAnim->moveCount], temp, 7);
      scrAnim->moves[scrAnim->moveCount][7] = '\0';
      scrAnim->moveCount++;
    } else {
      strncpy(scrAnim->moves[scrAnim->moveCount], token, 7);
      scrAnim->moves[scrAnim->moveCount][7] = '\0';
      scrAnim->moveCount++;
    }
   
    token = strtok(NULL, " ");
  }

  free(sc);
}

void update_animation(App *app) {
  if (!app->anim.active) return;

  app->anim.angle += 720.0f * GetFrameTime();
  if (app->anim.angle < 90.0f) return;

  app->anim.angle = 90.0f;

  if (!app->pendingMove.active) {
    app->anim.active = 0;
    return;
  }

  if (app->pendingMove.kind == MOVE_FACE) {
    rotate_face(&app->cube, app->pendingMove.face, app->pendingMove.clockwise);
    solver_apply_move(app->solverCube, app->pendingMove.face, app->pendingMove.clockwise);
  } else {
    rotate_cube_axis(&app->cube, app->pendingMove.axis, app->pendingMove.clockwise);
  }

  app->pendingMove.active = 0;
  app->anim.active = 0;

  if (
    app->mode == MODE_VIRTUAL_SOLVE &&
    app->timer.running &&
    app->currentScramble
  ) {
    if (virtual_cube_is_solved(&app->cube)) {
      double elapsed = get_time_elapsed(app->timer.startSolveTime);

      printf(
        "SOLVED: scramble=%s time=%.3f\n",
        app->currentScramble,
        elapsed
      );

      app->timer = (SolveTimer){0};
    }
  }
}

void update_sticker_color(App *app) {
  if (app->mode == MODE_FREE || app->mode == MODE_SELF_SOLVE) return;

  if (app->scrAnim.active) {
    float progress = (float)app->scrAnim.current / (float)app->scrAnim.moveCount;
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    // smoothstep
    app->stickerFade = progress * progress * (3.0f - 2.0f * progress);
  } else {
    SolveState state = get_solve_state(app);

    if (state == STATE_NO_SCRAMBLE) {
      app->stickerFade = 0.0f;
      return;
    }

    if (state == STATE_IDLE) {
      app->stickerFade = 1.0f;
      return;
    }

    if (state == STATE_INSPECT || state == STATE_RUNNING || state == STATE_HOLD) {
      app->stickerFade = 0.0f;
      return;
    }
  }
}
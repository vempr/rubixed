#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "app.h"
#include "theme.h"
#include "app_modes.h"
#include "app_ui.h"
#include "./utils/utils.h"
#include "../app/app.h"
#include "../cube/cube.h"
#include "../cube/anim.h"
#include "../cube/draw.h"
#include "../scramble/scramble.h"
#include "../scramble/scramble_engine.h"
#include "../camera/camera.h"
#include "../ui/ui.h"
#include "../timer/timer.h"
#include "../storage/log.h"

static const KeyMap face_map[] = {
  {KEY_I, FACE_RIGHT, 1},
  {KEY_K, FACE_RIGHT, 0},
  {KEY_E, FACE_LEFT, 0},
  {KEY_D, FACE_LEFT, 1},
  {KEY_F, FACE_UP, 0},
  {KEY_J, FACE_UP, 1},
  {KEY_S, FACE_DOWN, 0},
  {KEY_W, FACE_DOWN, 1},
  {KEY_H, FACE_FRONT, 1},
  {KEY_G, FACE_FRONT, 0},
  {KEY_R, FACE_BACK, 0},
  {KEY_Y, FACE_BACK, 1}
};

void init_app_window(int width, int height, char* title) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
	InitWindow(width, height, title);
	SetWindowMinSize(width, height);
}

void init_app_cube(App *app) {
  app->mode = MODE_FREE;
  app->cube = (RubiksCube){0};
	init_cube(&app->cube);
	app->solverCube = get_cubiecube();
  app->selfSolveRequested = 0;

	scramble_init("cache");
	app->currentScramble = NULL;

	app->anim = (CubeAnim){0};
	app->anim.active = 0;
	app->scrAnim = (ScrambleAnim){0};
	app->scrAnim.active = 0;
	app->pendingMove = (PendingMove){0};
	app->intent = (MoveIntent){0};
	app->timer = (SolveTimer){0};
  app->stickerFade = 0.0f;
  app->tablePage = 0;
}

void reset_session(App *app) {
  free(app->currentScramble);
  init_cube(&app->cube);

  // debug
  // for (int i = 0; i < 27; i++) {
  //   Cube *p = &app->cube.pieces[i];
  //   printf("(%d,%d,%d):", p->x, p->y, p->z);
  //   for (int j = 0; j < 6; j++) printf("%d ", p->colors[j]);
  //   printf("\n");
  // }

  app->currentScramble = NULL;
  app->anim = (CubeAnim){0};
	app->anim.active = 0;
	app->scrAnim = (ScrambleAnim){0};
	app->scrAnim.active = 0;
	app->pendingMove = (PendingMove){0};
	app->intent = (MoveIntent){0};
	app->timer = (SolveTimer){0};
  app->stickerFade = 0.0f;
  app->tablePage = 0;
}

static void solve_abort(App *app) {
  if (app->mode != MODE_VIRTUAL_SOLVE || !app->timer.running) return;

  double elapsed = 0.0;
  if (app->timer.running) {
    elapsed = get_time_elapsed(app->timer.startSolveTime);
  }

  printf(
    "DNF: scramble=%s time=%.3f\n",
    app->currentScramble,
    elapsed
  );
  
  // behaviour: dnf should not be logged if solve hasn't started
  if (elapsed > 0.0) log_solve(app->currentScramble, elapsed, 1, (app->mode == MODE_PHYSICAL_SOLVE) ? "physical" : "virtual");

  app->timer = (SolveTimer){0};
}

void handle_app_kb_shortcuts(App *app) {
  // new scramble
  if (IsKeyPressed(KEY_ENTER)) {
    solve_abort(app);

    free(app->currentScramble);
    init_cube(&app->cube);
    app->anim = (CubeAnim){ 0 };
    app->timer = (SolveTimer){ 0 };

    char* solutionStr = generate_scramble(21);
    app->currentScramble = invert_scramble(solutionStr);

    // printf("solution: [%s]\n", solutionStr);
    // printf("inverse: [%s]\n", app->currentScramble);
    
    parse_scramble(app->currentScramble, &app->scrAnim);

    free(solutionStr);
  } 
  
  // unscramble cube
  if (IsKeyPressed(KEY_BACKSPACE)) {
    solve_abort(app);

    free(app->currentScramble);
    init_cube(&app->cube);
    app->anim = (CubeAnim){ 0 };
    app->intent = (MoveIntent){0};
    app->pendingMove = (PendingMove){0};

    app->currentScramble = NULL;
    app->scrAnim = (ScrambleAnim){0};
  }

  if (IsKeyPressed(KEY_Q)) {
    SolveTimer *t = &app->timer;
    if (!app->currentScramble) return;

    double elapsed = 0.0;

    if (t->running) {
      elapsed = get_time_elapsed(t->startSolveTime);
      printf(
        "DNF: scramble=%s time=%.3f\n",
        app->currentScramble,
        elapsed
      );
    } else {
      printf("DNF: scramble=%s\n", app->currentScramble);
    }

    if (elapsed > 0.0) log_solve(app->currentScramble, elapsed, 1, (app->mode == MODE_PHYSICAL_SOLVE) ? "physical" : "virtual");
   
    t->dnf = 1;
    t->running = 0;
    t->inspectionActive = 0;
    t->armed = 0;
    t->spaceHeld = 0;
  }

  if (IsKeyPressed(KEY_ONE)) set_mode(app, MODE_FREE);
  if (IsKeyPressed(KEY_TWO)) set_mode(app, MODE_SELF_SOLVE);
  if (IsKeyPressed(KEY_THREE)) set_mode(app, MODE_PHYSICAL_SOLVE);
  if (IsKeyPressed(KEY_FOUR)) set_mode(app, MODE_VIRTUAL_SOLVE);
}

void app_draw(App *app, OrbitCamera *c) {
  int currentWidth = GetScreenWidth();
  int currentHeight = GetScreenHeight();

  if (currentHeight <= 0) currentHeight = 1;

  float s = ui_scale();
  char* name;
  UIRow row = ui_row((currentWidth - 840), 80, 200, 50, 10);

  BeginDrawing();

  ClearBackground(COLOR_BG);
  draw_utils(app);

  BeginMode3D(c->camera);
  draw_cube(&app->cube, &app->anim, app->stickerFade);
  EndMode3D();

  DrawFPS(7, 35);

  switch(app->mode) {
    case MODE_FREE: name = "Freestyle"; break;
    case MODE_SELF_SOLVE: name = "Instant Solve"; break;
    case MODE_PHYSICAL_SOLVE: name = "Physical Solve"; break;
    case MODE_VIRTUAL_SOLVE: name = "Virtual Solve"; break;
  }

  DrawText(
    TextFormat("rubixed: %s", name),
    10,
    10,
    (int)(12 * s),
    COLOR_TEXT
  );
  DrawText(
    app->currentScramble ? app->currentScramble : "",
    (currentWidth - MeasureText(app->currentScramble, (int)(25 * s))) / 2,
    30,
    (int)(25 * s),
    COLOR_TEXT
  );

  draw_solve_steps(app);
  draw_buttons(app, &row);

  EndDrawing();
}

void handle_cube_inputs(App *app) {
  if (app->mode == MODE_PHYSICAL_SOLVE) return;
	if (app->anim.active || app->intent.active || app->scrAnim.active) return;
  if (app->mode == MODE_VIRTUAL_SOLVE && !app->currentScramble) return;
  if (app->mode == MODE_VIRTUAL_SOLVE && get_solve_state(app) != STATE_INSPECT && get_solve_state(app) != STATE_RUNNING) return;

	if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) {
		Axis axis;
		int clockwise = IsKeyDown(KEY_LEFT_SHIFT) ? 1 : 0;
    int haveAxis = 0;

		if (IsKeyPressed(KEY_I)) {
			axis = AXIS_X;
			haveAxis = 1;
		}
		if (IsKeyPressed(KEY_F)) {
			axis = AXIS_Y;
			haveAxis = 1;
		}
		if (IsKeyPressed(KEY_H)) {
			axis = AXIS_Z;
			haveAxis = 1;
		}

    if (haveAxis) {
      app->intent = (MoveIntent){
        .active = 1,
        .axis = axis,
        .clockwise = clockwise,
        .kind = MOVE_WHOLE_CUBE
      };
    }

		return;
	}

  if (app->mode == MODE_VIRTUAL_SOLVE && get_solve_state(app) != STATE_RUNNING) return;

  for (int i = 0; i < (int)(sizeof(face_map) / sizeof(face_map[0])); i++) {
    if (IsKeyPressed(face_map[i].key)) {
      int isClockwise = face_map[i].clockwise;
      if (
        face_map[i].face == FACE_LEFT ||
        face_map[i].face == FACE_DOWN ||
        face_map[i].face == FACE_BACK
      ) {
        isClockwise = isClockwise ? 0 : 1;
      }

      app->intent = (MoveIntent){
        .active = 1,
        .kind = MOVE_FACE,
        .clockwise = isClockwise,
        .face = face_map[i].face
      };
    }
  }
}

void start_move_from_intent(App *app) {
  if (!app->intent.active || app->anim.active) return;
  if (app->timer.dnf) return;
  
  if (
    app->mode == MODE_VIRTUAL_SOLVE
    && app->intent.kind == MOVE_FACE
    && !app->timer.running
    && !app->scrAnim.active
    && app->currentScramble
    && app->timer.inspectionActive
    && !app->timer.dnf
  ) {
    app->timer.running = 1;
    app->timer.startSolveTime = GetTime();

    app->timer.inspectionActive = 0;
    app->timer.armed = 0;
    app->timer.spaceHeld = 0;

    printf("Solve started\n");
  }

  CubeAnim *anim = &app->anim;
  RubiksCube *cube = &app->cube;

  anim->active = 1;
  anim->angle = 0.0f;
  anim->pieceCount = 0;
  if (app->intent.kind == MOVE_FACE) {
    Face face = app->intent.face;
    anim->dir = face == FACE_LEFT || face == FACE_DOWN || face == FACE_BACK
      ? !app->intent.clockwise
      : app->intent.clockwise;
  
  } else {
    anim->dir = app->intent.clockwise;
  }

  app->pendingMove = (PendingMove){
    .active = 1,
    .axis = app->intent.axis,
    .clockwise = app->intent.clockwise,
    .face = app->intent.face,
    .kind = app->intent.kind
  };

  if (app->intent.kind == MOVE_FACE) {
    Face face = app->intent.face;

    anim->axis =
      face == FACE_RIGHT || face == FACE_LEFT ? AXIS_X :
      face == FACE_UP || face == FACE_DOWN ? AXIS_Y :
      AXIS_Z;
  
    int layer = face == FACE_RIGHT || face == FACE_UP || face == FACE_FRONT ? 1 : -1;

    for (int i = 0; i < 27; i++) {
      Cube *p = &cube->pieces[i];

      int selected = 0;

      if (anim->axis == AXIS_X && p->x == layer) selected = 1;
      if (anim->axis == AXIS_Y && p->y == layer) selected = 1;
      if (anim->axis == AXIS_Z && p->z == layer) selected = 1;

      if (selected) {
        int idx = anim->pieceCount++;
        anim->indices[idx] = i;
        anim->startPos[idx] = (Vector3){(float)p->x, (float)p->y, (float)p->z};
        anim->startOrient[idx] = p->orient;
        memcpy(anim->startColors[idx], p->colors, 6*sizeof(int));
      }
    }
  } else {
    anim->axis = app->intent.axis;

    for (int i = 0; i < 27; i++) {
      Cube *p = &cube->pieces[i];
      int idx = anim->pieceCount++;

      anim->indices[idx] = i;
      anim->startPos[idx] = (Vector3){(float)p->x, (float)p->y, (float)p->z};
      anim->startOrient[idx] = p->orient;
      memcpy(anim->startColors[idx], p->colors, 6*sizeof(int));
    }
  }

  app->intent.active = 0;
}

void app_update(App *app) {
  handle_solve_space(app);
  update_animation(app);
  update_sticker_color(app);

  if (!app->anim.active && !app->intent.active) {
    if (app->scrAnim.active) {
      scramble_engine_update(app);
    } else {
      handle_cube_inputs(app);
    }
  }
  
  start_move_from_intent(app);

  if (app->mode == MODE_SELF_SOLVE && !app->scrAnim.active) {
    // debug
    // for (int i = 0; i < 27; i++) {
    //   Cube *p = &app->cube.pieces[i];
    //   printf("(%d,%d,%d):", p->x, p->y, p->z);
    //   for (int j = 0; j < 6; j++) printf("%d ", p->colors[j]);
    //   printf("\n");
    // }
    
    static int last_solved = 0;
    int solved = virtual_cube_is_solved(&app->cube);
    if (solved && !last_solved) printf( "SOLVED virtual cube\n");
    last_solved = solved;
  }
}
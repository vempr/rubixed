#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "app.h"
#include "theme.h"
#include "../app/app.h"
#include "../cube/cube.h"
#include "../cube/anim.h"
#include "../cube/draw.h"
#include "../scramble/scramble.h"
#include "../scramble/scramble_engine.h"
#include "../camera/camera.h"
#include "../ui/ui.h"

void init_app_window(int width, int height, char* title) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
	InitWindow(width, height, title);
	SetWindowMinSize(width, height);
}

void init_app_cube(App *app) {
  app->mode = MODE_FREE;
  app->cube = (RubiksCube){0};
	init_cube(&app->cube);

	scramble_init("cache");
	app->currentScramble = NULL;

	app->anim = (CubeAnim){0};
	app->anim.active = 0;
	app->scrAnim = (ScrambleAnim){0};
	app->scrAnim.active = 0;
	app->pendingMove = (PendingMove){0};
	app->intent = (MoveIntent){0};
}

void handle_app_kb_shortcuts(App *app) {
  // new scramble
  if (IsKeyPressed(KEY_ENTER)) {
    free(app->currentScramble);
    init_cube(&app->cube);
    app->anim = (CubeAnim){ 0 };

    char* solutionStr = generate_scramble(21);
    app->currentScramble = invert_scramble(solutionStr);

    printf("solution: [%s]\n", solutionStr);
    printf("inverse: [%s]\n", app->currentScramble);
    
    parse_scramble(app->currentScramble, &app->scrAnim);

    free(solutionStr);
  } 
  
  // unscramble cube
  if (IsKeyPressed(KEY_BACKSPACE)) {
    free(app->currentScramble);
    init_cube(&app->cube);
    app->anim = (CubeAnim){ 0 };
    app->intent = (MoveIntent){0};
    app->pendingMove = (PendingMove){0};

    app->currentScramble = NULL;
    app->scrAnim = (ScrambleAnim){0};
  }

  if (IsKeyPressed(KEY_ONE)) {
    app->mode = MODE_FREE;
  }

  if (IsKeyPressed(KEY_TWO)) {
    app->mode = MODE_SELF_SOLVE;
  }

  if (IsKeyPressed(KEY_THREE)) {
    app->mode = MODE_PHYSICAL_SOLVE;
  }

  if (IsKeyPressed(KEY_FOUR)) {
    app->mode = MODE_VIRTUAL_SOLVE;
  }
}

void app_draw(App *app, OrbitCamera *c) {
  int currentWidth = GetScreenWidth();
  int currentHeight = GetScreenHeight();
  if (currentHeight <= 0) currentHeight = 1;

  BeginDrawing();

  ClearBackground(COLOR_BG);

  BeginMode3D(c->camera);
  draw_cube(&app->cube, &app->anim);
  EndMode3D();

  float s = ui_scale();

  char* name;
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
  DrawFPS(7, 35);

  UIRow row = ui_row((currentWidth - 840), 80, 200, 50, 10);

  if (ui_button(
    ui_next(&row),
    "(1) Freestyle",
    COLOR_BG,
    COLOR_TEXT,
    COLOR_ACCENT,
    COLOR_ACTIVE,
    app->mode == MODE_FREE
  )) {
    app->mode = MODE_FREE;
  }

  if (ui_button(
    ui_next(&row),
    "(2) Instant Solve",
    COLOR_BG,
    COLOR_TEXT,
    COLOR_ACCENT,
    COLOR_ACTIVE,
    app->mode == MODE_SELF_SOLVE
  )) {
    app->mode = MODE_SELF_SOLVE;
  }

  if (ui_button(
    ui_next(&row),
    "(3) Physical Solve",
    COLOR_BG,
    COLOR_TEXT,
    COLOR_ACCENT,
    COLOR_ACTIVE,
    app->mode == MODE_PHYSICAL_SOLVE
  )) {
    app->mode = MODE_PHYSICAL_SOLVE;
  }

  if (ui_button(
    ui_next(&row),
    "(4) Virtual Solve",
    COLOR_BG,
    COLOR_TEXT,
    COLOR_ACCENT,
    COLOR_ACTIVE,
    app->mode == MODE_VIRTUAL_SOLVE
  )) {
    app->mode = MODE_VIRTUAL_SOLVE;
  }

  EndDrawing();
}

void handle_cube_inputs(App *app) {
  if (app->mode == MODE_PHYSICAL_SOLVE || app->mode == MODE_SELF_SOLVE) return;
	if (app->anim.active || app->intent.active || app->scrAnim.active) return;

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

	int activeTarget = -1, isClockwise = 0;

	if (IsKeyPressed(KEY_I)) { activeTarget = FACE_RIGHT; isClockwise = 1; }
	else if (IsKeyPressed(KEY_K)) { activeTarget = FACE_RIGHT; isClockwise = 0; }
	else if (IsKeyPressed(KEY_E)) { activeTarget = FACE_LEFT; isClockwise = 1; }
	else if (IsKeyPressed(KEY_D)) { activeTarget = FACE_LEFT; isClockwise = 0; }
	else if (IsKeyPressed(KEY_F)) { activeTarget = FACE_UP; isClockwise = 1; }
	else if (IsKeyPressed(KEY_J)) { activeTarget = FACE_UP; isClockwise = 0; }
	else if (IsKeyPressed(KEY_S)) { activeTarget = FACE_DOWN; isClockwise = 1; }
	else if (IsKeyPressed(KEY_W)) { activeTarget = FACE_DOWN; isClockwise = 0; }
	else if (IsKeyPressed(KEY_H)) { activeTarget = FACE_FRONT; isClockwise = 1; }
	else if (IsKeyPressed(KEY_G)) { activeTarget = FACE_FRONT; isClockwise = 0; }
	else if (IsKeyPressed(KEY_R)) { activeTarget = FACE_BACK; isClockwise = 1; }
	else if (IsKeyPressed(KEY_Y)) { activeTarget = FACE_BACK; isClockwise = 0; }
	else { return; }

	Axis axis;
	int layer, dir;

	switch (activeTarget) {
		case FACE_RIGHT:
      app->intent = (MoveIntent){
        .active = 1,
        .kind = MOVE_FACE,
        .clockwise = isClockwise ? 1 : 0,
        .face = FACE_RIGHT
      };
			break;
		case FACE_LEFT:
      app->intent = (MoveIntent){
        .active = 1,
        .kind = MOVE_FACE,
        .clockwise = isClockwise ? 0 : 1,
        .face = FACE_LEFT
      };
			break;
		case FACE_UP:
      app->intent = (MoveIntent){
        .active = 1,
        .kind = MOVE_FACE,
        .clockwise = isClockwise ? 1 : 0,
        .face = FACE_UP
      };
			break;
		case FACE_DOWN:
      app->intent = (MoveIntent){
        .active = 1,
        .kind = MOVE_FACE,
        .clockwise = isClockwise ? 0 : 1,
        .face = FACE_DOWN
      };
			break;
		case FACE_FRONT:
			app->intent = (MoveIntent){
        .active = 1,
        .kind = MOVE_FACE,
        .clockwise = isClockwise ? 1 : 0,
        .face = FACE_FRONT
      };
			break;
		case FACE_BACK:
			app->intent = (MoveIntent){
        .active = 1,
        .kind = MOVE_FACE,
        .clockwise = isClockwise ? 0 : 1,
        .face = FACE_BACK
      };
			break;
	}
}

void start_move_from_intent(App *app) {
  if (!app->intent.active || app->anim.active) return;

  CubeAnim *anim = &app->anim;
  RubiksCube *cube = &app->cube;

  anim->active = 1;
  anim->angle = 0.0f;
  anim->pieceCount = 0;
  anim->dir = app->intent.clockwise;

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
  update_animation(app);

  if (!app->anim.active && !app->intent.active) {
    if (app->scrAnim.active) {
      scramble_engine_update(app);
    } else {
      handle_cube_inputs(app);
    }
  }
  
  start_move_from_intent(app);
}
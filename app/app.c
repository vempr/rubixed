#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "app.h"
#include "../cube/cube.h"
#include "../cube/anim.h"
#include "../cube/draw.h"
#include "../scramble/scramble.h"
#include "../camera/camera.h"

void init_app_window(int width, int height, char* title) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
	InitWindow(width, height, title);
	SetWindowMinSize(width, height);
}

void init_app_cube(App *app) {
  app->cube = (RubiksCube){0};
	init_cube(&app->cube);

	scramble_init("cache");
	app->currentScramble = NULL;

	app->anim = (CubeAnim){0};
	app->anim.active = 0;
	app->scrAnim = (ScrambleAnim){0};
	app->scrAnim.active = 0;
	app->pendingMove = (PendingMove){0};
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

    app->currentScramble = NULL;
  }
}

void app_draw(App *app, OrbitCamera *c) {
  int currentWidth = GetScreenWidth();
  int currentHeight = GetScreenHeight();
  if (currentHeight <= 0) currentHeight = 1;

  BeginDrawing();

  ClearBackground((Color){238, 255, 204});

  BeginMode3D(c->camera);
  draw_cube(&app->cube, &app->anim);
  EndMode3D();

  int font_multi = currentHeight / 600;

  DrawText(
    "rubixed",
    10,
    40,
    20 * font_multi,
    BLACK
  );
  DrawText(
    app->currentScramble ? app->currentScramble : "",
    (currentWidth - MeasureText(app->currentScramble, 30 * font_multi)) / 2,
    20,
    30 * font_multi,
    BLACK
  );
  DrawFPS(10, 10);

  EndDrawing();
}
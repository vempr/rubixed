#include "../cube/cube.h"
#include "../camera/camera.h"

#ifndef APP_H
#define APP_H

typedef struct {
  char moves[256][8];
  int moveCount;
  int current;
  int active;
} ScrambleAnim;

typedef struct {
  RubiksCube cube;
  char* currentScramble;

  CubeAnim anim;
	ScrambleAnim scrAnim;
	PendingMove pendingMove;
} App;

void init_app_window(int width, int height, char* title);
void init_app_cube(App *app);
void handle_app_kb_shortcuts(App *app);
void app_draw(App *app, OrbitCamera *c);

#endif
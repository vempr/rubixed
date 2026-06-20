#include "../cube/cube.h"
#include "../camera/camera.h"
#include "../timer/timer.h"
#include "cubiecube.h"

#ifndef APP_H
#define APP_H

typedef enum {
  MODE_FREE,
  MODE_SELF_SOLVE,
  MODE_PHYSICAL_SOLVE,
  MODE_VIRTUAL_SOLVE,
} AppMode;

typedef struct {
  char moves[256][8];
  int moveCount;
  int current;
  int active;
} ScrambleAnim;

typedef struct {
  AppMode mode;

  RubiksCube cube;
  cubiecube_t *solverCube;
  int selfSolveRequested;

  char* currentScramble;

  CubeAnim anim;
	ScrambleAnim scrAnim;
	PendingMove pendingMove;
  MoveIntent intent;

  SolveTimer timer;
} App;

typedef struct {
  int key;
  Face face;
  int clockwise;
} KeyMap;

void init_app_window(int width, int height, char* title);
void init_app_cube(App *app);
void reset_session(App *app);
void handle_app_kb_shortcuts(App *app);
void app_draw(App *app, OrbitCamera *c);
void handle_cube_inputs(App *app);
void app_update(App *app);

#endif
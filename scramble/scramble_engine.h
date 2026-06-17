#include "../cube/cube.h"
#include "../cube/anim.h"

#ifndef SCRAMBLE_ENGINE_H
#define SCRAMBLE_ENGINE_H

typedef struct {
  RubiksCube *cube;
  CubeAnim *anim;
  ScrambleAnim *scrAnim;
  PendingMove *pendingMove;
} ScrambleEngine;

void scramble_engine_update(App *app);

#endif
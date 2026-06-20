#include "solver.h"
#include "cubiecube.h"
#include "../cube/cube.h"

/*
switch(cubeString[i]) {
  case 'U':
      res->f[i] = U;
      break;
  case 'R':
      res->f[i] = R;
      break;
  case 'F':
      res->f[i] = F;
      break;
  case 'D':
      res->f[i] = D;
      break;
  case 'L':
      res->f[i] = L;
      break;
  case 'B':
      res->f[i] = B;
      break;
}
*/

void solver_apply_move(cubiecube_t *cc, Face face, int clockwise) {
  cubiecube_t* moves = get_moveCube();
  int idx;

  switch(face) {
    case FACE_UP: idx = 0; break;
    case FACE_RIGHT: idx = 1; break;
    case FACE_FRONT: idx = 2; break;
    case FACE_DOWN: idx = 3; break;
    case FACE_LEFT: idx = 4; break;
    case FACE_BACK: idx = 5; break;
  }

  if (clockwise) {
    multiply(cc, &moves[idx]);
  } else {
    multiply(cc, &moves[idx]);
    multiply(cc, &moves[idx]);
    multiply(cc, &moves[idx]);
  }
}
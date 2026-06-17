#include <stdio.h>
#include <string.h>
#include "scramble_engine.h"

static int parse_face(char c) {
  switch(c) {
    case 'R': return FACE_RIGHT;
    case 'L': return FACE_LEFT;
    case 'U': return FACE_UP;
    case 'D': return FACE_DOWN;
    case 'F': return FACE_FRONT;
    case 'B': return FACE_BACK;
    default: return -1;
  }
}

void scramble_engine_update(ScrambleEngine *engine) {
  RubiksCube *cube = engine->cube;
  CubeAnim *a = engine->anim;
  ScrambleAnim *s = engine->scrAnim;
  PendingMove *p = engine->pendingMove;

  if (!s->active || a->active) {
    return;
  }

  if (s->current >= s->moveCount) {
    s->active = 0;
    return;
  }

  char* move = s->moves[s->current];
  
  int face = parse_face(move[0]);
  if (face == -1) {
    fprintf(stderr, "Invalid move: %s\n", move[0]);
    return;
  }

  int clockwise = 1;
  if (move[1] == '\'') clockwise = 0;

  a->active = 1;
  a->dir = clockwise;
  a->angle = 0.0f;
  a->pieceCount = 0;
  a->axis =
    face == FACE_RIGHT || face == FACE_LEFT ? AXIS_X :
    face == FACE_UP || face == FACE_DOWN ? AXIS_Y :
    AXIS_Z;
  
  int layer = face == FACE_RIGHT || face == FACE_UP || face == FACE_FRONT ? 1 : -1;

  for (int i = 0; i < 27; i++) {
    Cube *piece = &cube->pieces[i];

    int selected = 0;

    if (a->axis == AXIS_X && piece->x == layer) selected = 1;
    if (a->axis == AXIS_Y && piece->y == layer) selected = 1;
    if (a->axis == AXIS_Z && piece->z == layer) selected = 1;

    if (selected) {
      int idx = a->pieceCount++;
      a->indices[idx] = i;
      a->startPos[idx] = (Vector3){(float)piece->x, (float)piece->y, (float)piece->z};
      a->startOrient[idx] = piece->orient;
      memcpy(a->startColors[idx], piece->colors, 6*sizeof(int));
    }
  }

  p->active = 1;
  p->face = face;
  p->clockwise = clockwise;

  s->current++;
}
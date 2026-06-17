#include <stdio.h>
#include <string.h>
#include "../app/app.h"
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

void scramble_engine_update(App *app) {
  ScrambleAnim *s = &app->scrAnim;

  if (!s->active || app->anim.active || app->intent.active) {
    return;
  }

  if (s->current >= s->moveCount) {
    s->active = 0;
    return;
  }

  char* move = s->moves[s->current];
  int face = parse_face(move[0]);

  if (face == -1) {
    fprintf(stderr, "Invalid move: %s\n", move);
    return;
  }

  int clockwise = 1;
  if (move[1] == '\'') clockwise = 0;

  app->intent = (MoveIntent){
    .active = 1,
    .face = face,
    .clockwise = clockwise,
    .kind = MOVE_FACE,
  };

  s->current++;
}
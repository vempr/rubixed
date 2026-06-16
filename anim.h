#ifndef ANIM_H
#define ANIM_H

typedef struct {
  char moves[256][8];
  int moveCount;
  int current;
  int active;
} ScrambleAnim;

void parse_scramble(const char* scramble, ScrambleAnim* scrAnim);

#endif
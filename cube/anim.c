#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cube.h"
#include "anim.h"
#include "../app/app.h"

// parse_scramble is in anim.c because instructions don't require scramble logic
// like other functions in scramble.c; only parses scramble for scrAnim to use
void parse_scramble(const char* scramble, ScrambleAnim* scrAnim) {
  scrAnim->moveCount = 0;
  scrAnim->current = 0;
  scrAnim->active = 1;
  
  char* sc = strdup(scramble);
  if (!sc) return;
  char* token = strtok(sc, " ");

  while (token != NULL) {
    if (token[1] == '2') {
      char temp[3] = {token[0], '\0', '\0'};

      strncpy(scrAnim->moves[scrAnim->moveCount], temp, 7);
      scrAnim->moves[scrAnim->moveCount][7] = '\0';
      scrAnim->moveCount++;

      strncpy(scrAnim->moves[scrAnim->moveCount], temp, 7);
      scrAnim->moves[scrAnim->moveCount][7] = '\0';
      scrAnim->moveCount++;
    } else {
      strncpy(scrAnim->moves[scrAnim->moveCount], token, 7);
      scrAnim->moves[scrAnim->moveCount][7] = '\0';
      scrAnim->moveCount++;
    }
   
    token = strtok(NULL, " ");
  }

  free(sc);
}

void update_animation(App *app) {
  if (app->anim.active) {
    app->anim.angle += 720.0f * GetFrameTime();
    if (app->anim.angle >= 90.0f) {
      app->anim.angle = 90.0f;
      app->anim.active = 0;

      if (app->pendingMove.active) {
        rotate_face(&app->cube, app->pendingMove.face, app->pendingMove.clockwise);
        app->pendingMove.active = 0;
      }
    }
  }
}
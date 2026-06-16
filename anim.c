#include "cube.h"
#include "anim.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
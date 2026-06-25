#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "search.h"
#include "cubiecube.h"
#include "facecube.h"
#include "coordcube.h"

#include "scramble.h"
#include "../cube/cube.h"

static int scramble_initialized = 0;

void scramble_init(const char* cache_dir) {
  if (!scramble_initialized) {
    initPruning(cache_dir);
    scramble_initialized = 1;
  }
}

// generate random 54 char string cube state
// generate solution to random 54
// invert solution, turning it into a scramble
// apply scramble to cube

static char* random_state(void) {
  cubiecube_t* cc = get_cubiecube();
  cubiecube_t* moveCube = get_moveCube();
  char* facelets = malloc(56);
  facecube_t* fc;

  static int seeded = 0;
  if (!seeded) {
    srand(time(NULL));
    seeded = 1;
  }

  for (int i = 0; i < 30; i++) {
    int move = rand() % 6;
    multiply(cc, &moveCube[move]);
  }

  fc = toFaceCube(cc);
  to_String(fc, facelets);

  free(cc);
  free(fc);
  return facelets;
}

char* generate_scramble(int max_moves) {
  if (!scramble_initialized) {
    scramble_init("cache");
  }

  char* state = random_state();
  if (!state) return "R U R' U' R U R' U' R U R' U'";
  char* scramble = solution(
    state,
    max_moves,
    10000,
    0,
    "cache"
  );

  free(state);

  if (!scramble || scramble[0] == '\0' || strcmp(scramble,  "Error") == 0) {
    return strdup("R U R' U' R U R' U'");
  }

  return scramble;
}

void apply_scramble(RubiksCube* cube, const char* scramble) {
  char* sc = strdup(scramble);
  if (!sc) return;

  char* token = strtok(sc, " ");
  while (token != NULL) {
    if (token[0] != '.' && token[0] != '\0') {
      int face;
      switch(token[0]) {
        case 'R': face = FACE_RIGHT; break;
        case 'L': face = FACE_LEFT; break;
        case 'U': face = FACE_UP; break;
        case 'D': face = FACE_DOWN; break;
        case 'F': face = FACE_FRONT; break;
        case 'B': face = FACE_BACK; break;
        default:
          fprintf(stderr, "Critical move: %s\n", token);
          break;
      }

      if (token[1] == '\'') {
        rotate_face(cube, face, 0);
      } else if (token[1] == '2') {
        rotate_face(cube, face, 1);
        rotate_face(cube, face, 1);
      } else {
        rotate_face(cube, face, 1);
      }
    }
    token = strtok(NULL, " ");
  }

  free(sc);
}

char* invert_scramble(const char* scramble) {
  if (!scramble || scramble[0] == '\0' || strcmp(scramble,  "Error") == 0) {
    char* empty = malloc(1);
    if (empty) empty[0] = '\0';
    return empty;
  }

  char* copy = strdup(scramble);
  if (!copy) return NULL;

  char* tokens[256];
  int count = 0;

  char* token = strtok(copy, " ");
  while(token && count < 256) {
    if (
      token[0] == 'R' ||
      token[0] == 'L' ||
      token[0] == 'F' ||
      token[0] == 'D' ||
      token[0] == 'U' ||
      token[0] == 'B'
    ) {
      tokens[count++] = token;
    }
    
    token = strtok(NULL, " ");
  }

  if (count == 0) {
    free(copy);
    char* empty = malloc(1);
    if (empty) empty[0] = '\0';
    return empty;
  }

  char* result = malloc(1024);
  if (!result) {
    free(copy);
    return NULL;
  }
  result[0] = '\0';

  char inv[8];
  for (int i = count - 1; i >= 0; i--) {
    inv[0] = tokens[i][0];

    if (tokens[i][1] == '\'') {
      inv[1] = '\0';
    } else if (tokens[i][1] == '2') {
      inv[1] = '2';
      inv[2] = '\0';
    } else {
      inv[1] = '\'';
      inv[2] = '\0';
    }

    strcat(result, inv);
    if (i != 0) strcat(result, " ");
  }

  free(copy);
  return result;
}
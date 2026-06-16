#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "cube.h"

static Color GetRaylibColor(int ci) {
  switch(ci) {
    case 1: return RAYWHITE;
    case 2: return YELLOW;
    case 3: return ORANGE;
    case 4: return RED;
    case 5: return GREEN;
    case 6: return BLUE;
    default: return BLACK;
  };
}

static void draw_piece_rotated(Vector3 center, Axis axis, float angleDeg) {
  rlPushMatrix();

  rlPopMatrix();
}

void draw_cube (RubiksCube *cube, CubeAnim *anim) {
  float size = 1.0f;
  float stickerSize = 0.9f;
  float offset = 0.01f;

  int animated[27] = {0};
  if (anim && anim->active) {
    for (int i = 0; i < anim->pieceCount; i++) {
      animated[anim->indices[i]] = 1;
    }
  }
  
  for (int i = 0; i < 27; i++) {
    if (animated[i]) continue;

    Cube *p = &cube->pieces[i];
    Vector3 pos = {(float)p->x, (float)p->y, (float)p->z};

    DrawCube(pos, size, size, size, (Color){0,0,0,100});
    // for debug (Color){0,0,0,0}
    
    // draw stickrs

    if (p->x == 1) {
      DrawCube(
        (Vector3){pos.x + size/2 + offset, pos.y, pos.z},
        0.01f,
        stickerSize,
        stickerSize,
        GetRaylibColor(p->colors[FACE_RIGHT])
      );
    }
    if (p->x == -1) {
      DrawCube(
        (Vector3){pos.x - size/2 - offset, pos.y, pos.z},
        0.01f,
        stickerSize,
        stickerSize,
        GetRaylibColor(p->colors[FACE_LEFT])
      );
    }
    if (p->y == 1) {
      DrawCube(
        (Vector3){pos.x, pos.y + size/2 + offset, pos.z},
        stickerSize,
        0.01f,
        stickerSize,
        GetRaylibColor(p->colors[FACE_UP])
      );
    }
    if (p->y == -1) {
      DrawCube(
        (Vector3){pos.x, pos.y - size/2 - offset, pos.z},
        stickerSize,
        0.01f,
        stickerSize,
        GetRaylibColor(p->colors[FACE_DOWN])
      );
    }
    if (p->z == 1) {
      DrawCube(
        (Vector3){pos.x, pos.y, pos.z + size/2 + offset},
        stickerSize,
        stickerSize,
        0.01f,
        GetRaylibColor(p->colors[FACE_FRONT])
      );
    }
    if (p->z == -1) {
      DrawCube(
        (Vector3){pos.x, pos.y, pos.z - size/2 - offset},
        stickerSize,
        stickerSize,
        0.01f,
        GetRaylibColor(p->colors[FACE_BACK])
      );
    }
  }

  if (anim && anim->active) {
    float sign = (anim->dir == 0) ? 1.0f : -1.0f;
    float angleRad = anim->angle * DEG2RAD;

    // rotation matrix
    Matrix rotMat;
    if (anim->axis == AXIS_X) rotMat = MatrixRotateX(sign * angleRad);
    if (anim->axis == AXIS_Y) rotMat = MatrixRotateY(sign * angleRad);
    if (anim->axis == AXIS_Z) rotMat = MatrixRotateZ(sign * angleRad);

    for (int i = 0; i < anim->pieceCount; i++) {
      int idx = anim->indices[i];
      float drawAngle = sign * anim->angle;
      Vector3 startPos = anim->startPos[i];
      Vector3 currentPos = Vector3Transform(startPos, rotMat);
      draw_piece_rotated(currentPos, anim->axis, drawAngle);
    }
  }
}
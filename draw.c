#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include "cube.h"

const Color INTERNAL_COLOR = BLACK;
// debug color (Color){0,0,0,100}

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

static void draw_piece(Vector3 center, Matrix rotation, int colors[6]) {
  rlPushMatrix();
  rlTranslatef(center.x, center.y, center.z);
  rlMultMatrixf(MatrixToFloat(rotation));
  
  float size = 1.0f;
  float stickerSize = 0.9f;
  float offset = 0.01f;
  float thickness = 0.01f;

  // Matrix matModel = rlGetMatrixModelview();
  // Vector3 worldCenter = (Vector3){matModel.m12, matModel.m13, matModel.m14};
  DrawCube((Vector3){0.0f, 0.0f, 0.0f}, size, size, size, INTERNAL_COLOR);

  if (colors[FACE_RIGHT] != 0) {
    DrawCube(
      (Vector3){size/2.0f + offset, 0.0f, 0.0f},
      thickness,
      stickerSize,
      stickerSize,
      GetRaylibColor(colors[FACE_RIGHT])
    );
  }
  if (colors[FACE_LEFT] != 0) {
    DrawCube(
      (Vector3){-size/2.0f - offset, 0.0f, 0.0f},
      thickness,
      stickerSize,
      stickerSize,
      GetRaylibColor(colors[FACE_LEFT])
    );
  }
  if (colors[FACE_UP] != 0) {
    DrawCube(
      (Vector3){0.0f, size/2.0f + offset, 0.0f},
      stickerSize,
      thickness,
      stickerSize,
      GetRaylibColor(colors[FACE_UP])
    );
  }
  if (colors[FACE_DOWN] != 0) {
    DrawCube(
      (Vector3){0.0f, -size/2.0f - offset, 0.0f},
      stickerSize,
      thickness,
      stickerSize,
      GetRaylibColor(colors[FACE_DOWN])
    );
  }
  if (colors[FACE_FRONT] != 0) {
    DrawCube(
      (Vector3){0.0f, 0.0f, size/2.0f + offset},
      stickerSize,
      stickerSize,
      thickness,
      GetRaylibColor(colors[FACE_FRONT])
    );
  }
  if (colors[FACE_BACK] != 0) {
    DrawCube(
      (Vector3){0.0f, 0.0f, -size/2.0f - offset},
      stickerSize,
      stickerSize,
      thickness,
      GetRaylibColor(colors[FACE_BACK])
    );
  }

  rlPopMatrix();
}

void draw_cube(RubiksCube *cube, CubeAnim *anim) {
  float size = 1.0f;
  float stickerSize = 0.9f;
  float offset = 0.01f;
  float thickness = 0.01f;

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

    draw_piece(pos, MatrixIdentity(), p->colors);
  }

  if (anim && anim->active) {
    float angleRad = anim->angle * DEG2RAD;
    float sign = (anim->dir == 0) ? 1.0f : -1.0f;
    
    // rotation matrix
    Matrix rotMat;
    if (anim->axis == AXIS_X) rotMat = MatrixRotateX(sign * angleRad);
    if (anim->axis == AXIS_Y) rotMat = MatrixRotateY(sign * angleRad);
    if (anim->axis == AXIS_Z) rotMat = MatrixRotateZ(sign * angleRad);

    for (int i = 0; i < anim->pieceCount; i++) {
      int idx = anim->indices[i];
      Vector3 startPos = anim->startPos[i];
      Vector3 currentPos = Vector3Transform(startPos, rotMat);
      draw_piece(currentPos, rotMat, anim->startColors[i]);
    }
  }
}
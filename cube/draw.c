#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>
#include "cube.h"
#include "draw.h"

// debug color (Color){0,0,0,100}
const Color INTERNAL_COLOR = (Color){0,0,0,100};
const Color FADE_COLOR = GREEN; // {32, 33, 37, 255};
const float SIZE = 1.0f, STICKER_SIZE = 0.9f, OFFSET = 0.01f, THICKNESS = 0.01f;

static Color LerpColor(Color a, Color b, float t) {
  if (t < 0.0f) t = 0.0f;
  if (t > 1.0f) t = 1.0f;

  // printf("lerpcolor %.3f\n", t);

  return (Color){
    (unsigned char)(a.r + (b.r - a.r) * t),
    (unsigned char)(a.g + (b.g - a.g) * t),
    (unsigned char)(a.b + (b.b - a.b) * t),
    (unsigned char)(a.a + (b.a - a.a) * t),
  };
}

Color GetRaylibColor(int ci, float fade) {
  Color base;

  switch(ci) {
    case 1: base = RAYWHITE; break;
    case 2: base = YELLOW; break;
    case 3: base = ORANGE; break;
    case 4: base = RED; break;
    case 5: base = GREEN; break;
    case 6: base = BLUE; break;
    default: base = BLACK; break;
  };

  return LerpColor(base, FADE_COLOR, fade);
}

static void draw_piece(Vector3 center, Quaternion orient, int colors[6], float fade) {
  rlPushMatrix();
  rlTranslatef(center.x, center.y, center.z);

  // deprecated: replaced with quaternion system
  //rlMultMatrixf(MatrixToFloat(rotation));
  // Matrix matModel = rlGetMatrixModelview();
  // Vector3 worldCenter = (Vector3){matModel.m12, matModel.m13, matModel.m14};
  
  Matrix rotMat = QuaternionToMatrix(orient);
  rlMultMatrixf(MatrixToFloat(rotMat));

  DrawCube((Vector3){0.0f, 0.0f, 0.0f}, SIZE, SIZE, SIZE, INTERNAL_COLOR);

  if (colors[FACE_RIGHT] != 0) {
    DrawCube(
      (Vector3){SIZE/2.0f + OFFSET, 0.0f, 0.0f},
      THICKNESS,
      STICKER_SIZE,
      STICKER_SIZE,
      GetRaylibColor(colors[FACE_RIGHT], fade)
    );
  }
  if (colors[FACE_LEFT] != 0) {
    DrawCube(
      (Vector3){-SIZE/2.0f - OFFSET, 0.0f, 0.0f},
      THICKNESS,
      STICKER_SIZE,
      STICKER_SIZE,
      GetRaylibColor(colors[FACE_LEFT], fade)
    );
  }
  if (colors[FACE_UP] != 0) {
    DrawCube(
      (Vector3){0.0f, SIZE/2.0f + OFFSET, 0.0f},
      STICKER_SIZE,
      THICKNESS,
      STICKER_SIZE,
      GetRaylibColor(colors[FACE_UP], fade)
    );
  }
  if (colors[FACE_DOWN] != 0) {
    DrawCube(
      (Vector3){0.0f, -SIZE/2.0f - OFFSET, 0.0f},
      STICKER_SIZE,
      THICKNESS,
      STICKER_SIZE,
      GetRaylibColor(colors[FACE_DOWN], fade)
    );
  }
  if (colors[FACE_FRONT] != 0) {
    DrawCube(
      (Vector3){0.0f, 0.0f, SIZE/2.0f + OFFSET},
      STICKER_SIZE,
      STICKER_SIZE,
      THICKNESS,
      GetRaylibColor(colors[FACE_FRONT], fade)
    );
  }
  if (colors[FACE_BACK] != 0) {
    DrawCube(
      (Vector3){0.0f, 0.0f, -SIZE/2.0f - OFFSET},
      STICKER_SIZE,
      STICKER_SIZE,
      THICKNESS,
      GetRaylibColor(colors[FACE_BACK], fade)
    );
  }

  rlPopMatrix();
}

void draw_cube(RubiksCube *cube, CubeAnim *anim, float fade) {
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

    draw_piece(pos, p->orient, p->colors, fade);
  }

  if (anim && anim->active) {
    float angleRad = anim->angle * DEG2RAD;
    float sign = (anim->dir == 0) ? 1.0f : -1.0f;

    // deprecated
    // Matrix rotMat;
    // if (anim->axis == AXIS_X) rotMat = MatrixRotateX(sign * angleRad);
    // if (anim->axis == AXIS_Y) rotMat = MatrixRotateY(sign * angleRad);
    // if (anim->axis == AXIS_Z) rotMat = MatrixRotateZ(sign * angleRad);

    Vector3 animAxis;
    switch (anim->axis) {
      case AXIS_X: animAxis = (Vector3){1.0f, 0.0f, 0.0f}; break;
      case AXIS_Y: animAxis = (Vector3){0.0f, 1.0f, 0.0f}; break;
      case AXIS_Z: animAxis = (Vector3){0.0f, 0.0f, 1.0f}; break;
    }
    Quaternion animRot = QuaternionFromAxisAngle(animAxis, sign * angleRad);

    for (int i = 0; i < anim->pieceCount; i++) {
      int idx = anim->indices[i];

      Vector3 startPos = anim->startPos[i];
      Matrix rotMat = QuaternionToMatrix(animRot);
      Vector3 currentPos = Vector3Transform(startPos, rotMat);
      Quaternion currentOrient = QuaternionNormalize(QuaternionMultiply(animRot, anim->startOrient[i]));
      draw_piece(currentPos, currentOrient, anim->startColors[i], fade);
    }
  }
}
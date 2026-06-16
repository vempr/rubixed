#include <raylib.h>

#ifndef CUBE_H
#define CUBE_H

typedef enum {
	AXIS_X,
	AXIS_Y,
	AXIS_Z
} Axis;

typedef enum {
	FACE_UP,
	FACE_DOWN,
	FACE_LEFT,
	FACE_RIGHT,
	FACE_FRONT,
	FACE_BACK
} Face;

typedef struct {
	int x, y, z;
	int colors[6]; // each piece has 1-3 colors (except core), e.g. corner piece has 3 stickers/colors
	Quaternion orient;
} Cube;

typedef struct {
	Cube pieces[27];
} RubiksCube;

typedef struct {
	int active;
	Axis axis;
	int dir;
	float angle;
	int pieceCount;
	int indices[27];
	Vector3 startPos[27];
	Quaternion startOrient[27];
	int startColors[27][6];
} CubeAnim;

typedef struct {
	int active;
	Face face;
	int clockwise;
} PendingMove;

void rotate_face(RubiksCube *cube, Face face, int clockwise);
void init_cube(RubiksCube *cube);
void draw_cube(RubiksCube *cube, CubeAnim *anim);
void handle_cube_inputs(RubiksCube *cube, CubeAnim *anim);

#endif
#ifndef CUBE_H
#define CUBE_H

typedef enum {
	AXIS_X,
	AXIS_Y,
	AXIS_Z
} Axis;

enum FACE { FACE_UP, FACE_DOWN, FACE_LEFT, FACE_RIGHT, FACE_FRONT, FACE_BACK };

extern const int COLOR_INTERNAL;
extern const int COLOR_UP;
extern const int COLOR_DOWN;
extern const int COLOR_LEFT;
extern const int COLOR_RIGHT;
extern const int COLOR_FRONT;
extern const int COLOR_BACK;

typedef struct {
	int x, y, z;
	int colors[6]; // each piece has 1-3 colors (except core), e.g. corner piece has 3 stickers/colors
} Cube;

typedef struct {
	Cube pieces[27];
} RubiksCube;

void rotate_face(RubiksCube *cube, enum FACE face, int clockwise);
void init_cube(RubiksCube *cube);
void draw_cube(RubiksCube *cube);
void handle_cube_inputs(RubiksCube *cube);

#endif
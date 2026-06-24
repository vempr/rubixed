#include "cube.h"

#ifndef DRAW_H
#define DRAW_H

void draw_cube(RubiksCube *cube, CubeAnim *anim, float fade);
Color GetRaylibColor(int ci, float fade);

#endif
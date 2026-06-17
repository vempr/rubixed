#include <raylib.h>

#ifndef UI_H
#define UI_H

typedef struct {
  Rectangle rect;
  const char *label;
} UIButton;

typedef struct {
  float x, y;
  float spacing;
  float width;
  float height;
} UIRow;

UIRow ui_row(float x, float y, float w, float h, float spacing);
int ui_button(
  Rectangle rect,
  const char *label,
  Color base, Color text,
  Color hover,
  Color active,
  bool isActive
);
Rectangle ui_next(UIRow *row);

#endif
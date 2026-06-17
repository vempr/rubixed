#include "ui.h"

int ui_button(Rectangle rect, const char *label, Color base, Color text, Color hover, Color active, bool isActive) {
  Vector2 mouse = GetMousePosition();
  int hovered = CheckCollisionPointRec(mouse, rect);

  if (isActive) {
    DrawRectangleRec(rect, hovered ? hover : active);
  } else {
    DrawRectangleRec(rect, hovered ? hover : base);
  }
  
  int fontSize = 20;
  int textWidth = MeasureText(label, fontSize);

  DrawText(
    label,
    rect.x + (rect.width - textWidth) / 2,
    rect.y + (rect.height - fontSize) / 2,
    fontSize,
    text
  );

  return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

UIRow ui_row(float x, float y, float w, float h, float spacing) {
  return (UIRow){x, y, spacing, w, h};
}

Rectangle ui_next(UIRow *row) {
  Rectangle r = {
    row->x,
    row->y,
    row->width,
    row->height
  };

  row->x += row->width + row->spacing;
  return r;
}
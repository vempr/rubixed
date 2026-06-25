#include "ui.h"
#include "../app/theme.h"

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

void ui_tab_dialog(UITabDialog *dialog, int *isInDialogView, int tabCount, UITab *tabs) {
  if (!dialog->open) return;

  float width = GetScreenWidth() * 0.9;
  float height = GetScreenHeight() * 0.9;
  Rectangle panel = {
    (GetScreenWidth() - width) / 2,
    (GetScreenHeight() - height) / 2,
    width,
    height
  };

  DrawRectangle(
    0,
    0,
    GetScreenWidth(),
    GetScreenHeight(),
    Fade(BLACK, 0.7f)
  );

  DrawText(
    dialog->title,
    panel.x,
    panel.y,
    30 * ui_scale(),
    COLOR_TEXT
  );

  UIRow tabRow = ui_row(
    panel.x,
    panel.y + 30 * ui_scale(),
    300,
    50,
    10
  );

  for (int i = 0; i < tabCount; i++) {
    if (ui_button(
      ui_next(&tabRow),
      tabs[i].label,
      COLOR_BG,
      COLOR_TEXT,
      COLOR_ACCENT,
      COLOR_ACTIVE,
      dialog->activeTab == i
    )) {
      dialog->activeTab = i;
    }
  }

  if (ui_button(
    (Rectangle){GetScreenWidth() * 0.95, GetScreenHeight() * 0.05, 40, 40},
    "X",
    COLOR_DESTRUCTIVE,
    COLOR_TEXT,
    COLOR_ACCENT,
    COLOR_ACCENT,
    false
  )) {
    dialog->open = false;
    isInDialogView = 0;
  }
}
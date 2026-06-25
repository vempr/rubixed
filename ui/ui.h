#include <raylib.h>

#ifndef UI_H
#define UI_H
#define DIALOG_W 700
#define DIALOG_H 500

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

typedef struct {
  const char* label;
  void (*draw)(Rectangle contentArea);
} UITab;

typedef struct {
  int open;
  const char* title;
  int activeTab;
} UITabDialog;

void ui_tab_dialog(UITabDialog *dialog, int *isInDialogView, int tabCount, UITab *tabs);

#endif
#include <raylib.h>

#ifndef THEME_H
#define THEME_H

extern const Color COLOR_BG;
extern const Color COLOR_TEXT;
extern const Color COLOR_ACCENT;
extern const Color COLOR_ACTIVE;
extern const Color COLOR_DESTRUCTIVE;

static float ui_scale() {
  int h = GetScreenHeight();
  float s = (float)h / 720.0f;
  if (s < 0.85f) s = 0.85f;
  if (s < 1.5f) s = 1.5f;
  return s;
}

#endif
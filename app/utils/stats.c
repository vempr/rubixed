#include "raylib.h"
#include "stdio.h"
#include "../app.h"
#include "stats.h"
#include "../theme.h"
#include "../../ui/ui.h"

void draw_time_tab(void *ctx, Rectangle area) {
  StatsTabCtx *s = ctx;

  DrawText(
    TextFormat("%.2f", s->solves[s->current].time),
    area.x,
    area.y,
    30,
    COLOR_TEXT
  );
}

void draw_ao5_tab(void *ctx, Rectangle area) {
  App *app = ctx;

}

void draw_ao12_tab(void *ctx, Rectangle area) {
  App *app = ctx;

}
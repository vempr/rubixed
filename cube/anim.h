#include "../scramble/scramble.h"
#include "../app/app.h"

#ifndef ANIM_H
#define ANIM_H

void parse_scramble(const char* scramble, ScrambleAnim* scrAnim);
void update_animation(App *app);
void update_sticker_color(App *app);

#endif
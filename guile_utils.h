#pragma once

#include <SDL2/SDL.h>
#include <libguile.h>

int scm_to_rect(SCM list, SDL_Rect *rect);
int scm_to_color(SCM list, SDL_Color *color);
int scm_to_point(SCM list, SDL_Point *point);
void scm_to_color_default(SCM list, SDL_Color *color);

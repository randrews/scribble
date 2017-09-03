#pragma once

#include "array.h"

extern Array primitives, effects, textures;
extern SDL_Point desired_window_size, desired_logical_size;
extern int should_resize, desired_resizable;
extern SDL_mutex *renderer_mutex, *output_mutex;

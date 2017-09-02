#include "guile_utils.h"

int scm_to_rect(SCM list, SDL_Rect *rect) {
    int vals[4];

    int n = 0;
    for(; n < 4; n++) {
        if(!scm_is_pair(list)) break;
        SCM car = scm_car(list);
        list = scm_cdr(list);
        if(!scm_is_integer(car)) break;
        vals[n] = scm_to_int(car);
    }

    if(n < 4) {
        return 0;
    }

    rect->x = vals[0];
    rect->y = vals[1];
    rect->w = vals[2];
    rect->h = vals[3];
    return 1;
}

// TODO make this handle color names too?
int scm_to_color(SCM list, SDL_Color *color) {
    int vals[4];

    int n = 0;
    for(; n < 4; n++) {
        if(!scm_is_pair(list)) break;
        SCM car = scm_car(list);
        list = scm_cdr(list);
        if(!scm_is_integer(car)) break;
        vals[n] = scm_to_int(car);
    }

    if(n < 3) {
        return 0;
    }

    color->r = vals[0];
    color->g = vals[1];
    color->b = vals[2];

    if(n == 4) color->a = vals[3];
    else color->a = SDL_ALPHA_OPAQUE;

    return 1;
}

int scm_to_point(SCM list, SDL_Point *point) {
	if(!scm_is_pair(list) || !scm_is_pair(scm_cdr(list))) return 0;
	SCM car = scm_car(list);
    SCM cadr = scm_cadr(list);

    point->x = scm_to_int(car);
    point->y = scm_to_int(cadr);

    return 1;
}

void scm_to_color_default(SCM list, SDL_Color *color) {
    if(!scm_to_color(list, color)) {
        color->r = color->g = color->b = 255;
        color->a = SDL_ALPHA_OPAQUE;
    }
}

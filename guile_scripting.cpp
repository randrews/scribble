#include <SDL2/SDL.h>
#include <libguile.h>
#include "guile_scripting.h"
#include "array.h"
#include "primitive.h"
#include "main.h"
#include "effect.h"

int scm_to_rect(SCM list, SDL_Rect *rect);
int scm_to_color(SCM list, SDL_Color *color);
int scm_to_point(SCM list, SDL_Point *point);
void scm_to_color_default(SCM list, SDL_Color *color);

SCM add_line(SCM p1, SCM p2, SCM color_s);
SCM add_rect(SCM rect_s, SCM color_s, SCM fill);
SCM add_texture(SCM filename);
SCM add_sprite(SCM texture, SCM dest, SCM src);
SCM del_entity(SCM type, SCM idx);
SCM edit(SCM idx, SCM name, SCM new_value);
SCM tween(SCM idx, SCM name, SCM from, SCM to, SCM duration, SCM repeat, SCM active);
SCM start(SCM ids);

static SCM key_source;
static SCM key_dest;

void* register_functions(void*);
int guile_thread(void*);

struct Args { int argc; char **argv; };

void guile_scripting_init(int argc, char **argv) {
    scm_with_guile(&register_functions, 0);
    struct Args args = { argc, argv };
    SDL_CreateThread(&guile_thread, "guile", &args);
}

void* register_functions(void *_data) {
    key_source = scm_from_utf8_keyword("source");
    key_dest = scm_from_utf8_keyword("dest");

    scm_c_define_gsubr("line", 2, 1, 0, (scm_t_subr) &add_line);
    scm_c_define_gsubr("rect", 1, 4, 0, (scm_t_subr) &add_rect);
    scm_c_define_gsubr("texture", 1, 0, 0, (scm_t_subr) &add_texture);
    scm_c_define_gsubr("sprite", 2, 1, 0, (scm_t_subr) &add_sprite);
    scm_c_define_gsubr("del", 2, 0, 0, (scm_t_subr) &del_entity);
    scm_c_define_gsubr("edit", 3, 0, 0, (scm_t_subr) &edit);
    scm_c_define_gsubr("tween", 4, 3, 0, (scm_t_subr) &tween);
    scm_c_define_gsubr("start", 0, 0, 1, (scm_t_subr) &start);
}

int guile_thread(void *_data) {
    scm_init_guile();
    struct Args *args = (struct Args*)(_data);
    scm_shell(args->argc, args->argv);
}

/**************************************************/

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

/**************************************************/

SCM add_texture(SCM filename_s) {
    char filename[256]; memset(filename, 0, 256);
    scm_to_locale_stringbuf(filename_s, filename, 256);

    SDL_Surface *bmp = SDL_LoadBMP(filename);
    textures.lock();
    return scm_from_int(textures.add(bmp));
}

SCM add_rect(SCM rect_s, SCM color_s, SCM fill) {
    Rect *rect = new Rect();
    if(!scm_to_rect(rect_s, &(rect->rect))) {
        scm_error_scm(scm_from_locale_symbol("wrong-type-arg"),
                      scm_from_locale_string("add_rect"),
                      scm_from_locale_string("invalid rect"),
                      SCM_BOOL_F, SCM_BOOL_F);
    }

    scm_to_color_default(color_s, &(rect->color));
    rect->fill = scm_is_bool(fill) ? scm_is_true(fill) : 0;

    int idx = primitives.add(rect);
    return scm_from_int(idx);
}

SCM add_line(SCM p1, SCM p2, SCM color_s) {
    Line *line = new Line();
    if(!scm_to_point(p1, &(line->p1)) ||
       !scm_to_point(p2, &(line->p2))) {
        scm_error_scm(scm_from_locale_symbol("wrong-type-arg"),
                      scm_from_locale_string("add_line"),
                      scm_from_locale_string("invalid point"),
                      SCM_BOOL_F, SCM_BOOL_F);
    }

    scm_to_color_default(color_s, &(line->color));

    int idx = primitives.add(line);
    return scm_from_int(idx);
}

SCM add_sprite(SCM texture, SCM src, SCM dest) {

}

SCM tween(SCM idx, SCM name_s, SCM from, SCM to, SCM duration, SCM repeat, SCM active) {
    Tween *tween = new Tween();

    char *name = (char*) malloc(64); memset(name, 0, 64);
    scm_to_locale_stringbuf(name_s, name, 64);

    tween->idx = scm_to_int(idx);
    tween->name = name;
    tween->from = scm_to_double(from);
    tween->to = scm_to_double(to);
    tween->duration = scm_is_real(duration) ? scm_to_double(duration) : 0.5;
    tween->repeat = scm_is_bool(repeat) ? scm_is_true(repeat) : 0;
    tween->active = scm_is_bool(active) ? scm_is_true(active) : 0;

    return scm_from_int(effects.add(tween));
}

SCM start(SCM ids) {
    effects.lock();

    while(1) {
        if(!scm_is_pair(ids)) break;
        SCM car = scm_car(ids);
        ids = scm_cdr(ids);
        if(!scm_is_integer(car)) break;
        int id = scm_to_int(car);
        Effect *e = (Effect*) effects.edit(id);
        if(e) e->active = 1;
    }

    effects.unlock();
    return SCM_UNSPECIFIED;
}

SCM del_entity(SCM type_s, SCM idx) {
    char type[64]; memset(type, 0, 64);
    scm_to_locale_stringbuf(type_s, type, 64);

    if(!strcmp(type, "rect") ||
       !strcmp(type, "line") ||
       !strcmp(type, "sprite") ||
       !strcmp(type, "prim") ||
       !strcmp(type, "primitive")) {
        Primitive *prim = (Primitive*) primitives.del(scm_to_int(idx));
        delete prim;
    } else if(!strcmp(type, "effect") || !strcmp(type, "tween")) {
        Effect *eff = (Effect*) effects.del(scm_to_int(idx));
        delete eff;
    } else if(!strcmp(type, "texture")) {
        SDL_Surface *srf = (SDL_Surface*) textures.del(scm_to_int(idx));
        SDL_FreeSurface(srf);
    }
    return SCM_UNSPECIFIED;
}

SCM edit(SCM idx, SCM name_s, SCM new_value) {
    Primitive *p = (Primitive*) primitives.edit(scm_to_int(idx));
    if(p) {
        char name[64]; memset(name, 0, 64);
        scm_to_locale_stringbuf(name_s, name, 64);
        int val;
        if(scm_is_integer(new_value)) val = scm_to_int(new_value);
        else if(scm_is_bool(new_value)) val = scm_is_true(new_value) ? 1 : 0;
        p->change(name, val);
    }
    return SCM_UNSPECIFIED;
}

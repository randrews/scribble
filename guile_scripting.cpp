#include <SDL2/SDL.h>
#include <libguile.h>
#include "guile_scripting.h"
#include "guile_utils.h"
#include "array.h"
#include "primitive.h"
#include "main.h"
#include "effect.h"

SCM add_line(SCM p1, SCM p2, SCM rest);
SCM add_rect(SCM rect_s, SCM rest);
SCM add_texture(SCM filename);
SCM add_sprite(SCM rest);
SCM del_entity(SCM rest);
SCM edit(SCM rest);
SCM tween(SCM rest);
SCM start(SCM ids);
SCM resolution(SCM rest);

static SCM key_active, key_color, key_dest, key_duration, key_fill, key_from,
    key_id, key_logical, key_name, key_p1, key_p2, key_repeat, key_resizable,
    key_src, key_texture, key_to, key_type, key_value, key_window;

void* register_functions(void*);
int guile_thread(void*);

struct Args { int argc; char **argv; };

void guile_scripting_init(int argc, char **argv) {
    scm_with_guile(&register_functions, 0);
    struct Args args = { argc, argv };
    SDL_CreateThread(&guile_thread, "guile", &args);
}

void* register_functions(void *_data) {
    key_active = scm_from_utf8_keyword("active");
    key_color = scm_from_utf8_keyword("color");
    key_dest = scm_from_utf8_keyword("dest");
    key_duration = scm_from_utf8_keyword("duration");
    key_fill = scm_from_utf8_keyword("fill");
    key_from = scm_from_utf8_keyword("from");
    key_id = scm_from_utf8_keyword("id");
    key_logical = scm_from_utf8_keyword("logical");
    key_name = scm_from_utf8_keyword("name");
    key_repeat = scm_from_utf8_keyword("repeat");
    key_resizable = scm_from_utf8_keyword("resizable");
    key_src = scm_from_utf8_keyword("src");
    key_texture = scm_from_utf8_keyword("texture");
    key_to = scm_from_utf8_keyword("to");
    key_type = scm_from_utf8_keyword("type");
    key_value = scm_from_utf8_keyword("value");
    key_window = scm_from_utf8_keyword("window");

    scm_c_define_gsubr("line", 2, 0, 1, (scm_t_subr) &add_line);
    scm_c_define_gsubr("rect", 1, 0, 1, (scm_t_subr) &add_rect);
    scm_c_define_gsubr("texture", 1, 0, 0, (scm_t_subr) &add_texture);
    scm_c_define_gsubr("sprite", 0, 0, 1, (scm_t_subr) &add_sprite);
    scm_c_define_gsubr("del", 0, 0, 1, (scm_t_subr) &del_entity);
    scm_c_define_gsubr("edit", 0, 0, 1, (scm_t_subr) &edit);
    scm_c_define_gsubr("tween", 0, 0, 1, (scm_t_subr) &tween);
    scm_c_define_gsubr("start", 0, 0, 1, (scm_t_subr) &start);
    scm_c_define_gsubr("resolution", 0, 0, 1, (scm_t_subr) &resolution);
}

int guile_thread(void *_data) {
    scm_init_guile();
    struct Args *args = (struct Args*)(_data);
    scm_shell(args->argc, args->argv);
}

/**************************************************/

SCM resolution(SCM rest) {
    SCM window=SCM_UNDEFINED,
        logical=SCM_UNDEFINED,
        resizable=SCM_UNDEFINED;

    scm_c_bind_keyword_arguments("resolution", rest, (scm_t_keyword_arguments_flags) 0,
                                 key_logical, &logical,
                                 key_resizable, &resizable,
                                 key_window, &window);

    SDL_LockMutex(renderer_mutex);

    if(window == SCM_UNDEFINED) {
        desired_window_size.x = 0;
    } else {
        scm_to_point(window, &desired_window_size);
    }

    if(logical == SCM_UNDEFINED) {
        desired_logical_size.x = 0;
    } else {
        scm_to_point(logical, &desired_logical_size);
    }

    if(resizable == SCM_UNDEFINED) {
        desired_resizable = -1;
    } else {
        if(scm_is_bool(resizable))
            desired_resizable = scm_is_true(resizable) ? 1 : 0;
    }

    should_resize = 1;
    SDL_UnlockMutex(renderer_mutex);

    return SCM_UNSPECIFIED;
}

SCM add_texture(SCM filename_s) {
    char filename[256]; memset(filename, 0, 256);
    scm_to_locale_stringbuf(filename_s, filename, 256);

    SDL_Surface *bmp = SDL_LoadBMP(filename);
    textures.lock();
    return scm_from_int(textures.add(bmp));
}

SCM add_rect(SCM rect_s, SCM rest) {
    SCM color_s, fill;
    scm_c_bind_keyword_arguments("rect", rest, (scm_t_keyword_arguments_flags) 0,
                                 key_color, &color_s,
                                 key_fill, &fill);

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

SCM add_line(SCM p1, SCM p2, SCM rest) {
    SCM color_s;
    scm_c_bind_keyword_arguments("line", rest, (scm_t_keyword_arguments_flags) 0,
                                 key_p1, &p1,
                                 key_p2, &p2,
                                 key_color, &color_s);

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

SCM add_sprite(SCM rest) {
    SCM texture=SCM_UNDEFINED, src, dest;

    scm_c_bind_keyword_arguments("sprite", rest, (scm_t_keyword_arguments_flags) 0,
                                 key_texture, &texture,
                                 key_src, &src,
                                 key_dest, &dest);

    if(texture == SCM_UNDEFINED) {
        scm_error_scm(scm_from_locale_symbol("wrong-type-arg"),
                      scm_from_locale_string("add_sprite"),
                      scm_from_locale_string("texture is required"),
                      SCM_BOOL_F, SCM_BOOL_F);
    }

    Sprite *sprite = new Sprite();
    if(!scm_to_rect(src, &(sprite->src)) ||
       !scm_to_rect(dest, &(sprite->dest))) {
        scm_error_scm(scm_from_locale_symbol("wrong-type-arg"),
                      scm_from_locale_string("add_sprite"),
                      scm_from_locale_string("invalid rect"),
                      SCM_BOOL_F, SCM_BOOL_F);
    }

    int idx = primitives.add(sprite);
    return scm_from_int(idx);
}

SCM tween(SCM rest) {
    SCM idx=SCM_UNDEFINED,
        name_s=SCM_UNDEFINED,
        from=SCM_UNDEFINED,
        to=SCM_UNDEFINED,
        duration=SCM_UNDEFINED,
        repeat=SCM_BOOL_F,
        active=SCM_BOOL_F;

    scm_c_bind_keyword_arguments("tween", rest, (scm_t_keyword_arguments_flags) 0,
                                 key_id, &idx,
                                 key_name, &name_s,
                                 key_from, &from,
                                 key_to, &to,
                                 key_duration, &duration,
                                 key_repeat, &repeat,
                                 key_active, &active);

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

SCM del_entity(SCM rest) {
    SCM type_s=SCM_UNDEFINED, idx;
    scm_c_bind_keyword_arguments("del", rest, (scm_t_keyword_arguments_flags) 0,
                                 key_id, &idx,
                                 key_type, &type_s);

    char type[64];

    if(!SCM_UNBNDP(type_s)) {
        memset(type, 0, 64);
        scm_to_locale_stringbuf(type_s, type, 64);
    }

    if(SCM_UNBNDP(type_s) ||
       !strcmp(type, "rect") ||
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

SCM edit(SCM rest) {
    SCM idx, name_s, new_value;
    scm_c_bind_keyword_arguments("edit", rest, (scm_t_keyword_arguments_flags) 0,
                                 key_id, &idx,
                                 key_name, &name_s,
                                 key_value, &new_value);

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

#include <SDL2/SDL.h>
#include <libguile.h>
#include "scripting.h"
#include "array.h"
#include "primitive.h"
#include "main.h"
#include "effect.h"

SCM add_line(SCM x1, SCM y1, SCM x2, SCM y2, SCM rs, SCM gs, SCM bs);
SCM del_prim(SCM idx);
SCM edit(SCM idx, SCM name, SCM new_value);
SCM tween(SCM idx, SCM name, SCM from, SCM to, SCM duration, SCM repeat);

void* register_functions(void*);
int guile_thread(void*);

struct Args { int argc; char **argv; };

void scripting_init(int argc, char **argv) {
    scm_with_guile(&register_functions, 0);
    struct Args args = { argc, argv };
    SDL_CreateThread(&guile_thread, "guile", &args);
}

void* register_functions(void *_data) {
    scm_c_define_gsubr("line", 4, 3, 0, (scm_t_subr) &add_line);
    scm_c_define_gsubr("del", 1, 0, 0, (scm_t_subr) &del_prim);
    scm_c_define_gsubr("edit", 3, 0, 0, (scm_t_subr) &edit);
    scm_c_define_gsubr("tween", 4, 2, 0, (scm_t_subr) &tween);
}

int guile_thread(void *_data) {
    scm_init_guile();
    struct Args *args = (struct Args*)(_data);
    scm_shell(args->argc, args->argv);
}

/**************************************************/

SCM add_line(SCM x1, SCM y1, SCM x2, SCM y2, SCM rs, SCM gs, SCM bs) {
    Line *line = new Line();
    line->x1 = scm_to_int(x1);
    line->y1 = scm_to_int(y1);
    line->x2 = scm_to_int(x2);
    line->y2 = scm_to_int(y2);
    line->r = scm_is_integer(rs) ? scm_to_int(rs) : 255;
    line->g = scm_is_integer(gs) ? scm_to_int(gs) : 255;
    line->b = scm_is_integer(bs) ? scm_to_int(bs) : 255;

    int idx = lines.add(line);
    return scm_from_int(idx);
}

SCM tween(SCM idx, SCM name_s, SCM from, SCM to, SCM duration, SCM repeat) {
    Tween *tween = new Tween();

    char *name = (char*) malloc(64); memset(name, 0, 64);
    scm_to_locale_stringbuf(name_s, name, 64);

    tween->idx = scm_to_int(idx);
    tween->name = name;
    tween->from = scm_to_double(from);
    tween->to = scm_to_double(to);
    tween->duration = scm_is_real(duration) ? scm_to_double(duration) : 0.5;
    tween->repeat = scm_is_bool(repeat) ? scm_is_true(repeat) : 0;

    return scm_from_int(effects.add(tween));
}

SCM del_prim(SCM idx) {
    Primitive *prim = (Primitive*) lines.del(scm_to_int(idx));
    delete prim;
    return SCM_UNSPECIFIED;
}

SCM edit(SCM idx, SCM name_s, SCM new_value) {
    Primitive *p = (Primitive*) lines.edit(scm_to_int(idx));
    if(p) {
	char name[64]; memset(name, 0, 64);
	scm_to_locale_stringbuf(name_s, name, 64);
	p->change(name, scm_to_int(new_value));
    }
    return SCM_UNSPECIFIED;
}

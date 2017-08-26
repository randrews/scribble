#include <SDL2/SDL.h>
#include <libguile.h>
#include "scripting.h"
#include "primitives.h"

static SCM add_line(SCM x1, SCM y1, SCM x2, SCM y2, SCM rs, SCM gs, SCM bs);
static SCM del_line(SCM idx);

static void* register_functions(void*);
static int guile_thread(void*);

struct Args { int argc; char **argv; };

void scripting_init(int argc, char **argv) {
    scm_with_guile(&register_functions, 0);
    struct Args args = { argc, argv };
    SDL_CreateThread(&guile_thread, "guile", &args);
}

void* register_functions(void *_data) {
    scm_c_define_gsubr("add-line", 4, 3, 0, &add_line);
    scm_c_define_gsubr("del-line", 1, 0, 0, &del_line);
}

int guile_thread(void *_data) {
    scm_init_guile();
    struct Args *args = (struct Args*)(_data);
    scm_shell(args->argc, args->argv);
}

/**************************************************/

SCM add_line(SCM x1, SCM y1, SCM x2, SCM y2, SCM rs, SCM gs, SCM bs) {
    int r = scm_is_integer(rs) ? scm_to_int(rs) : 255;
    int g = scm_is_integer(gs) ? scm_to_int(gs) : 255;
    int b = scm_is_integer(bs) ? scm_to_int(bs) : 255;

    Primitive p = (Primitive) {
        LINE,
        scm_to_int(x1),
        scm_to_int(y1),
        scm_to_int(x2),
        scm_to_int(y2),
        r, g, b };
    int idx = primitive_add(p);
    return scm_from_int(idx);
}

SCM del_line(SCM idx) {
    primitive_del(scm_to_int(idx));
    return SCM_UNSPECIFIED;
}

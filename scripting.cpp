#include <SDL2/SDL.h>
#include <libguile.h>
#include "scripting.h"
#include "array.h"
#include "primitive.h"
#include "main.h"

SCM add_line(SCM x1, SCM y1, SCM x2, SCM y2, SCM rs, SCM gs, SCM bs);
SCM del_line(SCM idx);

void* register_functions(void*);
int guile_thread(void*);

struct Args { int argc; char **argv; };

void scripting_init(int argc, char **argv) {
    scm_with_guile(&register_functions, 0);
    struct Args args = { argc, argv };
    SDL_CreateThread(&guile_thread, "guile", &args);
}

void* register_functions(void *_data) {
    scm_c_define_gsubr("add-line", 4, 3, 0, (scm_t_subr) &add_line);
    scm_c_define_gsubr("del-line", 1, 0, 0, (scm_t_subr) &del_line);
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

SCM del_line(SCM idx) {
    lines.del(scm_to_int(idx));
    return SCM_UNSPECIFIED;
}

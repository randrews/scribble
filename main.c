#include <stdlib.h>
#include <SDL2/SDL.h>
#include <libguile.h>

void handleError();
int handleEvent(SDL_Event*, SDL_Renderer*);
Uint32 timerCallback(Uint32 interval, void *param);
void redraw(SDL_Renderer*);

void* register_functions(void*);
int guile_thread(void*);

SCM draw_line(SCM x1, SCM y1, SCM x2, SCM y2, SCM rs, SCM gs, SCM bs);

struct Args { int argc; char **argv; };

Uint32 lastframe = 0;
int d = 1, y = 0;
SDL_Texture *tex;

struct Line { int x1, y1, x2, y2, r, g, b; };
SDL_mutex *lines_mutex;
int lines_max = 0;
struct Line *lines;

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER); handleError();

    scm_with_guile(&register_functions, 0);
    lines_mutex = SDL_CreateMutex();

    SDL_Window *win = SDL_CreateWindow("Scribble",
                                       100, 100,
                                       640, 480,
                                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Surface *bmp = SDL_LoadBMP("pepe.bmp");
    tex = SDL_CreateTextureFromSurface(ren, bmp);

    lines = malloc(sizeof(struct Line) * 256);

    struct Args args = { argc, argv };
    SDL_CreateThread(&guile_thread, "guile", &args);

    SDL_Event event;
    SDL_TimerID timer = SDL_AddTimer(30, timerCallback, 0);

    while(1) {
        int codes = 0;

        handleError();
        SDL_WaitEvent(&event);
        do { codes |= handleEvent(&event, ren); } while(SDL_PollEvent(&event));
        if(codes & 2) redraw(ren);
        if(codes & 1) break;
    }

    SDL_FreeSurface(bmp);
	SDL_Quit();
	return 0;
}

void* register_functions(void *_data) {
    scm_c_define_gsubr("draw-line", 4, 3, 0, &draw_line);
}

SCM draw_line(SCM x1, SCM y1, SCM x2, SCM y2, SCM rs, SCM gs, SCM bs) {
    int r = scm_is_integer(rs) ? scm_to_int(rs) : 255;
    int g = scm_is_integer(gs) ? scm_to_int(gs) : 255;
    int b = scm_is_integer(bs) ? scm_to_int(bs) : 255;

    if(!SDL_LockMutex(lines_mutex)) {
        lines[lines_max++] = (struct Line) { scm_to_int(x1),
                               scm_to_int(y1),
                               scm_to_int(x2),
                               scm_to_int(y2),
                               r, g, b };
        SDL_UnlockMutex(lines_mutex);
    }

    return scm_from_int(lines_max-1);
}

int guile_thread(void *_data) {
    scm_init_guile();
    struct Args *args = (struct Args*)(_data);
    scm_shell(args->argc, args->argv);
}

int handleEvent(SDL_Event *event, SDL_Renderer *ren) {
    switch(event->type){
    case SDL_WINDOWEVENT:
        if(event->window.event == SDL_WINDOWEVENT_CLOSE) return 1;
        break;
    case SDL_QUIT: return 1; break;
    case SDL_USEREVENT: return 2;
    }

    return 0;
}

void redraw(SDL_Renderer *ren) {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, tex, 0, 0);

    SDL_SetRenderDrawColor(ren, 255, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(ren, 0, y, 639, 479-y);
    SDL_RenderDrawLine(ren, 0, 479-y, 639, y);

    if(!SDL_LockMutex(lines_mutex)) {
        for(int n = 0; n < lines_max; n++) {
            struct Line *line = lines + n;
            SDL_SetRenderDrawColor(ren, line->r, line->g, line->b, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawLine(ren, line->x1, line->y1, line->x2, line->y2);
        }
        SDL_UnlockMutex(lines_mutex);
    }

    SDL_RenderPresent(ren);
}

void handleError() {
    const char *err = SDL_GetError();
    if(strcmp(err, "")) {
        printf("SDL Error: %s\n", err);
        exit(1);
    }
}

Uint32 timerCallback(Uint32 interval, void *param) {
    if(y < 1 && d < 0) d = 1;
    if(y > 478 && d > 0) d = -1;
    y += 10*d;

    SDL_Event event;
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
    return(interval);
}

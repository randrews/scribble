#include <stdlib.h>
#include <SDL2/SDL.h>
#include <libguile.h>

void handleError();
int handleEvent(SDL_Event*, SDL_Renderer*);
Uint32 timerCallback(Uint32 interval, void *param);
void redraw(SDL_Renderer*);

void* register_functions(void*);
int guile_thread(void*);

struct Args { int argc; char **argv; };

int d=1, y=0;
SDL_Texture *tex;

int main(int argc, char **argv) {
    scm_with_guile(&register_functions, 0);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER); handleError();

    SDL_Window *win = SDL_CreateWindow("Scribble",
                                       100, 100,
                                       640, 480,
                                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Surface *bmp = SDL_LoadBMP("pepe.bmp");
    tex = SDL_CreateTextureFromSurface(ren, bmp);

    struct Args args = { argc, argv };
    SDL_CreateThread(&guile_thread, "guile", &args);

    SDL_Event event;
    SDL_TimerID timer = SDL_AddTimer(30, timerCallback, 0);
    int exit = 0;

    while(1) {
        handleError();

        SDL_WaitEvent(&event);

        do { exit |= handleEvent(&event, ren); } while(SDL_PollEvent(&event));
        if(exit) break;
    }

    SDL_FreeSurface(bmp);
	SDL_Quit();
	return 0;
}

void* register_functions(void *_data) {
}

int guile_thread(void *_data) {
    scm_init_guile();
    struct Args *args = (struct Args*)(_data);
    scm_shell(args->argc, args->argv);
    /* scm_shell(0, 0); */
}

int handleEvent(SDL_Event *event, SDL_Renderer *ren) {
    switch(event->type){
    case SDL_WINDOWEVENT:
        if(event->window.event == SDL_WINDOWEVENT_CLOSE) return 1;
        break;
    case SDL_QUIT: return 1; break;
    case SDL_USEREVENT: redraw(ren);
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

    if(y == 0 && d < 0) d = 1;
    if(y == 479 && d > 0) d = -1;
    y += d;

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
    SDL_Event event;
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
    return(interval);
}

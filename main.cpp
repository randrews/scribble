#include <stdlib.h>
#include <SDL2/SDL.h>
#include "array.h"
#include "primitive.h"
#include "guile_scripting.h"
#include "effect.h"
#include "main.h"

void handleError();
int handleEvent(SDL_Event*, SDL_Renderer*);
Uint32 timerCallback(Uint32 interval, void *param);
void redraw(SDL_Renderer*);
void resize(SDL_Renderer *ren, SDL_Window *win);

Array primitives, effects, textures;

SDL_Point desired_window_size, desired_logical_size;
int should_resize = 0, desired_resizable;
SDL_mutex *renderer_mutex;

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER); handleError();

    SDL_Window *win = SDL_CreateWindow("Scribble",
                                       100, 100,
                                       640, 480,
                                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    renderer_mutex = SDL_CreateMutex();

    guile_scripting_init(argc, argv);

    SDL_Event event;
    SDL_TimerID timer = SDL_AddTimer(30, timerCallback, 0);

    while(1) {
        int codes = 0;

        handleError();
        SDL_WaitEvent(&event);
        do { codes |= handleEvent(&event, ren); } while(SDL_PollEvent(&event));
        if(codes & 2) redraw(ren);
        if(codes & 1) break;
        if(should_resize) { resize(ren, win); should_resize = 0; }
    }

    SDL_Quit();
    return 0;
}

void resize(SDL_Renderer *ren, SDL_Window *win) {
    SDL_LockMutex(renderer_mutex);

    if(desired_window_size.x > 0) {
        SDL_SetWindowSize(win, desired_window_size.x, desired_window_size.y);
    }

    if(desired_logical_size.x > 0) {
        SDL_RenderSetLogicalSize(ren, desired_logical_size.x, desired_logical_size.y);
    }

    if(desired_resizable >= 0) {
        SDL_SetWindowResizable(win, (desired_resizable == 0 ? SDL_FALSE : SDL_TRUE));
    }

    SDL_UnlockMutex(renderer_mutex);
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
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    primitives.lock();
    for(int n = 0; n <= primitives.max(); n++) {
	Primitive *p = (Primitive*)(primitives.contents()[n]);
        if(p) p->draw(ren);
    }
    primitives.unlock();

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
    effects.lock();
    for(int n = 0; n <= effects.max(); n++) {
	Effect *eff = (Effect*)(effects.contents()[n]);
	if(eff) {
	    int finished = eff->tick((double)interval / 1000.0);
	    if(finished) {
		effects.del(n);
		delete eff;
	    }
	}
    }
    effects.unlock();

    SDL_Event event;
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
    return(interval);
}

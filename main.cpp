#include <stdlib.h>
#include <SDL2/SDL.h>
#include "array.h"
#include "primitive.h"
#include "scripting.h"
#include "effect.h"
#include "main.h"

void handleError();
int handleEvent(SDL_Event*, SDL_Renderer*);
Uint32 timerCallback(Uint32 interval, void *param);
void redraw(SDL_Renderer*);

int d = 1, y = 0;
SDL_Texture *tex;

Array lines, effects;

int main(int argc, char **argv) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER); handleError();

    SDL_Window *win = SDL_CreateWindow("Scribble",
                                       100, 100,
                                       640, 480,
                                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Surface *bmp = SDL_LoadBMP("pepe.bmp");
    tex = SDL_CreateTextureFromSurface(ren, bmp);

    scripting_init(argc, argv);

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

    lines.lock();
    for(int n = 0; n <= lines.max(); n++) {
	Primitive *p = (Primitive*)(lines.contents()[n]);
        if(p) p->draw(ren);
    }
    lines.unlock();

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

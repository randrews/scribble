#include "primitive.h"

void Primitive::draw(SDL_Renderer *ren) const {
}

void Primitive::change(const char *name, int new_value) {
}

void Line::draw(SDL_Renderer *ren) const {
    SDL_SetRenderDrawColor(ren, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(ren, x1, y1, x2, y2);
}

void Line::change(const char *name, int new_value) {
    if(!strcmp(name, "x1")) x1 = new_value;
    if(!strcmp(name, "x2")) x2 = new_value;
    if(!strcmp(name, "y1")) y1 = new_value;
    if(!strcmp(name, "y2")) y2 = new_value;
    if(!strcmp(name, "r")) r = new_value;
    if(!strcmp(name, "g")) g = new_value;
    if(!strcmp(name, "b")) b = new_value;
}

void Rect::draw(SDL_Renderer *ren) const {
    SDL_SetRenderDrawColor(ren, r, g, b, SDL_ALPHA_OPAQUE);
    if(fill) {
        SDL_RenderFillRect(ren, &rect);
    } else {
        SDL_RenderDrawRect(ren, &rect);
    }
}

void Rect::change(const char *name, int new_value) {
    if(!strcmp(name, "x")) rect.x = new_value;
    if(!strcmp(name, "y")) rect.y = new_value;
    if(!strcmp(name, "w")) rect.w = new_value;
    if(!strcmp(name, "h")) rect.h = new_value;
    if(!strcmp(name, "r")) r = new_value;
    if(!strcmp(name, "g")) g = new_value;
    if(!strcmp(name, "b")) b = new_value;
    if(!strcmp(name, "fill")) fill = (new_value != 0);
}

#include "primitive.h"
#include "main.h"

void Primitive::draw(SDL_Renderer *ren) const {
}

void Primitive::change(const char *name, int new_value) {
}

void Line::draw(SDL_Renderer *ren) const {
    SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(ren, p1.x, p1.y, p2.x, p2.y);
}

void Line::change(const char *name, int new_value) {
    if(!strcmp(name, "p1.x")) p1.x = new_value;
    if(!strcmp(name, "p2.x")) p2.x = new_value;
    if(!strcmp(name, "p1.y")) p1.y = new_value;
    if(!strcmp(name, "p2.y")) p2.y = new_value;
    if(!strcmp(name, "r")) color.r = new_value;
    if(!strcmp(name, "g")) color.g = new_value;
    if(!strcmp(name, "b")) color.b = new_value;
    if(!strcmp(name, "a")) color.a = new_value;
}

void Rect::draw(SDL_Renderer *ren) const {
    SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);
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
    if(!strcmp(name, "color.r")) color.r = new_value;
    if(!strcmp(name, "color.g")) color.g = new_value;
    if(!strcmp(name, "color.b")) color.b = new_value;
    if(!strcmp(name, "color.a")) color.a = new_value;
    if(!strcmp(name, "fill")) fill = (new_value != 0);
}

void Sprite::draw(SDL_Renderer *ren) const {
    SDL_Surface *bmp = (SDL_Surface*) textures.edit(idx);
    if(!bmp) return;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_RenderCopy(ren, tex, &src, &dest);
    SDL_DestroyTexture(tex);
}

void Sprite::change(const char *name, int new_value) {
    if(!strcmp(name, "idx")) idx = new_value;
    if(!strcmp(name, "src.x")) src.x = new_value;
    if(!strcmp(name, "src.y")) src.y = new_value;
    if(!strcmp(name, "src.w")) src.w = new_value;
    if(!strcmp(name, "src.h")) src.h = new_value;
    if(!strcmp(name, "dest.x")) dest.x = new_value;
    if(!strcmp(name, "dest.y")) dest.y = new_value;
    if(!strcmp(name, "dest.w")) dest.w = new_value;
    if(!strcmp(name, "dest.h")) dest.h = new_value;
}

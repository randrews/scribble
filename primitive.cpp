#include "primitive.h"

void Primitive::draw(SDL_Renderer *ren) const {
}

void Line::draw(SDL_Renderer *ren) const {
    SDL_SetRenderDrawColor(ren, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(ren, x1, y1, x2, y2);
}

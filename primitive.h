#pragma once
#include <SDL2/SDL.h>

class Primitive {
 public:
    virtual void draw(SDL_Renderer*) const;
};

class Line : public Primitive {
 public:
    int x1, y1, x2, y2;
    int r, g, b;

    virtual void draw(SDL_Renderer*) const;
};

class Sprite : public Primitive {
};

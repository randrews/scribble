#pragma once
#include <SDL2/SDL.h>

class Primitive {
 public:
    virtual void draw(SDL_Renderer *ren) const;
    virtual void change(const char *name, int new_value);
};

class Line : public Primitive {
 public:
    int x1, y1, x2, y2;
    int r, g, b;

    virtual void draw(SDL_Renderer*) const;
    virtual void change(const char *name, int new_value);
};

class Sprite : public Primitive {
};

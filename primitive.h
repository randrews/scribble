#pragma once
#include <SDL2/SDL.h>

class Primitive {
 public:
    virtual void draw(SDL_Renderer *ren) const;
    virtual void change(const char *name, int new_value);
};

class Line : public Primitive {
 public:
    SDL_Point p1, p2;
    SDL_Color color;

    virtual void draw(SDL_Renderer*) const;
    virtual void change(const char *name, int new_value);
};

class Rect : public Primitive {
 public:
    SDL_Rect rect;
    SDL_Color color;
    int fill;

    virtual void draw(SDL_Renderer*) const;
    virtual void change(const char *name, int new_value);
};

class Sprite : public Primitive {
 public:
    int idx;
    SDL_Rect src;
    SDL_Rect dest;

    virtual void draw(SDL_Renderer*) const;
    virtual void change(const char *name, int new_value);
};

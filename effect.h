#pragma once
#include "array.h"

class Effect {
 public:
    int active;
    virtual int tick(double time);
};

class Tween : public Effect {
 public:
    Tween();
    ~Tween();

    int idx;
    int repeat;
    int reverse;
    char *name;
    double value;
    double from, to;
    double duration;
    double passed;
    virtual int tick(double time);
};

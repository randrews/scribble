#include "effect.h"
#include <math.h>
#include "main.h"
#include "primitive.h"

int Effect::tick(double time) {
}

Tween::Tween() {
    repeat = 0;
    reverse = 0;
    active = 0;
    passed = 0.0;
    name = 0;
}

Tween::~Tween() {
    if(name) free(name);
}

int Tween::tick(double time) {
    if(!active) return 0;

    Primitive *prim = (Primitive*) primitives.edit(idx);
    if(!prim) return 1;

    passed += time;
    if(passed >= duration) passed = duration;

    if(reverse) {
        value = to - (to - from) * passed / duration;        
    } else {
        value = from + (to - from) * passed / duration;
    }

    prim->change(name, (int)rint(value));

    if(passed == duration) {
        if(repeat) {
            passed = 0.0;
            reverse = !reverse;
        } else {
            return 1;
        }
    }

    return 0;
}

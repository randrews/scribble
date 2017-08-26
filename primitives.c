#include "primitives.h"
#include <SDL2/SDL.h>

#define INITIAL_SIZE 256

static Primitive **data;
static int count;
static int used;
static int deleted;
static SDL_mutex *mutex;

void primitives_init() {
    data = malloc(sizeof(Primitive*) * INITIAL_SIZE);
    count = INITIAL_SIZE;
    used = 0;
    deleted = 0;
    mutex = SDL_CreateMutex();
}

int primitive_add(Primitive p) {
    SDL_LockMutex(mutex);

    // First, make a copy:
    Primitive *p2 = malloc(sizeof(Primitive));
    *p2 = p;

    // Now, if used < count, then just tack it on the end:
    if(used < count) {
        data[used++] = p2;
        SDL_UnlockMutex(mutex);
        return used - 1;
    }

    // If we've used the whole space, check for holes we can reuse:
    if(used == count && deleted > 0) {
        int n = -1;
        while(data[++n]);
        data[n] = p2;
        deleted--;
        SDL_UnlockMutex(mutex);
        return n;
    }

    // Otherwise, we have to resize
    Primitive **new_data = malloc(sizeof(Primitive*) * count * 2);
    for(int n = 0; n < count; n++) new_data[n] = data[n];
    count *= 2;
    free(data);
    data = new_data;

    // Now that we're resized, used+1 is free:
    data[used++] = p2;
    SDL_UnlockMutex(mutex);
    return used - 1;
}

Primitive* primitive_edit(int n) {
    if(n >= 0 && n < used && data[n])
        return data[n];
    return 0;
}

void primitive_del(int n) {
    SDL_LockMutex(mutex);
    if(n >= 0 && n < used && data[n]) {
        free(data[n]);
        data[n] = 0;
        deleted++;
    }
    SDL_UnlockMutex(mutex);
}

int primitive_max() {
    return used - 1;
}

const Primitive const** primitives() {
    return (const Primitive const**)data;
}

void primitive_lock() {
    SDL_LockMutex(mutex);
}

void primitive_unlock() {
    SDL_UnlockMutex(mutex);
}

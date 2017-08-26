#include "array.h"

#define INITIAL_SIZE 256

Array::Array() {
    data = (void**) malloc(sizeof(void*) * INITIAL_SIZE);
    count = INITIAL_SIZE;
    used = 0;
    deleted = 0;
    mutex = SDL_CreateMutex();
}

Array::~Array() {
    SDL_DestroyMutex(mutex);
    for(int n = 0; n < used; n++) {
        if(data[n]) free(data[n]);
    }
    free(data);
}

int Array::add(void *p) {
    lock();

    // Now, if used < count, then just tack it on the end:
    if(used < count) {
        data[used++] = p;
        unlock();
        return used - 1;
    }

    // If we've used the whole space, check for holes we can reuse:
    if(used == count && deleted > 0) {
        int n = -1;
        while(data[++n]);
        data[n] = p;
        deleted--;
        unlock();
        return n;
    }

    // Otherwise, we have to resize
    void **new_data = (void**) malloc(sizeof(void*) * count * 2);
    for(int n = 0; n < count; n++) new_data[n] = data[n];
    count *= 2;
    free(data);
    data = new_data;

    // Now that we're resized, used+1 is free:
    data[used++] = p;
    unlock();
    return used - 1;
}

void* Array::edit(int n) {
    if(n >= 0 && n < used && data[n])
        return data[n];
    return 0;
}

void Array::del(int n) {
    lock();
    if(n >= 0 && n < used && data[n]) {
        free(data[n]);
        data[n] = 0;
        deleted++;
    }
    unlock();
}

int Array::max() {
    return used - 1;
}

const void** Array::contents() {
    return (const void**)data;
}

void Array::lock() {
    SDL_LockMutex(mutex);
}

void Array::unlock() {
    SDL_UnlockMutex(mutex);
}

#pragma once
#include <SDL2/SDL.h>

template <typename T> class Array {
 public:
    Array();
    ~Array();

    int add(T&);
    T* edit(int);
    void del(int);
    int max();
    const T** contents();
    void lock();
    void unlock();

 private:
    T **data;
    int count;
    int used;
    int deleted;
    SDL_mutex *mutex;
};

typedef enum { LINE, SPRITE } PrimitiveType;

typedef struct Primitive {
    PrimitiveType type;
    int x, y;
    int x2, y2;
    int r, g, b;
} Primitive;


/**********************************************************************/

#define INITIAL_SIZE 256

template<typename T> Array<T>::Array() {
    data = (T**) malloc(sizeof(T*) * INITIAL_SIZE);
    count = INITIAL_SIZE;
    used = 0;
    deleted = 0;
    mutex = SDL_CreateMutex();
}

template<typename T> Array<T>::~Array() {
    SDL_DestroyMutex(mutex);
    for(int n = 0; n < used; n++) {
        if(data[n]) free(data[n]);
    }
    free(data);
}

template<typename T> int Array<T>::add(T &p) {
    lock();

    // First, make a copy:
    T *p2 = (T*) malloc(sizeof(T));
    *p2 = p;

    // Now, if used < count, then just tack it on the end:
    if(used < count) {
        data[used++] = p2;
        unlock();
        return used - 1;
    }

    // If we've used the whole space, check for holes we can reuse:
    if(used == count && deleted > 0) {
        int n = -1;
        while(data[++n]);
        data[n] = p2;
        deleted--;
        unlock();
        return n;
    }

    // Otherwise, we have to resize
    T **new_data = (T**) malloc(sizeof(T*) * count * 2);
    for(int n = 0; n < count; n++) new_data[n] = data[n];
    count *= 2;
    free(data);
    data = new_data;

    // Now that we're resized, used+1 is free:
    data[used++] = p2;
    unlock();
    return used - 1;
}

template<typename T> T* Array<T>::edit(int n) {
    if(n >= 0 && n < used && data[n])
        return data[n];
    return 0;
}

template<typename T> void Array<T>::del(int n) {
    lock();
    if(n >= 0 && n < used && data[n]) {
        free(data[n]);
        data[n] = 0;
        deleted++;
    }
    unlock();
}

template<typename T> int Array<T>::max() {
    return used - 1;
}

template<typename T> const T** Array<T>::contents() {
    return (const T**)data;
}

template<typename T> void Array<T>::lock() {
    SDL_LockMutex(mutex);
}

template<typename T> void Array<T>::unlock() {
    SDL_UnlockMutex(mutex);
}

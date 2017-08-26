#pragma once
#include <SDL2/SDL.h>

class Array {
 public:
    Array();
    ~Array();

    int add(void*);
    void* edit(int);
    void del(int);
    int max();
    const void** contents();
    void lock();
    void unlock();

 private:
    void **data;
    int count;
    int used;
    int deleted;
    SDL_mutex *mutex;
};

/**********************************************************************/

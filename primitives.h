#ifndef PRIMITIVES_H
#define PRIMITIVES_H

typedef enum { LINE, SPRITE } PrimitiveType;

typedef struct Primitive {
    PrimitiveType type;
    int x, y;
    int x2, y2;
    int r, g, b;
} Primitive;

void primitives_init();
int primitive_add(Primitive);
Primitive* primitive_edit(int);
void primitive_del(int);
int primitive_max();
const Primitive const** primitives();
void primitive_lock();
void primitive_unlock();

#endif

#ifndef VECTOR_H
#define VECTOR_H

#include "oop.h"
#include <stddef.h>  // for size_t

typedef struct Vector Vector;
typedef struct VectorClass VectorClass;
typedef struct VectorVTable VectorVTable;

struct VectorVTable {
    VTable base; // inherit from VTable

    void (*push)(Vector *Vector, void *item);
    void *(*pop)(Vector *Vector);
    void *(*get)(Vector *Vector, size_t i);
    void (*set)(Vector *Vector, size_t i, void *item);
    void (*remove)(Vector *Vector, void *item);
    void (*clear)(Vector *Vector);
    size_t (*getLength)(Vector *Vector);
};

struct Vector {
    Object base;

    // private
    size_t _length;
    size_t _max_length;
    void **_items;
};

struct VectorClass {
    Class base; // inherit from Class
};

extern VectorClass vectorClass; // singleton object is the class

#endif // VECTOR_H

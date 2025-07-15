#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>  // for size_t

typedef struct Vector Vector;
typedef struct VectorClass VectorClass;
typedef struct VectorVTable VectorVTable;

struct VectorVTable {
    void (*push)(Vector *Vector, void *item);
    void *(*pop)(Vector *Vector);
    void *(*get)(Vector *Vector, size_t i);
    void (*set)(Vector *Vector, size_t i, void *item);
    size_t (*getLength)(Vector *Vector);
};

struct Vector {
    const VectorClass *klass;

    // private
    size_t _length;
    size_t _max_length;
    void **_items;
};

struct VectorClass {
    char *class_name;
    VectorVTable *vtable;
    Vector *(*new_instance)(const VectorClass *cls);
    void (*free)(VectorClass *cls,Vector *self);
    const VectorClass *(*create_child_class)(const VectorClass *cls, const char *name);
    const VectorClass *parent;
};

extern VectorClass vectorClass; // singleton object is the class

#endif // VECTOR_H

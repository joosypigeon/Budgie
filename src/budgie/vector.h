#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>  // for size_t

typedef struct {
    size_t length;
    size_t max_length;
    void **items;
} Vector;

Vector *createVector();
void push(Vector *vector, void *item);
void *pop(Vector *vector);
void *get(Vector *vector, size_t i);
void set(Vector *vector, size_t i, void *item);
size_t getLength(Vector *vector);
void freeVector(Vector *vector);

#endif // ARRAY_H

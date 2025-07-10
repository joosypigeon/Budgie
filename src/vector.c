#include "vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

Vector *createVector() {
    Vector *vector = malloc(sizeof(Vector));
    assert(vector);  // Check for allocation failure
    vector->length = 0;
    vector->max_length = 1;
    vector->items = malloc(sizeof(void *) * vector->max_length);
    assert(vector->items);  // Check for allocation failure
    return vector;
}

void push(Vector *vector, void *item) {
    if (vector->length >= vector->max_length) {
        vector->max_length *= 2;
        void **items = realloc(vector->items, vector->max_length * sizeof(void *));
        assert(items);
        vector->items = items;
    }
    vector->items[vector->length++] = item;
}

void *pop(Vector *vector) {
    if (vector->length == 0) return NULL;
    return vector->items[--vector->length];
}

void *get(Vector *vector, size_t i) {
    assert(i < vector->length);
    return vector->items[i];
}

void set(Vector *vector, size_t i, void *item) {
    assert(i < vector->length);
    vector->items[i] = item;
}

size_t getLength(Vector *vector) {
    return vector->length;
}

void freeVector(Vector *vector) {
    if (vector) {
        free(vector->items);
        free(vector);
    }
}

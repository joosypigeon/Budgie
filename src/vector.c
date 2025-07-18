#include "budgie/vector.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


static void push(Vector *vector, void *item) {
    if (vector->_length >= vector->_max_length) {
        vector->_max_length *= 2;
        void **items = realloc(vector->_items, vector->_max_length * sizeof(void *));
        assert(items);
        vector->_items = items;
    }
    vector->_items[vector->_length++] = item;
}

static void *pop(Vector *vector) {
    if (vector->_length == 0) return NULL;
    return vector->_items[--vector->_length];
}

static void *get(Vector *vector, size_t i) {
    assert(i < vector->_length);
    return vector->_items[i];
}

static void set(Vector *vector, size_t i, void *item) {
    assert(i < vector->_length);
    vector->_items[i] = item;
}

static void vector_remove(Vector *vector, void *item) {
    for (size_t i = 0; i < vector->_length; i++) {
        if (vector->_items[i] == item) {
            // Shift items to the left
            for (size_t j = i; j < vector->_length - 1; j++) {
                vector->_items[j] = vector->_items[j + 1];
            }
            vector->_length--;
            return;
        }
    }
}

static void clear(Vector *vector) {
    vector->_length = 0; // Reset length, items are not freed
}

static size_t getLength(Vector *vector) {
    return vector->_length;
}

// all objects of same class have the same singlton vtable object
static VectorVTable vector_vtable = {
        .base = {0}, // base VTable initialized to NULLs

        // application methods
        .push = push,
        .pop = pop,
        .get = get,
        .set = set,
        .remove = vector_remove,
        .clear = clear,
        .getLength = getLength
};

// new object
static Object *vector_new_instance(const Class *cls) {
    Vector *vector = malloc(sizeof(Vector));
    assert(vector);  // Check for allocation failure
    vector->_length = 0;
    vector->_max_length = 1;
    vector->_items = malloc(sizeof(void *) * vector->_max_length);
    assert(vector->_items);  // Check for allocation failure
    ((Object *)vector)->klass = cls;
    return (Object *)vector;
}


// free object
void vector_free_instance(const Class *cls, Object *self) {
    printf("Vector::free_instance:enter\n");
    if (self) {
        Vector *vector = (Vector *)self;
        free(vector->_items);
        free(vector);
    }
    printf("Vector::free_instance:leave\n");    
}

VectorClass vectorClass = {
    .base = {
        .class_name = "Vector",
        .vtable = (VTable *)&vector_vtable,
        .parent = &class,
        .new_instance = vector_new_instance,
        .free = vector_free_instance,
    }
};
#include "budgie/oop.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

static const char *get_name(Object *self) {
    return self->klass->class_name;
}

static const char *to_string(Object *self) {
    return "Object of class root.\n";
}

static const Class *get_class(Object *self) {
    return self->klass;
}

static const Class *get_parent(Object *self) {
    return self->klass->parent;
}

// all objects of same class have the same singlton vtable object
VTable vTable = {
        .get_name = get_name,
        .to_string = to_string,
        .get_parent = get_parent
};

// instantiate object from class 
Object *object_new_instance(const Class *cls) {
    printf("new_instance: enter %p\n", cls);
    Object *object= malloc(sizeof(Object));
    assert(object);
    object->klass = cls;
    printf("new_instance: leave\n");
    return object;
}

// free object
void object_free_instance(const Class *cls, Object *self) {
    free(self);
}

Class class = {
    .class_name = "root",
    .vtable = &vTable,
    .parent = NULL,
    .new_instance = object_new_instance,
    .free = object_free_instance,
};
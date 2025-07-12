#ifndef AAPP_H
#define AAPP_H

#include "../budgie/precision.h"
#include <stddef.h>
#include "raylib.h"

typedef struct Application Application;
typedef struct ApplicationClass ApplicationClass;
typedef struct ApplicationVTable ApplicationVTable;

struct ApplicationVTable {
    const char* (*getTitle)(Application *self);
    void (*init)(Application *self);
    void (*initGraphics)(Application *self);
    void (*setView)(Application *self);
    void (*deinit)(Application *self);
    void (*loop)(Application *self);
    void (*display)(Application *self);
    void (*display_info)(Application *self, size_t Y, size_t d);
    void (*keyboard)(Application *self, KeyboardKey key);
    void (*mouseButtonPressed)(Application *self, MouseButton button);
    void (*update)(Application *self, buReal physics_delta);
};

struct Application {
    const ApplicationClass *klass;

    // private
    size_t _height;
    size_t _width;
};

struct ApplicationClass {
    char *class_name;
    ApplicationVTable *vtable;
    Application *(*new_instance)(const ApplicationClass *cls);
    void (*free)(ApplicationClass *cls,Application *self);
    const ApplicationClass *(*create_child_class)(const ApplicationClass *cls, const char *name);
    const ApplicationClass *parent;
};

extern ApplicationClass applicationClass; // singleton object is the class
//extern const ApplicationClass *ballisticClass;

extern Application *getApplication();
#endif // AAPP_H
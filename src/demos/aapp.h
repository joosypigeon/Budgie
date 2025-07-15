#ifndef AAPP_H
#define AAPP_H

#include "../budgie/oop.h"

#include "../budgie/precision.h"
#include <stddef.h>
#include "raylib.h"

typedef struct Application Application;
typedef struct ApplicationClass ApplicationClass;
typedef struct ApplicationVTable ApplicationVTable;

struct ApplicationVTable {
    VTable base; // base VTable for all classes

    // application methods
    void (*setView)(Application *self);
    const char *(*getTitle)(Application *self);
    void (*initGraphics)(Application *self);
    void (*init)(Application *self);
    void (*deinit)(Application *self);
    void (*loop)(Application *self);
    void (*display)(Application *self);
    void (*display_info)(Application *self, size_t Y, size_t d);
    void (*keyboard)(Application *self, KeyboardKey key);
    void (*mouseButtonPressed)(Application *self, MouseButton button);
    void (*update)(Application *self, buReal physics_delta);
};

struct Application {
    Object base;

    // private
    size_t _height;
    size_t _width;
};

struct ApplicationClass {
    Class base;
};

extern ApplicationVTable application_vtable; 
extern ApplicationClass applicationClass; // singleton object is the class

extern void ApplicationCreateClass();
extern Object *getApplication();
#endif // AAPP_H
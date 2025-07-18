#ifndef SPRING_H
#define SPRING_H

#include "../aapp.h"



typedef struct Spring Spring;
typedef struct SpringClass SpringClass;
typedef struct SpringVTable SpringVTable;

struct SpringVTable {
    ApplicationVTable base; // Application base VTable
};

struct Spring {
    Application base;
};

struct SpringClass {
    ApplicationClass base;

    const char *class_name; // class name
    const char *(*get_name)(SpringClass *cls);
};

Object *getApplication();

extern void SpringCreateClass();


#endif //SPRING_H
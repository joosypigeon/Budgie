#ifndef PROJECTION_H
#define PROJECTION_H

#include "../aapp.h"



typedef struct Projection Projection;
typedef struct ProjectionClass ProjectionClass;
typedef struct ProjectionVTable ProjectionVTable;

struct ProjectionVTable {
    ApplicationVTable base; // Application base VTable
};

struct Projection {
    Application base;
};

struct ProjectionClass {
    ApplicationClass base;

    const char *class_name; // class name
    const char *(*get_name)(ProjectionClass *cls);
};

Object *getApplication();

extern void ProjectionCreateClass();


#endif //PROJECTION_H
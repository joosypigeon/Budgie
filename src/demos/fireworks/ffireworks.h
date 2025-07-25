#ifndef FFIREWORKS_H
#define FFIREWORKS_H

#include "../aapp.h"



typedef struct Fireworks Fireworks;
typedef struct FireworksClass FireworksClass;
typedef struct FireworksVTable FireworksVTable;

struct FireworksVTable {
    ApplicationVTable base; // Application base VTable
};

struct Fireworks {
    Application base;
};

struct FireworksClass {
    ApplicationClass base;

    const char *class_name; // class name
    const char *(*get_name)(FireworksClass *cls);
};

Object *getApplication();

extern void FireworksCreateClass();


#endif //FFIREWORKS_H
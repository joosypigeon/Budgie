#ifndef BBALLISTIC_H
#define BBALLISTIC_H

#include "../aapp.h"

typedef struct Ballistic Ballistic;
typedef struct BallisticClass BallisticClass;
typedef struct BallisticVTable BallisticVTable;

struct BallisticVTable {
    ApplicationVTable base; // Application base VTable
};

struct Ballistic {
    Application base;
};

struct BallisticClass {
    ApplicationClass base;

    const char *class_name; // class name
    const char *(*get_name)(BallisticClass *cls);
};

Object *getApplication();

extern void BallisticCreateClass();


#endif //BBALLISTIC_H
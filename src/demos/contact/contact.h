#ifndef CONTACT_H
#define CONTACT_H

#include "../aapp.h"

typedef struct Contact Contact;
typedef struct ContactClass ContactClass;
typedef struct ContactVTable ContactVTable;

struct ContactVTable {
    ApplicationVTable base; // Application base VTable
};

struct Contact {
    Application base;
};

struct ContactClass {
    ApplicationClass base;

    const char *class_name; // class name
    const char *(*get_name)(ContactClass *cls);
};

Object *getApplication();

extern void ContactCreateClass();


#endif //CONTACT_H
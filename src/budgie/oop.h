#ifndef OOP_H
#define OOP_H

#define INSTANCE_METHOD(obj, method, ...) \
    ((Object *)(obj)->klass->vtable->method((obj), ##__VA_ARGS__))

#define CLASS_METHOD(cls, method, ...) \
    ((Class *)cls)->method(((Class *)cls), ##__VA_ARGS__)

#define INSTANCE_METHOD_AS(vtable_type, obj, method, ...) \
    (((vtable_type *)(((Object *)obj)->klass->vtable))->method((obj), ##__VA_ARGS__))

#define UNUSED(x) (void)(x)

typedef struct Object Object;
typedef struct Class Class;
typedef struct VTable VTable;

struct VTable {
    const char* (*get_name)(Object *self);
    const char* (*to_string)(Object *self);
    const Class* (*get_class)(Object *self);
    const Class* (*get_parent)(Object *self);
};

struct Object {
    const Class *klass;
};

struct Class {
    char *class_name;
    const VTable *vtable;
    const Class *parent;
    Object *(*new_instance)(const Class *cls);
    void (*free)(const Class *cls,Object *self);
};

extern const VTable vTable;
extern Class class; // singleton object is the class

void RootCreateClass(); // Create the root class

#endif // OOP_H
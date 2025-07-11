#ifndef OOP_H
#define OOP_h

#define INSTANCE_METHOD(obj, method, ...) \
    (obj)->klass->vtable->method((obj), ##__VA_ARGS__)

#define CLASS_METHOD(obj, method, ...) \
    (obj)->method((obj), ##__VA_ARGS__)

#define UNUSED(x) (void)(x)

#endif // OOP_H
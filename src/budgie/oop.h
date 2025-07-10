#ifndef OOP_H
#define OOP_h

#define CALL_METHOD(obj, method, ...) \
    (obj)->klass->vtable->method((obj), ##__VA_ARGS__)

#define CLASS(obj, method, ...) \
    (obj)->klass->method(__VA_ARGS__)

#endif // OOP_H
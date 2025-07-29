// cparticle.h
#ifndef CUBE_H
#define CUBE_H

#include "../../budgie/precision.h"
#include "../../budgie/core.h"
#include "../../budgie/oop.h"
#include "../../budgie/cparticle.h"
#include "../../budgie/vector.h"
#include "../../budgie/pcontacts.h"
#include "linalg3x3.h"
#include <stdbool.h>

typedef struct Corner {
    buVector3 r_b;
    buVector3 r_w;
    buReal restitution;
    buVector3 normal;
} Corner;

typedef struct Cube Cube;
typedef struct CubeClass CubeClass;
typedef struct CubeVTable CubeVTable;

// methods of object
struct CubeVTable {
    ParticleVTable base; // inherit from ParticleVTableVTable

    void (*integrateRigidBody)(Cube *self, buReal duration);
    void (*setRigidBody)(
                Cube *self,
                buVector3 position,
                buReal length,
                buVector3 velocity,
                buVector3 acceleration, 
                buReal damping,
                buReal inverseMass,
                Matrix3x3 R, // Rotation matrix (body to world)
                buVector3 omega_b // Angular velocity (body frame)
            );

    void (*clearTorqueAccumulator)(Cube *self);
    void (*addTorque)(Cube *self, const buVector3 torque);
    buVector3 (*getTorqueAccum)(Cube *self);

    // This is a horrid hack to apply impluse to corners below ground
    void (*applyCornerImpluse)(
        Cube *self,
        Corner **corners,
        unsigned numContacts);
};

typedef struct Cube {
    Particle base;

    buReal _length; // Length of the cube
    Matrix3x3 _Lambda;       // Inertia tensor in body frame (diagonal)
    Matrix3x3 _R; // Rotation matrix (body to world)
    buVector3 _omega_b; // Angular velocity (body frame)
    buVector3 _torqueAccum;

    Vector *_corners;
} Cube;

typedef struct CubeClass {
    ParticleClass base; // inherit from Class
    
    const char *class_name; // class name
    const char *(*get_name)(CubeClass *cls);
} CubeClass;

extern CubeClass cubeClass; // singleton object is the class
extern void CubeCreateClass();


#endif // CUBE_H
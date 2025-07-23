// cparticle.h
#ifndef CUBE_H
#define CUBE_H

#include "../../budgie/precision.h"
#include "../../budgie/core.h"
#include "../../budgie/oop.h"
#include "../../budgie/cparticle.h"
#include <stdbool.h>

typedef struct Cube Cube;
typedef struct CubeClass CubeClass;
typedef struct CubeVTable CubeVTable;

// methods of object
struct CubeVTable {
    ParticleVTable base; // inherit from ParticleVTableVTable

    void (*integrate)(Cube *self, buReal duration);
    void (*integrate)(Cube *self, buReal duration);
    void (*set)(Cube *self, buVector3 position, buVector3 velocity, buVector3 acceleration, buReal damping, buReal inverseMass);
    void (*setMass)(Cube *self, const buReal mass);
    buReal (*getMass)(Cube *self);
    void (*setInverseMass)(Cube *self, const buReal inverseMass);
    buReal (*getInverseMass)(Cube *self);
    bool (*hasFiniteMass)(Cube *self);
    void (*setDamping)(Cube *self, const buReal damping);
    buReal (*getDamping)(Cube *self);
    void (*setPosition)(Cube *self, const buVector3 position);
    buVector3 (*getPosition)(Cube *self);
    void (*setVelocity)(Cube *self, const buVector3 velocity);
    buVector3 (*getVelocity)(Cube *self);
    void (*setAcceleration)(Cube *self, const buVector3 acceleration);
    buVector3 (*getAcceleration)(Cube *self);
    void (*setAngularVelocity)(Cube *self, const buVector3 angularVelocity);
    buVector3 (*getAngularVelocity)(Cube *self);
    void (*setAngularAcceleration)(Cube *self, const buVector3 acceleration);
    buVector3 (*getAngularAcceleration)(Cube *self);
    void (*clearTorqueAccumulator)(Cube *self);
    void (*addTorque)(Cube *self, const buVector3 torque);
    buVector3 (*getTorqueAccum)(Cube *self);
    void (*clearAccumulator)(Cube *self);
    void (*addForce)(Cube *self, const buVector3 force);
    buVector3 (*getForceAccum)(Cube *self);
    void (*addTorque)(Cube *self, const buVector3 torque);
    buVector3 (*getTorqueAccum)(Cube *self);
    buReal (*getKE)(Cube *self);
    buReal (*getPE)(Cube *self, buReal y); // PE relative to y
    buReal (*getEnergy)(Cube *self); // KE + PE
};

typedef struct Cube {
    Particle base;

    buVector3 _angularVelocity;
    buVector3 _torqueAccum;
    buVector3 _angularAcceleration; 
} Cube;

typedef struct CubeClass {
    ParticleClass base; // inherit from Class
    
    const char *class_name; // class name
    const char *(*get_name)(CubeClass *cls);
} CubeClass;

extern CubeClass cubeClass; // singleton object is the class
extern void CubeCreateClass();
#endif // CUBE_H
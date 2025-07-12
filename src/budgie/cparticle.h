// cparticle.h
#ifndef CPARTICLE_H
#define CPARTICLE_H

#include "precision.h"
#include "core.h"
#include <stdbool.h>


typedef struct Particle Particle;
typedef struct ParticleClass ParticleClass;
typedef struct ParticleVTable ParticleVTable;

// methods of object
struct ParticleVTable {
        void (*integrate)(Particle *particle, buReal duration);
        void (*set)(Particle *particle, buVector3 position, buVector3 velocity, buVector3 acceleration, buReal damping, buReal inverseMass);
        void (*setMass)(Particle *particle, const buReal mass);
        buReal (*getMass)(Particle *particle);
        void (*setInverseMass)(Particle *particle, const buReal inverseMass);
        buReal (*getInverseMass)(Particle *particle);
        bool (*hasFiniteMass)(Particle *particle);
        void (*setDamping)(Particle *particle, const buReal damping);
        buReal (*getDamping)(Particle *particle);
        void (*setPosition)(Particle *particle, const buVector3 position);
        buVector3 (*getPosition)(Particle *particle);
        void (*setVelocity)(Particle *particle, const buVector3 velocity);
        buVector3 (*getVelocity)(Particle *particle);
        void (*setAcceleration)(Particle *particle, const buVector3 acceleration);
        buVector3 (*getAcceleration)(Particle *particle);
        void (*clearAccumulator)(Particle *particle);
        void (*addForce)(Particle *particle, const buVector3 force);
};

typedef struct Particle {
    // class object including vtable
    ParticleClass *klass;

    // private
    buReal _inverseMass;
    buReal _damping;
    buVector3 _position;
    buVector3 _velocity;
    buVector3 _forceAccum;
    buVector3 _acceleration; 
} Particle;

typedef struct ParticleClass {
    const char *class_name;
    ParticleVTable *vtable;
    Particle *(*new_instance)(ParticleClass *cls); // instanciate new object
    void (*free)(ParticleClass *cls, Particle *self); // free object
} ParticleClass;

extern ParticleClass particleClass; // singleton object is the class

// All child classes of particleClass must be declared here

#endif // CPARTICLE_H
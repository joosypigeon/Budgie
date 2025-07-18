// cparticle.h
#ifndef CPARTICLE_H
#define CPARTICLE_H

#include "precision.h"
#include "core.h"
#include <stdbool.h>
#include "oop.h"


typedef struct Particle Particle;
typedef struct ParticleClass ParticleClass;
typedef struct ParticleVTable ParticleVTable;

// methods of object
struct ParticleVTable {
    VTable base; // inherit from VTable

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
    buVector3 (*getForceAccum)(Particle *particle);

    buReal (*getKE)(Particle *particle);
    buReal (*getPE)(Particle *particle, buReal y); // PE relative to y
    buReal (*getEnergy)(Particle *particle); // KE + PE
};

typedef struct Particle {
    Object base;

    // private
    buReal _inverseMass;
    buReal _damping;
    buVector3 _position;
    buVector3 _initial_position; // initial position
    buVector3 _velocity;
    buVector3 _forceAccum;
    buVector3 _acceleration; 
} Particle;

typedef struct ParticleClass {
    Class base; // inherit from Class
    
    const char *class_name; // class name
    const char *(*get_name)(ParticleClass *cls);
} ParticleClass;

extern ParticleClass particleClass; // singleton object is the class

extern void ParticleCreateClass();
#endif // CPARTICLE_H
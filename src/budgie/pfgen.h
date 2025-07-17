#ifndef PFGEN_H
#define PFGEN_H

#include "precision.h"
#include "core.h"
#include "cparticle.h"
#include "vector.h"
#include "oop.h"


//////////////////////////////////////////////////////////////////
// ParticleForceGenerator interface
//////////////////////////////////////////////////////////////////
typedef struct ParticleForceGenerator ParticleForceGenerator;
typedef struct ParticleForceGeneratorClass ParticleForceGeneratorClass;
typedef struct ParticleForceGeneratorVTable ParticleForceGeneratorVTable;

// methods of object
struct ParticleForceGeneratorVTable {
    VTable base; // inherit from VTable

    void (*updateForce)(ParticleForceGenerator *self, Particle *particle, buReal duration);
};

typedef struct ParticleForceGenerator {
    Object base;
} ParticleForceGenerator;

struct ParticleForceGeneratorClass {
    Class base; // inherit from Class

    const char *class_name; // class name
    const char *(*get_name)(ParticleForceGeneratorClass *cls);
};

void ParticleForceGeneratorCreateClass();

///////////////////////////////////////////////////////////////////
// ParticleGravity - applies a gravitational force to a particle
///////////////////////////////////////////////////////////////////
typedef struct ParticleGravity ParticleGravity;
typedef struct ParticleGravityClass ParticleGravityClass;
typedef struct ParticleGravityVTable ParticleGravityVTable;

struct ParticleGravityVTable {
    ParticleForceGeneratorVTable base; // Application base VTable
};

struct ParticleGravity {
    ParticleForceGenerator base;

    /** Holds the gravitational force vector. */
    buVector3 _gravity;
};

struct ParticleGravityClass {
    ParticleForceGeneratorClass base;

    const char *class_name; // class name
    const char *(*get_name)(ParticleGravityClass *cls);
    ParticleGravity *(*new_instance)(ParticleGravityClass *cls, const buVector3 *gravity);
    void (*free)(const ParticleGravityClass *cls, ParticleGravity *self);
};

///////////////////////////////////////////////////////////////////
// ParticleDrag - applies a drag force to a particle
///////////////////////////////////////////////////////////////////
typedef struct ParticleDrag ParticleDrag;
typedef struct ParticleDragClass ParticleDragClass;
typedef struct ParticleDragVTable ParticleDragVTable;

struct ParticleDragVTable {
    ParticleForceGeneratorVTable base; // Application base VTable
};

struct ParticleDrag {
    ParticleForceGenerator base;
    
    buReal _k1; /** Holds the velocity drag coeffificent. */
    buReal _k2; /** Holds the velocity squared drag coeffificent. */
};

struct ParticleDragClass {
    ParticleForceGeneratorClass base;

    const char *class_name; // class name
    const char *(*get_name)(ParticleDragClass *cls);
    ParticleDrag *(*new_instance)(ParticleDragClass *cls, buReal k1, buReal k2);
    void (*free)(const ParticleDragClass *cls, ParticleDrag *self);
};

///////////////////////////////////////////////////////////////////
// ParticleAnchoredSpring - applies a spring force to a particle
///////////////////////////////////////////////////////////////////
typedef struct ParticleAnchoredSpring ParticleAnchoredSpring;
typedef struct ParticleAnchoredSpringClass ParticleAnchoredSpringClass;
typedef struct ParticleAnchoredSpringVTable ParticleAnchoredSpringVTable;

struct ParticleAnchoredSpringVTable {
    ParticleForceGeneratorVTable base; // Application base VTable
};

struct ParticleAnchoredSpring {
    ParticleForceGenerator base;

    buVector3 _anchor; /** The location of the anchored end of the spring. */
    buReal _springConstant; /** Holds the spring constant. */
    buReal _restLength; /** Holds the rest length of the spring. */
};

struct ParticleAnchoredSpringClass {
    ParticleForceGeneratorClass base;

    const char *class_name; // class name
    const char *(*get_name)(ParticleAnchoredSpringClass *cls);
    ParticleAnchoredSpring *(*new_instance)(ParticleAnchoredSpringClass *cls, buVector3 anchor, buReal springConstant, buReal restLength);
    void (*free)(const ParticleAnchoredSpringClass *cls, ParticleAnchoredSpring *self);
};

///////////////////////////////////////////////////////////////////
// ParticleAnchoredBungee - applies a bungee force to a particle
///////////////////////////////////////////////////////////////////
typedef struct ParticleAnchoredBungee ParticleAnchoredBungee;
typedef struct ParticleAnchoredBungeeClass ParticleAnchoredBungeeClass;
typedef struct ParticleAnchoredBungeeVTable ParticleAnchoredBungeeVTable;

struct ParticleAnchoredBungeeVTable {
    ParticleForceGeneratorVTable base; // Application base VTable
};

struct ParticleAnchoredBungee {
    ParticleForceGenerator base;

    buVector3 _anchor;
    buReal _springConstant;
    buReal _restLength;
};

struct ParticleAnchoredBungeeClass {
    ParticleForceGeneratorClass base;

    const char *class_name; // class name
    const char *(*get_name)(ParticleAnchoredBungeeClass *cls);
    ParticleAnchoredBungee *(*new_instance)(ParticleAnchoredBungeeClass *cls, buVector3 anchor, buReal springConstant, buReal restLength);
    void (*free)(const ParticleAnchoredBungeeClass *cls, ParticleAnchoredBungee *self);
};

///////////////////////////////////////////////////////////////////
// ParticleFakeSpring - applies a fake spring force to a particle
///////////////////////////////////////////////////////////////////
typedef struct ParticleFakeSpring ParticleFakeSpring;
typedef struct ParticleFakeSpringClass ParticleFakeSpringClass;
typedef struct ParticleFakeSpringVTable ParticleFakeSpringVTable;

struct ParticleFakeSpringVTable {
    ParticleForceGeneratorVTable base; // Application base VTable
};

struct ParticleFakeSpring {
    ParticleForceGenerator base;
    
    buVector3 _anchor; /** The location of the anchored end of the spring. */
    buReal _springConstant; /** Holds the spring constant. */
    buReal _damping; /** Holds the damping on the oscillation of the spring. */
};

struct ParticleFakeSpringClass {
    ParticleForceGeneratorClass base;

    const char *class_name; // class name
    const char *(*get_name)(ParticleFakeSpringClass *cls);
    ParticleFakeSpring *(*new_instance)(ParticleFakeSpringClass *cls, buVector3 anchor, buReal springConstant, buReal damping);
    void (*free)(const ParticleFakeSpringClass *cls, ParticleFakeSpring *self);
};

///////////////////////////////////////////////////////////////////
// ParticleSpring - applies a Spring force to a particle
///////////////////////////////////////////////////////////////////
typedef struct ParticleSpring ParticleSpring;
typedef struct ParticleSpringClass ParticleSpringClass;
typedef struct ParticleSpringVTable ParticleSpringVTable;

struct ParticleSpringVTable {
    ParticleForceGeneratorVTable base; // Application base VTable
};

struct ParticleSpring {
    ParticleForceGenerator base;

    
    Particle *_other; /** The particle at the other end of the spring. */
    buReal _springConstant; /** Holds the spring constant. */
    buReal _restLength; /** Holds the rest length of the spring. */
};

struct ParticleSpringClass {
    ParticleForceGeneratorClass base;

    const char *class_name; // class name
    const char *(*get_name)(ParticleSpringClass *cls);
    ParticleSpring *(*new_instance)(ParticleSpringClass *cls, Particle *other, buReal springConstant, buReal restLength);
    void (*free)(const ParticleSpringClass *cls, ParticleSpring *self);
};

///////////////////////////////////////////////////////////////////
// ParticleBungee - applies a spring force only when extended.
///////////////////////////////////////////////////////////////////
typedef struct ParticleBungee ParticleBungee;
typedef struct ParticleBungeeClass ParticleBungeeClass;
typedef struct ParticleBungeeVTable ParticleBungeeVTable;

struct ParticleBungeeVTable {
    ParticleForceGeneratorVTable base; // Application base VTable
};

struct ParticleBungee {
    ParticleForceGenerator base;

    
    Particle *_other; /** The particle at the other end of the spring. */
    buReal _springConstant; /** Holds the spring constant. */
    /**
     * Holds the length of the bungee at the point it begins to
     * generator a force.
     */
    buReal _restLength;

};

struct ParticleBungeeClass {
    ParticleForceGeneratorClass base;

    const char *class_name; // class name
    const char *(*get_name)(ParticleBungeeClass *cls);
    ParticleBungee *(*new_instance)(ParticleBungeeClass *cls, Particle *other, buReal springConstant, buReal restLength);
    void (*free)(const ParticleBungeeClass *cls, ParticleBungee *self);
};

///////////////////////////////////////////////////////////////////
// ParticleBuoyancy - applies a buoyancy force to a particle
///////////////////////////////////////////////////////////////////
typedef struct ParticleBuoyancy ParticleBuoyancy;
typedef struct ParticleBuoyancyClass ParticleBuoyancyClass;
typedef struct ParticleBuoyancyVTable ParticleBuoyancyVTable;

struct ParticleBuoyancyVTable {
    ParticleForceGeneratorVTable base; // Application base VTable
};

struct ParticleBuoyancy {
    ParticleForceGenerator base;

    /**
     * The maximum submersion depth of the object before
     * it generates its maximum boyancy force.
     */
    buReal _maxDepth;

    /**
     * The volume of the object.
     */
    buReal _volume;

    /**
     * The height of the water plane above y=0. The plane will be
     * parrallel to the XZ plane.
     */
    buReal _waterHeight;

    /**
     * The density of the liquid. Pure water has a density of
     * 1000kg per cubic meter.
     */
    buReal _liquidDensity;
};

struct ParticleBuoyancyClass {
    ParticleForceGeneratorClass base;

    const char *class_name; // class name
    const char *(*get_name)(ParticleBuoyancyClass *cls);
    ParticleBuoyancy *(*new_instance)(ParticleBuoyancyClass *cls, buReal maxDepth, buReal volume, buReal waterHeight, buReal liquidDensity);
    void (*free)(const ParticleBuoyancyClass *cls, ParticleBuoyancy *self);
};

///////////////////////////////////////////////////////////////////
// ParticleForceRegistry - manages force generators
// and their particles
///////////////////////////////////////////////////////////////////
typedef struct ParticleForceRegistry ParticleForceRegistry;
typedef struct ParticleForceRegistryClass ParticleForceRegistryClass;
typedef struct ParticleForceRegistryVTable ParticleForceRegistryVTable;

typedef struct ParticleForceRegistration {
    Particle *particle;
    ParticleForceGenerator *fg;
} ParticleForceRegistration;

// methods of object
struct ParticleForceRegistryVTable {
    VTable base; // inherit from VTable

    /**
     * Registers the given force generator to apply to the
     * given particle.
     */
    void (* add)(ParticleForceRegistry *self, Particle* particle, ParticleForceGenerator *fg);

    /**
     * Removes the given registered pair from the registry.
     * If the pair is not registered, this method will have
     * no effect.
     */
    void (* remove)(ParticleForceRegistry *self, Particle* particle, ParticleForceGenerator *fg);

    /**
     * Clears all registrations from the registry. This will
     * not delete the particles or the force generators
     * themselves, just the records of their connection.
     */
    void (* clear)(ParticleForceRegistry *self);

    /**
     * Calls all the force generators to update the forces of
     * their corresponding particles.
     */
    void (* updateForces)(ParticleForceRegistry *self, buReal duration);
};

typedef struct ParticleForceRegistry {
    Object base;

    Vector *_registrations; // holds the registrations of particles and force generators
} ParticleForceRegistry;

typedef struct ParticleForceRegistryClass {
    Class base; // inherit from Class

    const char *class_name; // class name
    const char *(*get_name)(ParticleForceRegistryClass *cls);
    ParticleForceRegistry *(*new_instance)(ParticleForceRegistryClass *cls);
    void (*free)(const ParticleForceRegistryClass *cls, ParticleForceRegistry *self);
} ParticleForceRegistryClass;

extern ParticleForceRegistryClass particleForceRegistryClass; // singleton object is the class

#endif // PFGEN_H
// cpaticle.c
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "budgie/cparticle.h"

// Method definitions

static void integrate(Particle *particle, buReal duration) {
    //printf("buVector3ParticleIntegrate:enter:duration: " REAL_FMT, duration);
    // Skip integration if particle has infinite mass (i.e. inverse mass is zero or negative)
    if (particle->_inverseMass <= 0.0f) return;

    // Ensure duration is positive and meaningful
    assert(duration > 0.0);

    // Step 1: Update position using the current velocity (ignoring acceleration contribution)
    particle->_position
        = buVector3Add(
            particle->_position,
            buVector3Scalar(particle->_velocity, duration));

    // Step 2: Update velocity using the current acceleration
    buVector3 velocity
        = buVector3Add(
            particle->_velocity,
            buVector3Scalar(particle->_acceleration, duration));

    // Step 3: Apply damping to the updated velocity
    particle->_velocity = buVector3Scalar(
        velocity,
        buPow(particle->_damping, duration));
}

static void set(Particle *particle, buVector3 position, buVector3 velocity, buVector3 acceleration, buReal damping, buReal inverseMass) {
    particle->_position = position;
    particle->_initial_position = position; // Store initial position
    particle->_velocity = velocity;
    particle->_acceleration = acceleration;
    particle->_damping = damping;
    particle->_inverseMass = inverseMass;
}

static void setMass(Particle *particle, const buReal mass) {
    assert(mass != 0);
    particle->_inverseMass = ((buReal)1.0)/mass;
}

static buReal getMass(Particle *particle) {
    if (particle->_inverseMass == 0) {
        return REAL_MAX;
    } else {
        return ((buReal)1.0)/particle->_inverseMass;
    }
}

static void setInverseMass(Particle *particle, const buReal inverseMass) {
    particle->_inverseMass = inverseMass;
}

static buReal getInverseMass(Particle *particle) {
    return particle->_inverseMass;
}

static bool hasFiniteMass(Particle *particle) {
    return particle->_inverseMass > 0.0f;
}

static void setDamping(Particle *particle, const buReal damping) {
    particle->_damping = damping;
}

buReal getDamping(Particle *particle) {
    return particle->_damping;
}

static void setPosition(Particle *particle, const buVector3 position) {
    particle->_position = position;
}

static buVector3 getPosition(Particle *particle) {
    return particle->_position;
}

static void setVelocity(Particle *particle, const buVector3 velocity) {
    particle->_velocity = velocity;
}

static buVector3 getVelocity(Particle *particle) {
    return particle->_velocity;
}

static void setAcceleration(Particle *particle, const buVector3 acceleration) {
    particle->_acceleration = acceleration;
}

static buVector3 getAcceleration(Particle *particle) {
    return particle->_acceleration;
}

static buReal getKE(Particle *particle) {
    assert(particle->_inverseMass > 0.0f); // Ensure inverse mass is positive
    buReal mass = 1.0 / particle->_inverseMass;
    buReal speedSquared = buVector3Dot(particle->_velocity, particle->_velocity);
    return 0.5 * mass * speedSquared;
}

static buReal getPE(Particle *particle, buReal y) {
    // Potential Energy = m * g * h
    // Assuming g = 9.81 m/s^2 (standard gravity)
    assert(particle->_inverseMass > 0.0); // Ensure inverse mass is positive
    buReal mass = 1.0 / particle->_inverseMass;
    const buReal g = 9.81;
    buReal height = particle->_position.y - y; // Height relative to y
    return mass * g * height;
}

static buReal getEnergy(Particle *particle) {
    // Total Energy = Kinetic Energy + Potential Energy
    buReal ke = getKE(particle);
    buReal pe = getPE(particle, particle->_initial_position.y);
    return ke + pe;
}

static void clearAccumulator(Particle *particle) {
    particle->_forceAccum = (buVector3){0.0, 0.0, 0.0};
}

static void addForce(Particle *particle, const buVector3 force) {
    particle->_forceAccum = buVector3Add(particle->_forceAccum, force);
}

static buVector3 getForceAccum(Particle *particle) {
    return particle->_forceAccum;
}

ParticleClass particleClass;
ParticleVTable particle_vtable;

// free object
void particle_free_instance(const Class *cls, Object *self) {
    printf("Particle::free_instance:enter\n");
    free(self);
    printf("Particle::free_instance:leave\n");
}

// new object
static Object *particle_new_instance(const Class *cls) {
    Particle *p = malloc(sizeof(Particle));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *get_name(ParticleClass *cls) {
    return cls->class_name;
}

static bool particle_initialized = false;
void ParticleCreateClass() {
    printf("ParticleCreateClass:enter\n");
    if (!particle_initialized) {
        printf("ParticleCreateClass:initializing\n");
        particle_vtable.base = vTable; // inherit from VTable

        // methods
        particle_vtable.integrate = integrate,
        particle_vtable.set = set,
        particle_vtable.setMass = setMass,
        particle_vtable.getMass = getMass,
        particle_vtable.setInverseMass = setInverseMass,
        particle_vtable.getInverseMass = getInverseMass,
        particle_vtable.hasFiniteMass = hasFiniteMass,
        particle_vtable.setDamping = setDamping,
        particle_vtable.getDamping = getDamping,
        particle_vtable.setPosition = setPosition,
        particle_vtable.getPosition = getPosition,
        particle_vtable.setVelocity = setVelocity,
        particle_vtable.getVelocity = getVelocity,
        particle_vtable.setAcceleration = setAcceleration,
        particle_vtable.getAcceleration = getAcceleration,
        particle_vtable.clearAccumulator = clearAccumulator,
        particle_vtable.addForce = addForce,
        particle_vtable.getForceAccum = getForceAccum,
        particle_vtable.getKE = getKE,
        particle_vtable.getPE = getPE,
        particle_vtable.getEnergy = getEnergy, 

        // init the particle class
        particleClass.base = class; // inherit from Class
        particleClass.base.vtable = (VTable *)&particle_vtable;
        particleClass.base.new_instance = particle_new_instance;
        particleClass.base.free = particle_free_instance;
        particleClass.class_name = strdup("Particle");
        particleClass.get_name = get_name;

        particle_initialized = true;
    }
    printf("ParticleCreateClass:leave\n");
}
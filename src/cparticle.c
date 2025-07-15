// cpaticle.c
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
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
    return particle->_inverseMass >= 0.0f;
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

static void clearAccumulator(Particle *particle) {
    particle->_forceAccum = (buVector3){0.0, 0.0, 0.0};
}

static void addForce(Particle *particle, const buVector3 force) {
    particle->_forceAccum = buVector3Add(particle->_forceAccum, force);
}

// all objects of same class have the same singlton vtable object
static ParticleVTable particle_vtable = {
        .base = {0}, // base VTable initialized to NULLs

        // application methods
        .integrate = integrate,
        .set = set,
        .setMass = setMass,
        .getMass = getMass,
        .setInverseMass = setInverseMass,
        .getInverseMass = getInverseMass,
        .hasFiniteMass = hasFiniteMass,
        .setDamping = setDamping,
        .getDamping = getDamping,
        .setPosition = setPosition,
        .getPosition = getPosition,
        .setVelocity = setVelocity,
        .getVelocity = getVelocity,
        .setAcceleration = setAcceleration,
        .getAcceleration = getAcceleration,
        .clearAccumulator = clearAccumulator,
        .addForce = addForce
};

 __attribute__((constructor))
void particle_init() {
    // Complite Initialize the vtable for the Particle class
    particle_vtable.base = vTable;
}


// new object
static Object *particle_new_instance(const Class *cls) {
    Particle *p = malloc(sizeof(Particle));
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

// free object
void particle_free_instance(const Class *cls, Object *self) {
    free(self);
}

ParticleClass particleClass = {
    .base = {
        .class_name = "Particle",
        .vtable = (VTable *)&particle_vtable,
        .parent = &class,
        .new_instance = particle_new_instance,
        .free = particle_free_instance,
    }
};
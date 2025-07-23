// cpaticle.c
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "cube.h"

// Method definitions

static void integrate(Cube *self, buReal duration) {
    INSTANCE_METHOD_AS(ParticleVTable, self, integrate, duration);
}

static void set(
                Cube *self,
                buVector3 position,
                buVector3 velocity,
                buVector3 acceleration, 
                buVector3 angularVelocity,
                buVector3 angularAcceleration,
                buReal damping,
                buReal inverseMass) {
    INSTANCE_METHOD_AS(ParticleVTable, self, set, position, velocity, acceleration, damping, inverseMass);
    self->_angularVelocity = angularVelocity;
    self->_angularAcceleration = angularAcceleration;
}

static void setMass(Cube *self, const buReal mass) {
    INSTANCE_METHOD_AS(ParticleVTable, self, setMass, mass);
}

static buReal getMass(Cube *self) {
    return INSTANCE_METHOD_AS(ParticleVTable, self, getMass);
}

static void setInverseMass(Cube *self, const buReal inverseMass) {
    INSTANCE_METHOD_AS(ParticleVTable, self, setInverseMass, inverseMass);
}

static buReal getInverseMass(Cube *self) {
    return INSTANCE_METHOD_AS(ParticleVTable, self, getInverseMass);
}

static bool hasFiniteMass(Cube *self) {
    return INSTANCE_METHOD_AS(ParticleVTable, self, hasFiniteMass);
}

static void setDamping(Cube *self, const buReal damping) {
    INSTANCE_METHOD_AS(ParticleVTable, self, setDamping, damping);
}

buReal getDamping(Cube *self) {
    return INSTANCE_METHOD_AS(ParticleVTable, self, getDamping);
}

static void setPosition(Cube *self, const buVector3 position) {
    INSTANCE_METHOD_AS(ParticleVTable, self, setPosition, position);
}

static buVector3 getPosition(Cube *self) {
    return INSTANCE_METHOD_AS(ParticleVTable, self, getPosition);
}

static void setVelocity(Cube *self, const buVector3 velocity) {
    INSTANCE_METHOD_AS(ParticleVTable, self, setVelocity, velocity);
}

static buVector3 getVelocity(Cube *self) {
    return INSTANCE_METHOD_AS(ParticleVTable, self, getVelocity);
}

static void setAcceleration(Cube *self, const buVector3 acceleration) {
    INSTANCE_METHOD_AS(ParticleVTable, self, setAcceleration, acceleration);
}

static buVector3 getAcceleration(Cube *self) {
    return INSTANCE_METHOD_AS(ParticleVTable, self, getAcceleration);
}

static buReal getKE(Cube *self) {
    return INSTANCE_METHOD_AS(ParticleVTable, self, getKE);
}

static buReal getPE(Cube *self, buReal y) {
    return INSTANCE_METHOD_AS(ParticleVTable, self, getPE, y);
}

static buReal getEnergy(Cube *self) {
    // Total Energy = Kinetic Energy + Potential Energy
    buReal ke = INSTANCE_METHOD_AS(ParticleVTable, self, getKE);
    buReal pe = INSTANCE_METHOD_AS(ParticleVTable, self, getPE, self->base._initial_position.y);
    // Need to add the rotational eneregy as well!!!
    return ke + pe;
}

static void clearAccumulator(Cube *self) {
    INSTANCE_METHOD_AS(ParticleVTable, self, clearAccumulator);
    self->_torqueAccum = (buVector3){0.0, 0.0, 0.0};
}

static void addForce(Cube *self, const buVector3 force) {
    INSTANCE_METHOD_AS(ParticleVTable, self, addForce, force);
}

static buVector3 getForceAccum(Cube *self) {
    return INSTANCE_METHOD_AS(ParticleVTable, self, getForceAccum);
}

static void addTorque(Cube *self, const buVector3 torque) {
    self->_torqueAccum = buVector3Add(self->_torqueAccum, torque);
}

static buVector3 getTorqueAccum(Cube *self) {
    return self->_torqueAccum;
} 

CubeClass cubeClass;
CubeVTable cube_vtable;

// free object
void cube_free_instance(const Class *cls, Object *self) {
    printf("Particle::free_instance:enter\n");
    free(self);
    printf("Particle::free_instance:leave\n");
}

// new object
static Object *cube_new_instance(const Class *cls) {
    Cube *p = malloc(sizeof(Cube));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *get_name(CubeClass *cls) {
    return cls->class_name;
}

static bool cube_initialized = false;
void CubeCreateClass() {
    printf("CubeCreateClass:enter\n");
    if (!cube_initialized) {
        printf("CubeCreateClass:initializing\n");
        ParticleCreateClass();
        cube_vtable.base = particle_vtable; // inherit from VTable

        // methods
        cube_vtable.integrate = integrate,
        cube_vtable.set = set,
        cube_vtable.setMass = setMass,
        cube_vtable.getMass = getMass,
        cube_vtable.setInverseMass = setInverseMass,
        cube_vtable.getInverseMass = getInverseMass,
        cube_vtable.hasFiniteMass = hasFiniteMass,
        cube_vtable.setDamping = setDamping,
        cube_vtable.getDamping = getDamping,
        cube_vtable.setPosition = setPosition,
        cube_vtable.getPosition = getPosition,
        cube_vtable.setVelocity = setVelocity,
        cube_vtable.getVelocity = getVelocity,
        cube_vtable.setAcceleration = setAcceleration,
        cube_vtable.getAcceleration = getAcceleration,
        cube_vtable.clearAccumulator = clearAccumulator,
        cube_vtable.addForce = addForce,
        cube_vtable.getForceAccum = getForceAccum,
        cube_vtable.addTorque = addTorque,
        cube_vtable.getTorqueAccum = getTorqueAccum,
        cube_vtable.getKE = getKE,
        cube_vtable.getPE = getPE,
        cube_vtable.getEnergy = getEnergy, 

        // init the particle class
        cubeClass.base = particleClass; // inherit from Class
        cubeClass.base.base.vtable = (VTable *)&cube_vtable;
        cubeClass.base.base.new_instance = cube_new_instance;
        cubeClass.base.base.free = cube_free_instance;
        cubeClass.class_name = strdup("Cube");
        cubeClass.get_name = get_name;

        cube_initialized = true;
    }
    printf("CubeCreateClass:leave\n");
}
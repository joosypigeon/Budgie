#include "budgie/pfgen.h"
#include "budgie/precision.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////
// ParticleForceGenerator interface
//////////////////////////////////////////////////////////////////
ParticleForceGeneratorClass particleForceGeneratorClass;
ParticleForceGeneratorVTable pfg_vtable;

 static void pfg_updateForce(const ParticleForceGenerator *self, Particle *particle, buReal duration) {
    // This is an abstract method, should be overridden in derived classes
    assert(false && "updateForce must be implemented in derived classes");  
 }

// free object
static void pfg_free_instance(const Class *cls, Object *self) {
    // This is an abstract method, should be overridden in derived classes
    assert(false && "updateForce must be implemented in derived classes");  
}

// new object
static Object *pfg_new_instance(const Class *cls) {
    // This is an abstract method, should be overridden in derived classes
    assert(false && "updateForce must be implemented in derived classes");  
    return (Object *)NULL;
}

static const char *get_name(const ParticleForceGeneratorClass *cls) {
    return cls->class_name;
}

static bool pfg_initialized = false;
void ParticleForceGeneratorCreateClass() {
    printf("ParticleForceGeneratorCreateClass:enter\n");
    if (!pfg_initialized) {
        printf("ParticleForceGeneratorCreateClass:initializing\n");
        pfg_vtable.base = vTable; // inherit from VTable

        // methods
        pfg_vtable.updateForce = pfg_updateForce;

        // init the particle class
        particleForceGeneratorClass.base = class; // inherit from Class
        particleForceGeneratorClass.base.vtable = (VTable *)&pfg_vtable;
        particleForceGeneratorClass.base.new_instance = pfg_new_instance;
        particleForceGeneratorClass.base.free = pfg_free_instance;
        particleForceGeneratorClass.class_name = strdup("ParticleForceGenerator");
        particleForceGeneratorClass.get_name = get_name;

        pfg_initialized = true;
    }
    printf("ParticleForceGeneratorCreateClass:leave\n");
}

///////////////////////////////////////////////////////////////////
// ParticleGravity - applies a gravitational force to a particle
///////////////////////////////////////////////////////////////////
ParticleGravityClass particleGravityClass;
ParticleGravityVTable pg_vtable;

// new object
static ParticleGravity *pg_new_instance(const ParticleGravityClass *cls, buVector3 gravity) {
    ParticleGravity *p = malloc(sizeof(ParticleGravity));
    assert(p);  // Check for allocation failure
    p->_gravity = gravity;
    ((Object *)p)->klass = (Class *)cls;
    return p;
}

// free object
void pg_free_instance(const ParticleGravityClass *cls, ParticleGravity *self) {
    free(self);
}

void pg_updateForce(const ParticleForceGenerator *self, Particle *particle, buReal duration) {
    // Check that we do not have infinite mass
    if (!INSTANCE_METHOD_AS(ParticleVTable, particle, hasFiniteMass)) return;

    // Apply the mass-scaled force to the particle
    buReal mass = INSTANCE_METHOD_AS(ParticleVTable, particle, getMass);
    assert(mass > 0.0); // Ensure mass is positive
    buVector3 force = buVector3Scalar(((ParticleGravity *)self)->_gravity, mass);
    INSTANCE_METHOD_AS(ParticleVTable, particle, addForce, force);
}


static const char *pg_get_name(const ParticleGravityClass *cls) {
    return cls->class_name;
}

static bool pg_initialized = false;
void ParticleGravityCreateClass() {
    printf("ParticleGravityCreateClass:enter\n");
    if (!pg_initialized) {
        printf("ParticleGravityCreateClass:initializing\n");
        ParticleForceGeneratorCreateClass();
        pg_vtable.base = pfg_vtable; // inherit from VTable

        // methods
        pg_vtable.base.updateForce = pg_updateForce;

        // init the particle class
        particleGravityClass.base = particleForceGeneratorClass; // inherit from Class
        particleGravityClass.base.base.vtable = (VTable *)&pg_vtable;
        particleGravityClass.new_instance = pg_new_instance;
        particleGravityClass.free = pg_free_instance;
        particleGravityClass.class_name = strdup("ParticleGravity");
        particleGravityClass.get_name = pg_get_name;

        pg_initialized = true;
    }
    printf("ParticleGravityCreateClass:leave\n");
}


///////////////////////////////////////////////////////////////////
// ParticleDrag - applies a drag force to a particle
///////////////////////////////////////////////////////////////////
ParticleDragClass particleDragClass;
ParticleDragVTable pd_vtable;

// new object
static ParticleDrag *pd_new_instance(const ParticleDragClass *cls, buReal k1, buReal k2) {
    ParticleDrag *p = malloc(sizeof(ParticleDrag));
    assert(p);  // Check for allocation failure
    p->_k1 = k1;
    p->_k2 = k2;
    ((Object *)p)->klass = (Class *)cls;
    return p;
}

// free object
void pd_free_instance(const ParticleDragClass *cls, ParticleDrag *self) {
    free(self);
}

void pd_updateForce(const ParticleForceGenerator *self, Particle* particle, buReal duration) {
    buVector3 force = INSTANCE_METHOD_AS(ParticleVTable, particle, getVelocity);

    // Calculate the total drag coefficient
    buReal dragCoeff = buVector3Norm(force);
    dragCoeff = ( ((ParticleDrag *)self)->_k1) * dragCoeff + ((ParticleDrag *)self)->_k2 * dragCoeff;

    // Calculate the final force and apply it
    force = buVector3Normalise(force);
    force = buVector3Scalar(force, -dragCoeff);
    INSTANCE_METHOD_AS(ParticleVTable, particle, addForce, force);
}

static const char *pd_get_name(const ParticleDragClass *cls) {
    return cls->class_name;
}

static bool pd_initialized = false;
void ParticleDragCreateClass() {
    printf("ParticleDragCreateClass:enter\n");
    if (!pd_initialized) {
        printf("ParticleDragCreateClass:initializing\n");
        ParticleForceGeneratorCreateClass();
        pd_vtable.base = pfg_vtable; // inherit from VTable

        // methods
        pd_vtable.base.updateForce = pd_updateForce;

        // init the particle class
        particleDragClass.base = particleForceGeneratorClass; // inherit from Class
        particleDragClass.base.base.vtable = (VTable *)&pd_vtable;
        particleDragClass.new_instance = pd_new_instance;
        particleDragClass.free = pd_free_instance;
        particleDragClass.class_name = strdup("ParticleDrag");
        particleDragClass.get_name = pd_get_name;

        pd_initialized = true;
    }
    printf("ParticleDragCreateClass:leave\n");
}

///////////////////////////////////////////////////////////////////
// ParticleAnchoredSpring - applies a spring force to a particle
///////////////////////////////////////////////////////////////////
ParticleAnchoredSpringClass particleAnchoredSpringClass;
ParticleAnchoredSpringVTable pas_vtable;

// new object
static ParticleAnchoredSpring *pas_new_instance(
                                    const ParticleAnchoredSpringClass *cls,
                                    buVector3 anchor,
                                    buReal springConstant,
                                    buReal restLength) {
    ParticleAnchoredSpring *p = malloc(sizeof(ParticleAnchoredSpring));
    assert(p);  // Check for allocation failure
    p->_anchor = anchor;
    p->_springConstant = springConstant;
    p->_restLength = restLength;
    ((Object *)p)->klass = (Class *)cls;
    return p;
}

// free object
void pas_free_instance(const ParticleAnchoredSpringClass *cls, ParticleAnchoredSpring *self) {
    free(self);
}

void pas_updateForce(const ParticleForceGenerator *self, Particle* particle, buReal duration) {
    // Calculate the vector of the spring
    buVector3 force = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition);
    force = buVector3Difference(force, ((ParticleAnchoredSpring *)self)->_anchor);

    // Calculate the magnitude of the force
    buReal magnitude = buVector3Norm(force);
    magnitude = (((ParticleAnchoredSpring *)self)->_restLength - magnitude) * ((ParticleAnchoredSpring *)self)->_springConstant;

    // Calculate the final force and apply it
    force = buVector3Normalise(force);
    force = buVector3Scalar(force, magnitude);
    INSTANCE_METHOD_AS(ParticleVTable, particle, addForce, force);
}

static const char *pas_get_name(const ParticleAnchoredSpringClass *cls) {
    return cls->class_name;
}

static bool pas_initialized = false;
void ParticleAnchoredSpringCreateClass() {
    printf("ParticleAnchoredSpringCreateClass:enter\n");
    if (!pas_initialized) {
        printf("ParticleAnchoredSpringCreateClass:initializing\n");
        ParticleForceGeneratorCreateClass();
        pas_vtable.base = pfg_vtable; // inherit from VTable

        // methods
        pas_vtable.base.updateForce = pas_updateForce;

        // init the particle class
        particleAnchoredSpringClass.base = particleForceGeneratorClass; // inherit from Class
        particleAnchoredSpringClass.base.base.vtable = (VTable *)&pas_vtable;
        particleAnchoredSpringClass.new_instance = pas_new_instance;
        particleAnchoredSpringClass.free = pas_free_instance;
        particleAnchoredSpringClass.class_name = strdup("ParticleAnchoredSpring");
        particleAnchoredSpringClass.get_name = pas_get_name;

        pas_initialized = true;
    }
    printf("ParticleAnchoredSpringCreateClass:leave\n");
}



///////////////////////////////////////////////////////////////////
// ParticleSpring - applies a Spring force to a particle
///////////////////////////////////////////////////////////////////
ParticleSpringClass particleSpringClass;
ParticleSpringVTable ps_vtable;

// new object
static ParticleSpring *ps_new_instance(
                                    const ParticleSpringClass *cls,
                                    Particle *other, buReal sc, buReal rl) {
    ParticleSpring *p = malloc(sizeof(ParticleSpring));
    assert(p);  // Check for allocation failure
    p->_other = other;
    p->_springConstant = sc;
    p->_restLength = rl;
    ((Object *)p)->klass = (Class *)cls;
    return p;
}

// free object
void ps_free_instance(const ParticleSpringClass *cls, ParticleSpring *self) {
    free(self);
}

void ps_updateForce(const ParticleForceGenerator *self, Particle* particle, buReal duration)
{
    // Calculate the vector of the spring
    buVector3 force = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition);
    buVector3 other = INSTANCE_METHOD_AS(ParticleVTable, ((ParticleSpring *)self)->_other, getPosition);
    force = buVector3Difference(force, other);

    // Calculate the magnitude of the force
    buReal magnitude = buVector3Norm(force);
    magnitude = magnitude - ((ParticleSpring *)self)->_restLength;
    magnitude *= ((ParticleSpring *)self)->_springConstant;

    // Calculate the final force and apply it
    force = buVector3Normalise(force);
    force = buVector3Scalar(force, -magnitude);
    INSTANCE_METHOD_AS(ParticleVTable, particle, addForce, force);
}

static const char *ps_get_name(const ParticleSpringClass *cls) {
    return cls->class_name;
}

static bool ps_initialized = false;
void ParticleSpringCreateClass() {
    printf("ParticleSpringCreateClass:enter\n");
    if (!ps_initialized) {
        printf("ParticleSpringCreateClass:initializing\n");
        ParticleForceGeneratorCreateClass();
        ps_vtable.base = pfg_vtable; // inherit from VTable

        // methods
        ps_vtable.base.updateForce = ps_updateForce;

        // init the particle class
        particleSpringClass.base = particleForceGeneratorClass; // inherit from Class
        particleSpringClass.base.base.vtable = (VTable *)&ps_vtable;
        particleSpringClass.new_instance = ps_new_instance;
        particleSpringClass.free = ps_free_instance;
        particleSpringClass.class_name = strdup("ParticleSpring");
        particleSpringClass.get_name = ps_get_name;

        ps_initialized = true;
    }
    printf("ParticleSpringCreateClass:leave\n");
}

///////////////////////////////////////////////////////////////////
// ParticleBuoyancy - applies a buoyancy force to a particle
///////////////////////////////////////////////////////////////////
ParticleBuoyancyClass particleBuoyancyClass;
ParticleBuoyancyVTable pb_vtable;

// new object
static ParticleBuoyancy *pb_new_instance(
                                    const ParticleBuoyancyClass *cls,
                                    buReal maxDepth,
                                    buReal volume,
                                    buReal waterHeight,
                                    buReal liquidDensity) {
    ParticleBuoyancy *p = malloc(sizeof(ParticleBuoyancy));
    assert(p);  // Check for allocation failure
    p->_maxDepth = maxDepth;
    p->_volume = volume;
    p->_waterHeight = waterHeight;
    p->_liquidDensity = liquidDensity;
    ((Object *)p)->klass = (Class *)cls;
    return p;
}

// free object
void pb_free_instance(const ParticleBuoyancyClass *cls, ParticleBuoyancy *self) {
    free(self);
}


void pb_updateForce(const ParticleForceGenerator *self, Particle* particle, buReal duration)
{
    // Calculate the submersion depth
    buReal depth = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition).y;

    // Check if we're out of the water
    if (depth >= ((ParticleBuoyancy *)self)->_waterHeight + ((ParticleBuoyancy *)self)->_maxDepth) return;
    buVector3 force = (buVector3){0.0, 0.0 , 0.0};

    // Check if we're at maximum depth
    if (depth <= ((ParticleBuoyancy *)self)->_waterHeight - ((ParticleBuoyancy *)self)->_maxDepth) {
        force.y = ((ParticleBuoyancy *)self)->_liquidDensity * ((ParticleBuoyancy *)self)->_volume;
        INSTANCE_METHOD_AS(ParticleVTable, particle, addForce, force);
        return;
    }

    // Otherwise we are partly submerged
    force.y = ((ParticleBuoyancy *)self)->_liquidDensity * ((ParticleBuoyancy *)self)->_volume *
        (depth - ((ParticleBuoyancy *)self)->_maxDepth - ((ParticleBuoyancy *)self)->_waterHeight) / (2 * ((ParticleBuoyancy *)self)->_maxDepth);
    INSTANCE_METHOD_AS(ParticleVTable, particle, addForce, force);
}

static const char *pb_get_name(const ParticleBuoyancyClass *cls) {
    return cls->class_name;
}

static bool pb_initialized = false;
void ParticleBuoyancyCreateClass() {
    printf("ParticleBuoyancyCreateClass:enter\n");
    if (!pb_initialized) {
        printf("ParticleBuoyancyCreateClass:initializing\n");
        ParticleForceGeneratorCreateClass();
        pb_vtable.base = pfg_vtable; // inherit from VTable

        // methods
        pb_vtable.base.updateForce = pb_updateForce;

        // init the particle class
        particleBuoyancyClass.base = particleForceGeneratorClass; // inherit from Class
        particleBuoyancyClass.base.base.vtable = (VTable *)&pb_vtable;
        particleBuoyancyClass.new_instance = pb_new_instance;
        particleBuoyancyClass.free = pb_free_instance;
        particleBuoyancyClass.class_name = strdup("ParticleBuoyancy");
        particleBuoyancyClass.get_name = pb_get_name;

        pb_initialized = true;
    }
    printf("ParticleBuoyancyCreateClass:leave\n");
}

///////////////////////////////////////////////////////////////////
// ParticleAnchoredBungee - applies a bungee force to a particle
///////////////////////////////////////////////////////////////////
ParticleAnchoredBungeeClass particleAnchoredBungeeClass;
ParticleAnchoredBungeeVTable pab_vtable;

// new object
static ParticleAnchoredBungee *pab_new_instance(
                                    const ParticleAnchoredBungeeClass *cls,
                                    buVector3 anchor, buReal sc, buReal rl) {
    ParticleAnchoredBungee *p = malloc(sizeof(ParticleAnchoredBungee));
    assert(p);  // Check for allocation failure
    p->_anchor = anchor;
    p->_springConstant = sc;
    p->_restLength = rl;
    ((Object *)p)->klass = (Class *)cls;
    return p;
}

// free object
void pab_free_instance(const ParticleAnchoredBungeeClass *cls, ParticleAnchoredBungee *self) {
    free(self);
}

void pab_updateForce(const ParticleForceGenerator *self, Particle* particle, buReal duration) {
    // Calculate the vector of the spring
    buVector3 force = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition);
    force = buVector3Difference(force, ((ParticleAnchoredBungee *)self)->_anchor);

    // Calculate the magnitude of the force
    buReal magnitude = buVector3Norm(force);
    if (magnitude < ((ParticleAnchoredBungee *)self)->_restLength) return;

    magnitude = magnitude - ((ParticleAnchoredBungee *)self)->_restLength;
    magnitude *= ((ParticleAnchoredBungee *)self)->_springConstant;

    // Calculate the final force and apply it
    force = buVector3Normalise(force);
    force = buVector3Scalar(force, -magnitude);
    INSTANCE_METHOD_AS(ParticleVTable, particle, addForce, force);
}

static const char *pab_get_name(const ParticleAnchoredBungeeClass *cls) {
    return cls->class_name;
}

static bool pab_initialized = false;
void ParticleAnchoredBungeeCreateClass() {
    printf("ParticleAnchoredBungeeCreateClass:enter\n");
    if (!pab_initialized) {
        printf("ParticleAnchoredBungeeCreateClass:initializing\n");
        ParticleForceGeneratorCreateClass();
        pab_vtable.base = pfg_vtable; // inherit from VTable

        // methods
        pab_vtable.base.updateForce = pab_updateForce;

        // init the particle class
        particleAnchoredBungeeClass.base = particleForceGeneratorClass; // inherit from Class
        particleAnchoredBungeeClass.base.base.vtable = (VTable *)&pab_vtable;
        particleAnchoredBungeeClass.new_instance = pab_new_instance;
        particleAnchoredBungeeClass.free = pab_free_instance;
        particleAnchoredBungeeClass.class_name = strdup("ParticleAnchoredBungee");
        particleAnchoredBungeeClass.get_name = pab_get_name;

        pab_initialized = true;
    }
    printf("ParticleAnchoredBungeeCreateClass:leave\n");
}


///////////////////////////////////////////////////////////////////
// ParticleBungee - applies a spring force only when extended.
///////////////////////////////////////////////////////////////////
ParticleBungeeClass particleBungeeClass;
ParticleBungeeVTable pbu_vtable;

// new object
static ParticleBungee *pbu_new_instance(
                                    const ParticleBungeeClass *cls,
                                    Particle *other, buReal sc, buReal rl) {
    ParticleBungee *p = malloc(sizeof(ParticleBungee));
    assert(p);  // Check for allocation failure
    p->_other = other;
    p->_springConstant = sc;
    p->_restLength = rl;
    ((Object *)p)->klass = (Class *)cls;
    return p;
}

// free object
void pbu_free_instance(const ParticleBungeeClass *cls, ParticleBungee *self) {
    free(self);
}

void pbu_updateForce(const ParticleForceGenerator *self, Particle* particle, buReal duration) {
    // Calculate the vector of the spring
    buVector3 force = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition);
    buVector3 otherPosition = INSTANCE_METHOD_AS(ParticleVTable, ((ParticleBungee *)self)->_other, getPosition);
    force = buVector3Difference(force, otherPosition);

    // Check if the bungee is compressed
    buReal magnitude = buVector3Norm(force);
    if (magnitude <= ((ParticleBungee *)self)->_restLength) return;

    // Calculate the magnitude of the force
    magnitude = ((ParticleBungee *)self)->_springConstant * (((ParticleBungee *)self)->_restLength - magnitude);

    // Calculate the final force and apply it
    force = buVector3Normalise(force);
    force = buVector3Scalar(force, -magnitude);
    INSTANCE_METHOD_AS(ParticleVTable, particle, addForce, force);
}

static const char *pbu_get_name(const ParticleBungeeClass *cls) {
    return cls->class_name;
}

static bool pbu_initialized = false;
void ParticleBungeeCreateClass() {
    printf("ParticleBungeeCreateClass:enter\n");
    if (!pbu_initialized) {
        printf("ParticleBungeeCreateClass:initializing\n");
        ParticleForceGeneratorCreateClass();
        pbu_vtable.base = pfg_vtable; // inherit from VTable

        // methods
        pbu_vtable.base.updateForce = pbu_updateForce;

        // init the particle class
        particleBungeeClass.base = particleForceGeneratorClass; // inherit from Class
        particleBungeeClass.base.base.vtable = (VTable *)&pbu_vtable;
        particleBungeeClass.new_instance = pbu_new_instance;
        particleBungeeClass.free = pbu_free_instance;
        particleBungeeClass.class_name = strdup("ParticleBungee");
        particleBungeeClass.get_name = pbu_get_name;

        pbu_initialized = true;
    }
    printf("ParticleBungeeCreateClass:leave\n");
}


///////////////////////////////////////////////////////////////////
// ParticleFakeSpring - applies a fake spring force to a particle
///////////////////////////////////////////////////////////////////
ParticleFakeSpringClass particleFakeSpringClass;
ParticleFakeSpringVTable pfs_vtable;

// new object
static ParticleFakeSpring *pfs_new_instance(
                                    const ParticleFakeSpringClass *cls,
                                    buVector3 anchor, buReal sc, buReal dam) {
    ParticleFakeSpring *p = malloc(sizeof(ParticleFakeSpring));
    assert(p);  // Check for allocation failure
    p->_anchor = anchor;
    p->_springConstant = sc;
    p->_damping = dam;
    ((Object *)p)->klass = (Class *)cls;
    return p;
}

// free object
void pfs_free_instance(const ParticleFakeSpringClass *cls, ParticleFakeSpring *self) {
    free(self);
}

void pfs_updateForce(const ParticleForceGenerator *self, Particle* particle, buReal duration) {
    // Check that we do not have infinite mass
    if(!INSTANCE_METHOD_AS(ParticleVTable, particle, hasFiniteMass)) return;

    // Calculate the relative position of the particle to the anchor
    buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition);
    position = buVector3Difference(position, ((ParticleFakeSpring *)self)->_anchor);

    // Calculate the constants and check they are in bounds.
    buReal gamma = 0.5f * buSqrt(4 * ((ParticleFakeSpring *)self)->_springConstant - ((ParticleFakeSpring *)self)->_damping * ((ParticleFakeSpring *)self)->_damping);
    if (gamma == 0.0f) return;
    buVector3 c = buVector3Add(
        buVector3Scalar(position, ((ParticleFakeSpring *)self)->_damping / (2.0 * gamma)),
        buVector3Scalar(INSTANCE_METHOD_AS(ParticleVTable, particle, getVelocity), 1.0 / gamma));

    // Calculate the target position
    buVector3 target = buVector3Add(
        buVector3Scalar(position, buCos(gamma * duration)),
        buVector3Scalar(c, buSin(gamma * duration)));
    target = buVector3Scalar(target, buExp(-0.5 * duration * ((ParticleFakeSpring *)self)->_damping));

    // Calculate the resulting acceleration and therefore the force
    buVector3 accel = buVector3Difference(
            buVector3Scalar(buVector3Difference(target, position), ((buReal)1.0 / (duration*duration))),
            buVector3Scalar(INSTANCE_METHOD_AS(ParticleVTable, particle, getVelocity), ((buReal)1.0/duration)));

    buVector3 force = buVector3Scalar(accel, INSTANCE_METHOD_AS(ParticleVTable, particle, getMass));
    INSTANCE_METHOD_AS(ParticleVTable, particle, addForce, force);

}

static const char *pfs_get_name(const ParticleFakeSpringClass *cls) {
    return cls->class_name;
}

static bool pfs_initialized = false;
void ParticleFakeSpringCreateClass() {
    printf("ParticleFakeSpringCreateClass:enter\n");
    if (!pfs_initialized) {
        printf("ParticleFakeSpringCreateClass:initializing\n");
        ParticleForceGeneratorCreateClass();
        pfs_vtable.base = pfg_vtable; // inherit from VTable

        // methods
        pfs_vtable.base.updateForce = pfs_updateForce;

        // init the particle class
        particleFakeSpringClass.base = particleForceGeneratorClass; // inherit from Class
        particleFakeSpringClass.base.base.vtable = (VTable *)&pfs_vtable;
        particleFakeSpringClass.new_instance = pfs_new_instance;
        particleFakeSpringClass.free = pfs_free_instance;
        particleFakeSpringClass.class_name = strdup("ParticleFakeSpring");
        particleFakeSpringClass.get_name = pfs_get_name;

        pfs_initialized = true;
    }
    printf("ParticleFakeSpringCreateClass:leave\n");
}

//////////////////////////////////////////////////////////////////
// ParticleForceRegistry
//////////////////////////////////////////////////////////////////
ParticleForceRegistryClass particleForceRegistryClass;
ParticleForceRegistryVTable pfr_vtable;

void pfr_add(ParticleForceRegistry *self, Particle* particle, ParticleForceGenerator *fg) {
    ParticleForceRegistration *registration = malloc(sizeof(ParticleForceRegistration));
    registration->particle = particle;
    registration->fg = fg;
    INSTANCE_METHOD_AS(VectorVTable, self->_registrations, push, registration);
}

void pfr_remove(ParticleForceRegistry *self, Particle* particle, ParticleForceGenerator *fg) {
    size_t size = INSTANCE_METHOD_AS(VectorVTable, self->_registrations, getLength);
    for (size_t i = 0; i < size; i++) {
        ParticleForceRegistration *registration = INSTANCE_METHOD_AS(VectorVTable, self->_registrations, get, i);
        if (registration->particle == particle && registration->fg == fg) {
            INSTANCE_METHOD_AS(VectorVTable, self->_registrations, remove, registration);
            free(registration);
            return;
        }
    }
}

void pfr_clear(ParticleForceRegistry *self) {
    size_t size = INSTANCE_METHOD_AS(VectorVTable, self->_registrations, getLength);
    for (size_t i = 0; i < size; i++) {
        ParticleForceRegistration *registration = INSTANCE_METHOD_AS(VectorVTable, self->_registrations, get, i);
        free(registration);
    }
    INSTANCE_METHOD_AS(VectorVTable, self->_registrations, clear);
}

void pfr_updateForces(ParticleForceRegistry *self, buReal duration) {
    size_t size = INSTANCE_METHOD_AS(VectorVTable, self->_registrations, getLength);
    for (size_t i = 0; i < size; i++) {
        ParticleForceRegistration *registration = INSTANCE_METHOD_AS(VectorVTable, self->_registrations, get, i);
        Particle *particle = registration->particle;
        ParticleForceGenerator *generator = registration->fg;
        INSTANCE_METHOD_AS(ParticleForceGeneratorVTable, generator, updateForce, particle, duration);
    }
}

// free object
void pfr_free_instance(const Class *cls, Object *self) {
    printf("ParticleForceRegistry::free_instance:enter\n");
    free(self);
    printf("ParticleForceRegistry::free_instance:leave\n");
}

// new object
static Object *pfr_new_instance(const Class *cls) {
    ParticleForceRegistry *pfg = malloc(sizeof(ParticleForceRegistry));
    assert(pfg);  // Check for allocation failure
    ((Object *)pfg)->klass = cls;
    pfg->_registrations = (Vector *)CLASS_METHOD(&vectorClass, new_instance);
    return (Object *)pfg;
}

static const char *pfr_get_name(const ParticleForceRegistryClass *cls) {
    return cls->class_name;
}

static bool pfr_initialized = false;
void ParticleForceRegistryCreateClass() {
    printf("ParticleForceRegistryCreateClass:enter\n");
    if (!pfr_initialized) {
        printf("ParticleForceRegistryCreateClass:initializing\n");
        pfr_vtable.base = vTable; // inherit from Class's vtable

        // methods
        pfr_vtable.add = pfr_add;
        pfr_vtable.remove = pfr_remove;
        pfr_vtable.clear = pfr_clear;
        pfr_vtable.updateForces = pfr_updateForces;

        // init the particle class
        particleForceRegistryClass.base = class; // inherit from Class
        particleForceRegistryClass.base.vtable = (VTable *)&pfr_vtable;
        particleForceRegistryClass.base.new_instance = pfr_new_instance;
        particleForceRegistryClass.base.free = pfr_free_instance;
        particleForceRegistryClass.class_name = strdup("Particle");
        particleForceRegistryClass.get_name = pfr_get_name;

        pfr_initialized = true;
    }
    printf("ParticleCreateClass:leave\n");
}



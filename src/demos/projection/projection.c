#include "../../budgie/oop.h"
#include "projection.h"
#include "../../budgie/cparticle.h"
#include "../../budgie/pfgen.h"
#include "../timing.h"
#include <stdio.h>
#include "../../budgie/random.h"
#include <limits.h>
#include <string.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"
#include "../camera.h"

#define DAMPING 1.0
#define PARTICLE_MASS 1.0
#define PARTICLE_SIZE 10.0
#define PARTICLE_COLOR RED

#define INITIAL_SPEED 100

#define TARGET (Vector3){0.0, 0.0, 0.0}
#define CAMERA_DISTANCE 3000.0
#define CAMERA_PITCH 0.5
#define CAMERA_YAW 0.5




bool pause = false;

buReal total_energy = -1.0;

const buVector3 gravity = (buVector3){0.0, -9.81, 0.0};

Particle *particle = NULL;
ParticleForceRegistry *forceRegistry; /** Holds the force registry. */

buVector3 initial_velocity = (buVector3){0.0, 0.0, 0.0};

void setProjectionParameters() {
    buVector3 position = (buVector3){0.0, 0.0, 0.0};
    buVector3 velocity = buVector3Scalar((buVector3){INITIAL_SPEED, INITIAL_SPEED, 0.0}, 1.0 / buSqrt(2.0));
    buVector3 acceleration = (buVector3){0.0, 0.0, 0.0};
    INSTANCE_METHOD_AS(ParticleVTable, particle, set, position, velocity, acceleration, DAMPING, PARTICLE_MASS);
}

void init(Application *self) {
    printf("projection::init:enter\n");
    // Initialize classes
    ParticleCreateClass();
    ParticleForceGeneratorCreateClass();
    ParticleGravityCreateClass();
    ParticleForceRegistryCreateClass();


    particle = (Particle *)CLASS_METHOD(&particleClass, new_instance);
    assert(particle);
    setProjectionParameters();

    

    // Initialize the force registry
    forceRegistry = (ParticleForceRegistry *)CLASS_METHOD(&particleForceRegistryClass, new_instance);
    assert(forceRegistry); // Check for allocation failure

    ParticleForceGenerator *gravityForce = (ParticleForceGenerator *)CLASS_METHOD_AS(ParticleGravityClass, &particleGravityClass, new_instance, gravity);
    assert(gravityForce); // Check for allocation failure
    INSTANCE_METHOD_AS(ParticleForceRegistryVTable, forceRegistry, add, particle, gravityForce);
    
    total_energy = INSTANCE_METHOD_AS(ParticleVTable, particle, getEnergy);

    setTarget(TARGET);
    setCameraDistance(CAMERA_DISTANCE);
    setCameraPitch(CAMERA_PITCH);
    setCameraYaw(CAMERA_YAW);

    printf("projection::init:leave\n");
}

void deinitDemo(Application *self) {
    CLASS_METHOD(&particleClass, free, (Object *)particle);
}


const char* getTitle(Application *self) {
    return "Projection Demo";
}


void update(Application *self, buReal duration) {
    INSTANCE_METHOD_AS(ParticleVTable, particle, clearAccumulator);
    INSTANCE_METHOD_AS(ParticleForceRegistryVTable, forceRegistry, updateForces, duration);

    buReal inverseMass = INSTANCE_METHOD_AS(ParticleVTable, particle, getInverseMass);
    assert(inverseMass > 0.0); // Ensure inverse mass is positive
    buVector3 force = INSTANCE_METHOD_AS(ParticleVTable, particle, getForceAccum);
    INSTANCE_METHOD_AS(ParticleVTable, particle, setAcceleration, buVector3Scalar(force, inverseMass));
    INSTANCE_METHOD_AS(ParticleVTable, particle, integrate, duration);
    // Calculate total energy
    total_energy = INSTANCE_METHOD_AS(ParticleVTable, particle, getEnergy);

    buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition);
    if (position.y < 0.0) {
        setProjectionParameters();
    }
}

void display(Application *self) {
    buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition);
    DrawCube((Vector3){position.x, position.y, position.z}, PARTICLE_SIZE, PARTICLE_SIZE, PARTICLE_SIZE, PARTICLE_COLOR);

}

void keyboard(Application *self, KeyboardKey key) {

}

void display_info(Application *self, size_t Y, size_t d){
    DrawText(TextFormat("Total Energy: %0.2f", total_energy), 20, Y, 30, BLUE);
    DrawText(TextFormat("Gravity: (%.2f, %.2f, %.2f)",
             gravity.x, gravity.y, gravity.z),
             20, Y + d, 30, BLUE);
             
}

static Object *projection_new_instance(const Class *cls) {
    Projection *p = malloc(sizeof(Projection));
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

void projection_free_instance(const Class *cls, Object *self) {
    printf("Projection::free_instance:enter\n");
    free(self);
    printf("Projection::free_instance:leave\n");
}

static const char *get_name(ProjectionClass *cls) {
    return cls->class_name;
}

ProjectionClass projectionClass;
ProjectionVTable projection_vtable;

static bool projection_initialized = false;
void ProjectionCreateClass() {
    printf("ProjectionCreateClass:enter\n");
    if (!projection_initialized) {
        printf("ProjectionCreateClass: initializing\n");
        ApplicationCreateClass();
        ParticleCreateClass();
        projection_vtable.base = application_vtable;

        // override application methods
        projection_vtable.base.init = init;
        projection_vtable.base.deinit = deinitDemo;
        projection_vtable.base.getTitle = getTitle;
        projection_vtable.base.update = update;
        projection_vtable.base.display = display;
        projection_vtable.base.display_info = display_info;
        projection_vtable.base.keyboard = keyboard;

        // init the projection class
        projectionClass.base = applicationClass;
        projectionClass.base.base.vtable = (VTable *)&projection_vtable;
        projectionClass.class_name = strdup("Projection");
        projectionClass.get_name = get_name;
        projectionClass.base.base.new_instance = projection_new_instance;
        projectionClass.base.base.free = projection_free_instance;

        projection_initialized = true;
    }
    printf("ProjectionCreateClass:leave\n");
}

Object *getApplication() {
    printf("getApplication: enter\n");
    ProjectionCreateClass();
    printf("getApplication: leave\n");
    return CLASS_METHOD(&projectionClass, new_instance);
}
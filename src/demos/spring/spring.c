#include "../../budgie/oop.h"
#include "spring.h"
#include "../../budgie/cparticle.h"
#include "../../budgie/pfgen.h"
#include "../timing.h"
#include <stdio.h>
#include "../../budgie/random.h"
#include "../camera.h"
#include <limits.h>
#include <string.h>
#include <assert.h>
#include "raylib.h"
#include "raymath.h"

#define NUMBER_OF_PARTICLES 10
#define REST_LENGTH 10.0
#define ANCHOR (buVector3){0.0, 100.0, 0.0}
#define SPRING_CONSTANT 1.0
#define DAMPING 0.9946
#define PARTICLE_MASS 10.0
#define PARTICLE_SIZE 1.0
#define PARTICLE_COLOR (Color){255, 255, 255, 255}
#define SPRING_COLOR MAROON

#define TARGET (Vector3){0.0, 100.0, 0.0}
#define CAMERA_DISTANCE 300.0
#define CAMERA_PITCH 0.5
#define CAMERA_YAW 0.5

bool pause = false;

buReal total_energy = -1.0;

const buVector3 gravity = (buVector3){0.0, -9.81, 0.0};

Particle *particles[NUMBER_OF_PARTICLES] = {NULL}; /** Holds the particles. */
ParticleForceRegistry *forceRegistry; /** Holds the force registry. */

buReal spring_PE(buVector3 start, buVector3 end) {
    return 0.5 * SPRING_CONSTANT * buPow(buVector3Norm(buVector3Difference(end, start)) - REST_LENGTH, 2);
}

void getEnergy(){
    total_energy = 0.0;
    buVector3 start;
    buVector3 end;
    for (size_t i = 0; i < NUMBER_OF_PARTICLES; i++) {
        Particle *p = particles[i];
        buReal energy = INSTANCE_METHOD_AS(ParticleVTable, p, getEnergy);
        total_energy += energy;
        if(i==0){
            start = INSTANCE_METHOD_AS(ParticleVTable, p, getPosition);
            total_energy += spring_PE(ANCHOR, start);
            continue;
        }
        end = INSTANCE_METHOD_AS(ParticleVTable, p, getPosition);
        total_energy += spring_PE(start, end);
        start = end;
    }
}

void init(Application *self) {
    printf("spring::init:enter\n");
    // Initialize classes
    ParticleCreateClass();
    ParticleForceGeneratorCreateClass();
    ParticleAnchoredSpringCreateClass();
    ParticleSpringCreateClass();
    ParticleGravityCreateClass();
    ParticleForceRegistryCreateClass();


    assert(particles); // Check for allocation failure

    // Initialize particles
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        Particle *p = (Particle *)CLASS_METHOD(&particleClass, new_instance);
        assert(p); // Check for allocation failure
        buVector3 position = buVector3Add((buVector3){0.0, - (i + 1) * REST_LENGTH, 0.0}, ANCHOR);
        buVector3 velocity = i == NUMBER_OF_PARTICLES - 1 ? (buVector3){100.0, 0.0, 0.0} : buRandomVectorByRange(&(buVector3){-1.0, -1.0, -1.0}, &(buVector3){1.0, 1.0, 1.0});
        buVector3 acceleration = {0.0, 0.0, 0.0};
        INSTANCE_METHOD_AS(ParticleVTable, p, set, position, velocity, acceleration, DAMPING, PARTICLE_MASS);
        particles[i] = p; // Store the particle
        INSTANCE_METHOD_AS(ParticleVTable, p, clearAccumulator); // Clear the force accumulator
    }

    // Initialize the force registry
    forceRegistry = (ParticleForceRegistry *)CLASS_METHOD(&particleForceRegistryClass, new_instance);
    assert(forceRegistry); // Check for allocation failure


    // Initialize spring force generators
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        if(i == 0) {
            ParticleForceGenerator *spring = (ParticleForceGenerator *)CLASS_METHOD_AS(ParticleAnchoredSpringClass, &particleAnchoredSpringClass, new_instance, ANCHOR, SPRING_CONSTANT, REST_LENGTH);
            INSTANCE_METHOD_AS(ParticleForceRegistryVTable, forceRegistry, add, particles[i], spring);
            continue;
        }
        Particle *a = particles[i - 1];
        Particle *b = particles[i];
        ParticleForceGenerator *spring_a = (ParticleForceGenerator *)CLASS_METHOD_AS(ParticleSpringClass, &particleSpringClass, new_instance, a, SPRING_CONSTANT, REST_LENGTH);
        ParticleForceGenerator *spring_b = (ParticleForceGenerator *)CLASS_METHOD_AS(ParticleSpringClass, &particleSpringClass, new_instance, b, SPRING_CONSTANT, REST_LENGTH);
        INSTANCE_METHOD_AS(ParticleForceRegistryVTable, forceRegistry, add, a, spring_b);
        INSTANCE_METHOD_AS(ParticleForceRegistryVTable, forceRegistry, add, b, spring_a);
    }

    // Add gravity force generator to all particles
    ParticleForceGenerator *gravityForce = (ParticleForceGenerator *)CLASS_METHOD_AS(ParticleGravityClass, &particleGravityClass, new_instance, gravity);
    assert(gravityForce); // Check for allocation failure
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        INSTANCE_METHOD_AS(ParticleForceRegistryVTable, forceRegistry, add, particles[i], gravityForce);
    }


    // Calculate total energy
    getEnergy();

    // set camera
    setTarget(TARGET);
    setCameraDistance(CAMERA_DISTANCE);
    setCameraPitch(CAMERA_PITCH);
    setCameraYaw(CAMERA_YAW);

    printf("spring::init:leave\n");
}

void deinitDemo(Application *self) {
    for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        Particle *p = particles[i];
        CLASS_METHOD(&particleClass, free, (Object *)p);
    }
}


const char* getTitle(Application *self) {
    return "Spring Demo";
}


void update(Application *self, buReal duration) {
    // clear force registers in particles
    for(size_t i = 0; i < NUMBER_OF_PARTICLES; i++) {
        Particle *particle = particles[i];
        INSTANCE_METHOD_AS(ParticleVTable, particle, clearAccumulator);
    }

    // update forces
    INSTANCE_METHOD_AS(ParticleForceRegistryVTable, forceRegistry, updateForces, duration);

    // do physics
    for(size_t i = 0; i < NUMBER_OF_PARTICLES; i++) {
        Particle *particle = particles[i];
        buReal inverseMass = INSTANCE_METHOD_AS(ParticleVTable, particle, getInverseMass);
        assert(inverseMass > 0.0); // Ensure inverse mass is positive
        buVector3 force = INSTANCE_METHOD_AS(ParticleVTable, particle, getForceAccum);
        INSTANCE_METHOD_AS(ParticleVTable, particle, setAcceleration, buVector3Scalar(force, inverseMass));
        INSTANCE_METHOD_AS(ParticleVTable, particle, integrate, duration);
    }

    // Calculate total energy
    getEnergy();
}



void DrawSpring3D(Vector3 start, Vector3 end, float radius, int coils, Color color) {
    Vector3 axis = Vector3Subtract(end, start);
    float length = Vector3Length(axis);
    Vector3 dir = Vector3Normalize(axis);

    // Build a basis: choose a perpendicular vector
    Vector3 up = (fabs(dir.y) < 0.99f) ? (Vector3){0, 1, 0} : (Vector3){1, 0, 0};
    Vector3 side = Vector3Normalize(Vector3CrossProduct(dir, up));
    Vector3 binormal = Vector3Normalize(Vector3CrossProduct(dir, side));

    int segments = coils * 10;
    float angleStep = 2 * PI * coils / segments;

    for (int i = 0; i < segments; i++) {
        float t1 = (float)i / segments;
        float t2 = (float)(i + 1) / segments;

        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        Vector3 p1 = Vector3Add(start,
            Vector3Add(Vector3Scale(dir, t1 * length),
            Vector3Add(Vector3Scale(side, radius * cosf(angle1)),
                       Vector3Scale(binormal, radius * sinf(angle1)))));

        Vector3 p2 = Vector3Add(start,
            Vector3Add(Vector3Scale(dir, t2 * length),
            Vector3Add(Vector3Scale(side, radius * cosf(angle2)),
                       Vector3Scale(binormal, radius * sinf(angle2)))));

        DrawLine3D(p1, p2, color);
    }
}



void display(Application *self) {

    for (size_t i = 0; i < NUMBER_OF_PARTICLES; i++) {
        Particle *particle = particles[i];
        buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition);
        DrawCube((Vector3){position.x, position.y, position.z}, PARTICLE_SIZE, PARTICLE_SIZE, PARTICLE_SIZE, PARTICLE_COLOR);
    }

    // Draw springs
    for (size_t i = 0; i < NUMBER_OF_PARTICLES; i++) {
        Particle *particle = particles[i];
        if (i == 0) {
            // Draw anchored spring
            buVector3 anchor = ANCHOR;
            buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition);
            DrawSpring3D((Vector3){anchor.x, anchor.y, anchor.z}, (Vector3){position.x, position.y, position.z}, PARTICLE_SIZE, 10, SPRING_COLOR);
            continue;
        }
        Particle *other = particles[i - 1];
        buVector3 otherPosition = INSTANCE_METHOD_AS(ParticleVTable, other, getPosition);
        buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, particle, getPosition);
        DrawSpring3D((Vector3){otherPosition.x, otherPosition.y, otherPosition.z}, (Vector3){position.x, position.y, position.z}, PARTICLE_SIZE, 10, SPRING_COLOR);
    }
}


void keyboard(Application *self, KeyboardKey key) {

}

void display_info(Application *self, size_t Y, size_t d){
    DrawText(TextFormat("Total Energy: %0.2f", total_energy), 20, Y, 30, BLUE);

    DrawText(TextFormat("Number of Particles: %d", NUMBER_OF_PARTICLES), 20, Y + d, 30, BLUE);
    DrawText(TextFormat("Particle Mass: %0.2f", PARTICLE_MASS), 20, Y + 2 * d, 30, BLUE);
    DrawText(TextFormat("Spring Constant: %0.2f", SPRING_CONSTANT), 20, Y + 3 * d, 30, BLUE);
    DrawText(TextFormat("Damping: %0.2f", DAMPING),
             20, Y + 4 * d, 30, BLUE);
    DrawText(TextFormat("Rest Length: %0.2f", REST_LENGTH),
             20, Y + 5 * d, 30, BLUE);
    DrawText(TextFormat("Anchor Position: (%.2f, %.2f, %.2f)",
             ANCHOR.x, ANCHOR.y, ANCHOR.z),
             20, Y + 6 * d, 30, BLUE);
    DrawText(TextFormat("Gravity: (%.2f, %.2f, %.2f)",
             gravity.x, gravity.y, gravity.z),
             20, Y + 7 * d, 30, BLUE);
             
}

static Object *spring_new_instance(const Class *cls) {
    Spring *p = malloc(sizeof(Spring));
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

void spring_free_instance(const Class *cls, Object *self) {
    printf("Spring::free_instance:enter\n");
    free(self);
    printf("Spring::free_instance:leave\n");
}

static const char *get_name(SpringClass *cls) {
    return cls->class_name;
}

SpringClass springClass;
SpringVTable spring_vtable;

static bool spring_initialized = false;
void SpringCreateClass() {
    printf("SpringCreateClass:enter\n");
    if (!spring_initialized) {
        printf("SpringCreateClass: initializing\n");
        ApplicationCreateClass();
        ParticleCreateClass();
        spring_vtable.base = application_vtable;

        // override application methods
        spring_vtable.base.init = init;
        spring_vtable.base.deinit = deinitDemo;
        spring_vtable.base.getTitle = getTitle;
        spring_vtable.base.update = update;
        spring_vtable.base.display = display;
        spring_vtable.base.display_info = display_info;
        spring_vtable.base.keyboard = keyboard;

        // init the spring class
        springClass.base = applicationClass;
        springClass.base.base.vtable = (VTable *)&spring_vtable;
        springClass.class_name = strdup("Spring");
        springClass.get_name = get_name;
        springClass.base.base.new_instance = spring_new_instance;
        springClass.base.base.free = spring_free_instance;

        spring_initialized = true;
    }
    printf("SpringCreateClass:leave\n");
}

Object *getApplication() {
    printf("getApplication: enter\n");
    SpringCreateClass();
    printf("getApplication: leave\n");
    return CLASS_METHOD(&springClass, new_instance);
}
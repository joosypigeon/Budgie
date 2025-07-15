#include "bballistic.h"
#include "../../budgie/oop.h"
#include "../../budgie/cparticle.h"
#include "../timing.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AMMO_ROUNDS 16

typedef enum {
    UNUSED = 0,
    PISTOL,
    ARTILLERY,
    FIREBALL,
    LASER
} ShotType;

/**
 * Holds a single ammunition round record.
 */
typedef struct {
    Particle *particle;
    ShotType type;
    unsigned startTime;
} AmmoRound;

/** Draws the round. */
void render(AmmoRound *ammoRound) {
    buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, ammoRound->particle, getPosition);
    ShotType type = ammoRound->type;
    buReal radius = -1.0;
    Color color = BLACK;
    switch(type) {
        case PISTOL: 
            radius = 1.0;
            color = RED;
            break;
        case ARTILLERY:
            radius = 10.0;
            color = BLUE;
            break;
        case FIREBALL:
            radius = 10.0;
            color = YELLOW;
            break;
        case LASER:
            radius = 1.0;
            color = YELLOW;
            break;
        default:
            radius = 1.0;
            color = BLACK;
            break;
    }
    DrawSphere((Vector3){position.x, position.y, position.z}, radius, color);
}

/**
 * Holds the maximum number of  rounds that can be
 * fired.
 */
static const unsigned ammoRounds = AMMO_ROUNDS;

/** Holds the particle data. */
static AmmoRound ammo[AMMO_ROUNDS];

/** Holds the current shot type. */
ShotType currentShotType;

// Method definitions
void init(Application *self) {
    initTiming();
    // Make all shots unused
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++) {
        shot->type = UNUSED;
        shot->particle = (Particle *)CLASS_METHOD(&particleClass,new_instance);
    }
}

void deinitDemo() {
    deinitTiming();
}

const char* getTitle(Application *self) {
    return ((Object *)self)->klass->class_name;
}

void fire() {
    // Find the first available round.
    AmmoRound *shot;
    for (shot = ammo; shot < ammo+ammoRounds; shot++) {
        if (shot->type == UNUSED) break;
    }
    // If we didn't find a round, then exit - we can't fire.
    if (shot >= ammo+ammoRounds) return;
    // Set the properties of the particle
    switch(currentShotType) {
        case PISTOL:
            INSTANCE_METHOD_AS(ParticleVTable, shot->particle, set, (buVector3){0.0f, 1.5f, 0.0f}, (buVector3){0.0f, 0.0f, 35.0f}, (buVector3){0.0f, -1.0f, 0.0f}, 0.9, 1.0/2.0);
            break;

        case ARTILLERY:
            INSTANCE_METHOD_AS(ParticleVTable, shot->particle, set, (buVector3){0.0f, 1.5f, 0.0f}, (buVector3){0.0f, 30.0f, 40.0f}, (buVector3){0.0f, -20.0f, 0.0f}, 0.99, 1.0/200.0);
            break;

        case FIREBALL:
            INSTANCE_METHOD_AS(ParticleVTable, shot->particle, set, (buVector3){0.0f, 1.5f, 0.0f}, (buVector3){0.0f, 0.0f, 10.0f}, (buVector3){0.0f, 0.6f, 0.0f}, 0.9, 1.0/1.0);
            break;

        case LASER:
            INSTANCE_METHOD_AS(ParticleVTable, shot->particle, set, (buVector3){0.0f, 1.5f, 0.0f}, (buVector3){0.0f, 0.0f, 100.0f}, (buVector3){0.0f, 0.0f, 0.0f}, 0.99, 1.0/0.1);
            break;
    }
    // Set the data common to all particle types
    shot->startTime = getTiming()->lastFrameTimestamp;
    shot->type = currentShotType;
    // Clear the force accumulators
    INSTANCE_METHOD_AS(ParticleVTable, shot->particle, clearAccumulator);
    //clearAccumulator(&shot->particle);
}

void update(Application *self, buReal duration) {
    //updateTiming();

    // Find the duration of the last frame in seconds
    // buReal duration = getTiming()->lastFrameDuration * 0.001f;
    if (duration <= 0.0f) return;

    // Update the physics of each particle in turn
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++) {
        if (shot->type != UNUSED) {
            // Run the physics
            INSTANCE_METHOD_AS(ParticleVTable, shot->particle, integrate, duration);

            // Check if the particle is now invalid
            buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, shot->particle, getPosition);
            if (position.y < 0.0f ||
                shot->startTime+5000 < getTiming()->lastFrameTimestamp ||
                position.z > 200.0f) {
                // We simply set the shot type to be unused, so the
                // memory it occupies can be reused by another shot.
                shot->type = UNUSED;
            }
        }
    }
}

void display() {
    // Render each particle in turn
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++) {
        if (shot->type != UNUSED) {
            render(shot);
        }
    }
}

static char *message = "No Ammo Selected";
void display_info(Application *self, size_t Y, size_t d) {
    // Render the name of the current shot type
    DrawText(TextFormat("Leftclick to fire."), 20, Y , 30, BLUE);
    DrawText(TextFormat(message), 20, Y + d, 30, BLUE);
    DrawText(TextFormat("Ammo types are . . ."), 20, Y + 2*d, 30, BLUE);
    switch(currentShotType) {
        case PISTOL: message = "Current Ammo: Pistol"; break;
        case ARTILLERY: message = "Current Ammo: Artillery"; break;
        case FIREBALL: message = "Current Ammo: Fireball"; break;
        case LASER: message = "Current Ammo: Laser"; break;
        default: message = "No Ammo Selected";
    }
    DrawText(TextFormat("   press 1 for pistol"), 20, Y + 3*d, 30, BLUE);
    DrawText(TextFormat("   press 2 for artillery"), 20, Y + 4*d, 30, BLUE);
    DrawText(TextFormat("   press 3 for fireball"), 20, Y + 5*d, 30, BLUE);
    DrawText(TextFormat("   press 4 for laser"), 20, Y + 6*d, 30, BLUE);
    
}

void keyboard(Application *self, KeyboardKey key) {
    switch(key) {
        case KEY_ONE: currentShotType = PISTOL; break;
        case KEY_TWO: currentShotType = ARTILLERY; break;
        case KEY_THREE: currentShotType = FIREBALL; break;
        case KEY_FOUR: currentShotType = LASER; break;
    }
}

void mouseButtonPressed(Application *self, MouseButton mouseButton) {
    if(mouseButton==MOUSE_BUTTON_LEFT) fire();
}

BallisticVTable ballistic_vtable = {
    .base = {0}, // base VTable initialized to NULLs
};

static Object *ballistic_new_instance(const Class *cls) {
    Ballistic *p = malloc(sizeof(Ballistic));
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

void ballistic_free_instance(const Class *cls, Object *self) {
    free(self);
}


BallisticClass ballisticClass;
BallisticVTable ballistic_vtable;

static bool ballistic_initialized = false;
void BallisticCreateClass() {
    if (!ballistic_initialized) {
        ApplicationCreateClass();
        ballistic_vtable.base = application_vtable;

        // override application methods
        ballistic_vtable.base.init = init;
        ballistic_vtable.base.deinit = deinitDemo;
        ballistic_vtable.base.getTitle = getTitle;
        ballistic_vtable.base.update = update;
        ballistic_vtable.base.display = display;
        ballistic_vtable.base.display_info = display_info;
        ballistic_vtable.base.keyboard = keyboard;
        ballistic_vtable.base.mouseButtonPressed = mouseButtonPressed;

        // init the ballistic class
        ballisticClass.base = applicationClass;
        ballisticClass.base.base.vtable = (VTable *)&ballistic_vtable;
        ballisticClass.base.base.class_name = strdup("Ballistic");
        ballisticClass.base.base.new_instance = ballistic_new_instance;
        ballisticClass.base.base.free = ballistic_free_instance;

        ballistic_initialized = true;
    }
}

Object *getApplication() {
    BallisticCreateClass();
    printf("getApplication: %s\n", ballisticClass.base.base.class_name);
    return CLASS_METHOD(&ballisticClass, new_instance);
}
// ballistic.c
#include "../demo.h"
#include "../../budgie/oop.h"
#include "../../budgie/cparticle.h"
#include "../timing.h"
#include <stdio.h>

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
    buVector3 position = CALL_METHOD(ammoRound->particle, getPosition);
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
void initDemo() {
    initTiming();
    // Make all shots unused
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++) {
        shot->type = UNUSED;
        shot->particle = particleClass.new_instance();
    }
}

void deinitDemo() {
    deinitTiming();
}

const char* getTitle() {
    return "Ballistic Demo";
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
            CALL_METHOD(shot->particle, set, (buVector3){0.0f, 1.5f, 0.0f}, (buVector3){0.0f, 0.0f, 35.0f}, (buVector3){0.0f, -1.0f, 0.0f}, 0.9, 1.0/2.0);
            break;

        case ARTILLERY:
            CALL_METHOD(shot->particle, set, (buVector3){0.0f, 1.5f, 0.0f}, (buVector3){0.0f, 30.0f, 40.0f}, (buVector3){0.0f, -20.0f, 0.0f}, 0.99, 1.0/200.0);
            break;

        case FIREBALL:
            CALL_METHOD(shot->particle, set, (buVector3){0.0f, 1.5f, 0.0f}, (buVector3){0.0f, 0.0f, 10.0f}, (buVector3){0.0f, 0.6f, 0.0f}, 0.9, 1.0/1.0);
            break;

        case LASER:
            CALL_METHOD(shot->particle, set, (buVector3){0.0f, 1.5f, 0.0f}, (buVector3){0.0f, 0.0f, 100.0f}, (buVector3){0.0f, 0.0f, 0.0f}, 0.99, 1.0/0.1);
            break;
    }

    // Set the data common to all particle types
    shot->startTime = getTiming()->lastFrameTimestamp;
    shot->type = currentShotType;

    // Clear the force accumulators
    CALL_METHOD(shot->particle, cclearAccumulator);
    //clearAccumulator(&shot->particle);
}

void updateDemo(buReal duration) {
    //updateTiming();

    // Find the duration of the last frame in seconds
    // buReal duration = getTiming()->lastFrameDuration * 0.001f;
    if (duration <= 0.0f) return;

    // Update the physics of each particle in turn
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++) {
        if (shot->type != UNUSED) {
            // Run the physics
            CALL_METHOD(shot->particle, integrate, duration);

            // Check if the particle is now invalid
            buVector3 position = CALL_METHOD(shot->particle, getPosition);
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

void displayDemo() {
    // Render each particle in turn
    for (AmmoRound *shot = ammo; shot < ammo+ammoRounds; shot++) {
        if (shot->type != UNUSED) {
            render(shot);
        }
    }
}

static char *message = "No Ammo Selected";
void displayInfo(size_t Y, size_t d) {
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

void keyboard(KeyboardKey key) {
    switch(key) {
        case KEY_ONE: currentShotType = PISTOL; break;
        case KEY_TWO: currentShotType = ARTILLERY; break;
        case KEY_THREE: currentShotType = FIREBALL; break;
        case KEY_FOUR: currentShotType = LASER; break;
    }
}

void mouseButtonPressed(MouseButton mouseButton) {
    if(mouseButton==MOUSE_BUTTON_LEFT) fire();
}
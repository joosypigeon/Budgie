#include "../demo.h"
#include "../../budgie/particle.h"
#include "../timing.h"
#include <stdio.h>
#include "../../budgie/random.h"
#include <limits.h>
#include "raylib.h"

#define MAX_FIREWORKS 10240 /** Holds the maximum number of fireworks that can be in use. */
#define RULE_COUNT 9 /** And the number of rules. */

unsigned liveFireworks = 0;

bool pause = false;

typedef struct {
    unsigned type;
    buReal age;
    buVector3Particle particle;
    bool spawn;
} Firework;

Firework fireworks[MAX_FIREWORKS]; /** Holds the firework data. */
unsigned nextFirework = UINT_MAX; /** Holds the index of the next firework slot to use. */

/**
 * The payload is the new firework type to create when this
 * firework's fuse is over.
 */
typedef struct Payload {
    unsigned type; /** The type of the new particle to create. */
    unsigned count; /** The number of particles in this payload. */
} Payload;

/**
 * Firework rules control the length of a firework's fuse and the
 * particles it should evolve into.
 */
typedef struct FireworkRule {
    unsigned type; /** The type of firework that is managed by this rule. */
    buReal minAge; /** The minimum length of the fuse. */
    buReal maxAge; /** The maximum legnth of the fuse. */
    buVector3 minVelocity; /** The minimum relative velocity of this firework. */
    buVector3 maxVelocity; /** The maximum relative velocity of this firework. */
    buReal damping; /** The damping of this firework type. */
    unsigned payloadCount; /** The number of payloads for this firework type. */
    Payload *payloads; /** The set of payloads. */
} FireworkRule;

/** Holds the set of rules. */
FireworkRule rules[RULE_COUNT];

/**
 * Updates the firework by the given duration of time. Returns true
 * if the firework has reached the end of its life and needs to be
 * removed.
 */
bool updateFirework(Firework *firework, buReal duration) {
    //updateTiming();

    // Update our physical state
    buVector3ParticleIntegrate(&firework->particle, duration);

    // We work backwards from our age to zero.
    firework->age -= duration;
    return (firework->age < 0) || (firework->particle.position.y < 0);
}


/**
 * Set all the rule parameters in one go.
 */
void setParameters(FireworkRule *fireworkRule, unsigned type, buReal minAge, buReal maxAge,
    buVector3 minVelocity, buVector3 maxVelocity, buReal damping)
{
    fireworkRule->type = type;
    fireworkRule->minAge = minAge;
    fireworkRule->maxAge = maxAge;
    fireworkRule->minVelocity = minVelocity;
    fireworkRule->maxVelocity = maxVelocity;
    fireworkRule->damping = damping;
    fireworkRule->payloadCount = 0;
    fireworkRule->payloads = NULL;
}

/**
 * Creates a new firework of this type and writes it into the given
 * instance. The optional parent firework is used to base position
 * and velocity on.
 */
void createFireworkFromRule(const FireworkRule *fireworkRule, Firework *child, const Firework *parent) {
    child->type = fireworkRule->type;
    child->age = buRandomReal(fireworkRule->minAge, fireworkRule->maxAge);

    buVector3 velocity = (buVector3){0};
    if (parent) {
        // The position and velocity are based on the parent.
        child->particle.position = parent->particle.position;
        velocity = parent->particle.velocity;
    } else {
        buVector3 start = (buVector3){
            (buReal)(5 * (buRandomInt(3) - 1)),
            (buReal)0.0,
            (buReal)0.0
        };
        child->particle.position = start;
    }
    child->particle.velocity = buVector3Add(velocity, buRandomVectorByRange(&fireworkRule->minVelocity, &fireworkRule->maxVelocity));
    buVector3 p = child->particle.position;
    // We use a mass of one in all cases (no point having fireworks
    // with different masses, since they are only under the influence
    // of gravity).
    child->particle.inverseMass = 1.0;
    child->particle.damping = fireworkRule->damping;
    child->particle.acceleration = GRAVITY;
    clearAccumulator(&child->particle);
}

void initFireworkRules() {
    // Go through the firework types and create their rules.
    setParameters(
        rules,
        1, // type
        0.5f, 1.4f, // age range
        (buVector3){-5, 25, -5}, // min velocity
        (buVector3){5, 100, 5}, // max velocity
        0.1 // damping
        );
    rules[0].payloadCount = 3;
    rules[0].payloads = malloc(3*sizeof(Payload));
    rules[0].payloads[0].type = 2;
    rules[0].payloads[0].count = 1;
    rules[0].payloads[1].type = 2;
    rules[0].payloads[1].count = 1;
    rules[0].payloads[2].type = 2;
    rules[0].payloads[2].count = 1;

    setParameters(
        rules+1,
        2, // type
        0.5f, 1.0f, // age range
        (buVector3){-5, 10, -5}, // min velocity
        (buVector3){5, 20, 5}, // max velocity
        0.8 // damping
        );
    rules[1].payloadCount = 1;
    rules[1].payloads = malloc(1*sizeof(Payload));
    rules[1].payloads[0].type = 4;
    rules[1].payloads[0].count = 2;

    setParameters(
        rules+2,
        3, // type
        0.5f, 1.5f, // age range
        (buVector3){-5, 5, -5}, // min velocity
        (buVector3){5, 15, 5}, // max velocity
        0.1 // damping
        );
    rules[2].payloadCount = 0;
    rules[2].payloads = NULL;

    setParameters(
        rules+3,
        4, // type
        0.25f, 0.5f, // age range
        (buVector3){-20, 5, -5}, // min velocity
        (buVector3){20, 5, 5}, // max velocity
        0.2 // damping
        );
    rules[3].payloadCount = 1;
    rules[3].payloads = malloc(1*sizeof(Payload));
    rules[3].payloads[0].type = 9;
    rules[3].payloads[0].count = 10;

    setParameters(
        rules+4,
        5, // type
        0.5f, 1.0f, // age range
        (buVector3){-20, 2, -5}, // min velocity
        (buVector3){20, 18, 5}, // max velocity
        0.01 // damping
        );
    rules[4].payloadCount = 1;
    rules[4].payloads = malloc(1*sizeof(Payload));
    rules[4].payloads[0].type = 9;
    rules[4].payloads[0].count = 10;

    setParameters(
        rules+5,
        6, // type
        3, 5, // age range
        (buVector3){-5, 5, -5}, // min velocity
        (buVector3){5, 10, 5}, // max velocity
        0.95 // damping
        );
    rules[5].payloadCount = 1;
    rules[5].payloads = malloc(1*sizeof(Payload));
    rules[5].payloads[0].type = 1;
    rules[5].payloads[0].count = 10;;

    setParameters(
        rules+6,
        7, // type
        4, 5, // age range
        (buVector3){-5, 50, -5}, // min velocity
        (buVector3){5, 60, 5}, // max velocity
        0.01 // damping
        );
    rules[6].payloadCount = 1;
    rules[6].payloads = malloc(1*sizeof(Payload));
    rules[6].payloads[0].type = 1;
    rules[6].payloads[0].count = 10;

    setParameters(
        rules+7,
        8, // type
        0.25f, 0.5f, // age range
        (buVector3){-1, 1, -1}, // min velocity
        (buVector3){1, 2, 1}, // max velocity
        0.01 // damping
        );
    rules[7].payloadCount = 0;
    rules[7].payloads = NULL;

    setParameters(
        rules+8,
        9, // type
        3, 5, // age range
        (buVector3){-15, 10, -5}, // min velocity
        (buVector3){15, 15, 5}, // max velocity
        0.95 // damping
        );
    rules[8].payloadCount = 0;
    rules[8].payloads = NULL;
    // ... and so on for other firework types ...
}

void initDemo() {
    // Make all shots unused
    for (Firework *firework = fireworks;
         firework < fireworks+MAX_FIREWORKS;
         firework++) {
        firework->type = 0;
        firework->spawn = false;
    }

    // Create the firework types
    initFireworkRules();

    nextFirework = 0;
}

void deinitDemo(unsigned count) {
    
}


const char* getTitle() {
    return "Fireworks Demo";
}

void createFirework(unsigned type, const Firework *parent) {
    // Get the rule needed to create this firework
    FireworkRule *rule = rules + (type - 1);

    // Create the firework
    createFireworkFromRule(rule, fireworks+nextFirework, parent);

    // Increment the index for the next firework
    nextFirework = (nextFirework + 1) % MAX_FIREWORKS;
}

void createFireworks(unsigned type, unsigned number, const Firework *parent) {
    for (unsigned i = 0; i < number; i++) {
        createFirework(type, parent);
    }
}

void updateDemo(buReal duration) {
    // Find the duration of the last frame in seconds
    //float duration = (float)TimingData::get().lastFrameDuration * 0.001f;
    if (duration <= 0.0f) return;

    if(pause) return;

    #pragma omp parallel for schedule(static)
    for (Firework *firework = fireworks;
         firework < fireworks+MAX_FIREWORKS;
         firework++) {
        // Check if we need to process this firework.
        firework->spawn = false;
        if (firework->type > 0) {
            firework->spawn = updateFirework(firework, duration);
        }
    }

    for (Firework *firework = fireworks;
         firework < fireworks+MAX_FIREWORKS;
         firework++) {
            if(firework->spawn){
                FireworkRule *rule = rules + (firework->type-1);
                for (unsigned i = 0; i < rule->payloadCount; i++) {
                    Payload * payload = rule->payloads + i; 
                    createFireworks(payload->type, payload->count, firework);
                }
                firework->type = 0;
                firework->spawn = false;
            }
    }

    liveFireworks = 0;
    for (Firework *firework = fireworks;
         firework < fireworks+MAX_FIREWORKS;
         firework++) {
            if(firework->type >0) ++liveFireworks;
    }

    liveFireworks = 0;
    for (Firework *firework = fireworks;
         firework < fireworks+MAX_FIREWORKS;
         firework++) {
        if (firework->type > 0) ++liveFireworks;
    }
}

void displayDemo() {
    const static buReal size = 1.0f;
    for (Firework *firework = fireworks;
        firework < fireworks+MAX_FIREWORKS;
        firework++) {
        if (firework->type > 0) { // Check if we need to process this firework.
            Color color = (Color){0};
            switch (firework->type) {
                case 1: color = (Color){255,  0,  0,255}; break;
                case 2: color = (Color){255,128,  0,255}; break;
                case 3: color = (Color){255,255,  0,255}; break;
                case 4: color = (Color){  0,255,  0,255}; break;
                case 5: color = (Color){  0,255,255,255}; break;
                case 6: color = (Color){102,102,255,255}; break;
                case 7: color = (Color){255,  0,255,255}; break;
                case 8: color = (Color){255,255,255,255}; break;
                case 9: color = (Color){255,128,128,255}; break;
            };

            buVector3 position = firework->particle.position;
            DrawCube((Vector3){position.x, position.y, position.z}, size, size, size, color);
            DrawCube((Vector3){position.x, -position.y, position.z}, size, size, size, color);// Render the firework's reflection
        }
    }
}

void keyboard(KeyboardKey key) {

    switch (key) {
        case KEY_ONE: createFireworks(1, 1, NULL); break;
        case KEY_TWO: createFireworks(2, 1, NULL); break;
        case KEY_THREE: createFireworks(3, 1, NULL); break;
        case KEY_FOUR: createFireworks(4, 1, NULL); break;
        case KEY_FIVE: createFireworks(5, 1, NULL); break;
        case KEY_SIX: createFireworks(6, 1, NULL); break;
        case KEY_SEVEN: createFireworks(7, 1, NULL); break;
        case KEY_EIGHT: createFireworks(8, 1, NULL); break;
        case KEY_NINE: createFireworks(9, 1, NULL); break;
        case KEY_SPACE: pause = !pause; break;

    }
}
void mouseButtonPressed(MouseButton mouseButton){}
void displayInfo(size_t Y, size_t d){
    DrawText(TextFormat("live fireworks: %d", liveFireworks), 20, Y, 30, BLUE);
}
#include <stdio.h>
#include <assert.h>
#include "budgie/particle.h"

void buVector3ParticleSet(buVector3Particle *particle, buVector3 position, buVector3 velocity, buVector3 acceleration, buReal damping, buReal inverseMass) {
    buCopyFromTo(position, &particle->position);
    buCopyFromTo(velocity, &particle->velocity);
    buCopyFromTo(acceleration, &particle->acceleration);
    particle->damping = damping;
    particle->inverseMass = inverseMass;
}

void buVector3ParticleIntegrate(buVector3Particle *particle, buReal duration) {
    //printf("buVector3ParticleIntegrate:enter:duration: " REAL_FMT, duration);
    // Skip integration if particle has infinite mass (i.e. inverse mass is zero or negative)
    if (particle->inverseMass <= 0.0f) return;

    // Ensure duration is positive and meaningful
    assert(duration > 0.0);

    // Step 1: Update position using the current velocity (ignoring acceleration contribution)
    particle->position
        = buVector3Add(
            particle->position,
            buVector3Scalar(particle->velocity, duration));

    // Step 2: Update velocity using the current acceleration
    buVector3 velocity
        = buVector3Add(
            particle->velocity,
            buVector3Scalar(particle->acceleration, duration));

    // Step 3: Apply damping to the updated velocity
    particle->velocity = buVector3Scalar(
        velocity,
        buPow(particle->damping, duration));
}

buReal buVector3KineticEnergy(buVector3Particle *particle) {
    assert(particle->inverseMass > 0.0);
    return 0.5/particle->inverseMass*buVector3SquareNorm(particle->velocity);
}


void clearAccumulator(buVector3Particle *particle) {
    
}
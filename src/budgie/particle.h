#ifndef PARTICLE_H
#define PARTICLE_H

#include "precision.h"
#include "core.h"

typedef struct {
    buVector3 position;
    buVector3 velocity;
    buVector3 acceleration;

    buReal damping;
    buReal inverseMass;

} buVector3Particle;


void buVector3ParticleSet(buVector3Particle *particle, buVector3 position, buVector3 velocity, buVector3 acceleration, buReal damping, buReal inverseMass);


buVector3 buVector3ParticleGetPosition(buVector3Particle *particle);
void buVector3ParticleIntegrate( buVector3Particle *particle, buReal duration);
buReal buVector3KineticEnergy(buVector3Particle *particle);
void clearAccumulator(buVector3Particle *particle);
#endif // PARTICLE_H
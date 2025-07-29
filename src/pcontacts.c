#include "budgie/pcontacts.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/////////////////////////////////////////////////////////////
// ParticleContact
/////////////////////////////////////////////////////////////



static buReal pc_calculateSeparatingVelocity(ParticleContact *self) {
    //printf("ParticleContact::calculateSeparatingVelocity:enter\n");
    buVector3 relativeVelocity = INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], getVelocity);
    if (self->_particle[1]) {
        relativeVelocity = buVector3Difference(relativeVelocity, INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], getVelocity));
    }
    //printf("ParticleContact::calculateSeparatingVelocity:relativeVelocity:(%f, %f, %f)\n", relativeVelocity.x, relativeVelocity.y, relativeVelocity.z);
    return buVector3Dot(relativeVelocity, self->_contactNormal);
}

static void pc_resolveVelocity(ParticleContact *self, buReal duration) {
    //printf("ParticleContact::resolveVelocity:enter: duration:%f\n", duration);
    // Find the velocity in the direction of the contact
    buReal separatingVelocity = pc_calculateSeparatingVelocity(self);

    // Check if it needs to be resolved
    if (separatingVelocity > 0)
    {
        // The contact is either separating, or stationary - there's
        // no impulse required.
        return;
    }

    // Calculate the new separating velocity
    buReal newSepVelocity = -separatingVelocity * self->_restitution;

    // Check the velocity build-up due to acceleration only
    buVector3 accCausedVelocity = INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], getAcceleration);
    
    if (self->_particle[1]) {
        accCausedVelocity = buVector3Difference(accCausedVelocity, INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], getAcceleration));
    }
    buReal accCausedSepVelocity = buVector3Dot(accCausedVelocity, self->_contactNormal) * duration;

    // If we've got a closing velocity due to acceleration build-up,
    // remove it from the new separating velocity
    if (accCausedSepVelocity < 0)
    {
        newSepVelocity += self->_restitution * accCausedSepVelocity;

        // Make sure we haven't removed more than was
        // there to remove.
        if (newSepVelocity < 0) newSepVelocity = 0;
    }

    buReal deltaVelocity = newSepVelocity - separatingVelocity;

    // We apply the change in velocity to each object in proportion to
    // their inverse mass (i.e. those with lower inverse mass [higher
    // actual mass] get less change in velocity)..
    buReal totalInverseMass = INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], getInverseMass);
    if (self->_particle[1]) totalInverseMass += INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], getInverseMass);

    // If all particles have infinite mass, then impulses have no effect
    if (totalInverseMass <= 0) return;

    // Calculate the impulse to apply
    buReal impulse = deltaVelocity / totalInverseMass;

    // Find the amount of impulse per unit of inverse mass
    buVector3 impulsePerIMass = buVector3Scalar(self->_contactNormal, impulse);

    // Apply impulses: they are applied in the direction of the contact,
    // and are proportional to the inverse mass.
    buVector3 velocity = buVector3Add(
        INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], getVelocity),
        buVector3Scalar(
            impulsePerIMass,
            INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], getInverseMass)));
    INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], setVelocity, velocity);

    if (self->_particle[1])
    {
        // Particle 1 goes in the opposite direction
        velocity = buVector3Add(
                INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], getVelocity),
                buVector3Scalar(
                    impulsePerIMass,
                    -INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], getInverseMass)));
        INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], setVelocity, velocity);
    }
    //printf("ParticleContact::resolveVelocity:leave\n");
}

static void pc_resolveInterpenetration(ParticleContact *self, buReal duration) {
    //printf("ParticleContact::resolveInterpenetration:enter: duration:%f\n", duration);
    // If we don't have any penetration, skip this step.
    if (self->_penetration <= 0) return;

    // The movement of each object is based on their inverse mass, so
    // total that.
    buReal totalInverseMass = INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], getInverseMass);
    //printf("ParticleContact::resolveInterpenetration:totalInverseMass:%f\n", totalInverseMass);
    if (self->_particle[1]) {
        totalInverseMass += INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], getInverseMass);
    }
    //printf("ParticleContact::resolveInterpenetration:totalInverseMass:%f\n", totalInverseMass);
    // If all particles have infinite mass, then we do nothing
    if (totalInverseMass <= 0) return;

    // Find the amount of penetration resolution per unit of inverse mass
    //printf("ParticleContact::resolveInterpenetration:penetration:%f\n", self->_penetration);
    //printf("ParticleContact::resolveInterpenetration:contactNormal:(%f, %f, %f)\n", self->_contactNormal.x, self->_contactNormal.y, self->_contactNormal.z);
    //printf("ParticleContact::resolveInterpenetration:totalInverseMass:%f\n", totalInverseMass);
    buVector3 movePerIMass = buVector3Scalar(self->_contactNormal, (self->_penetration / totalInverseMass));
    //printf("ParticleContact::resolveInterpenetration:movePerIMass:(%f, %f, %f)\n", movePerIMass.x, movePerIMass.y, movePerIMass.z);
    // Calculate the the movement amounts
    self->_particleMovement[0] = buVector3Scalar(movePerIMass, INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], getInverseMass));
    //printf("ParticleContact::resolveInterpenetration:particleMovement[0]:(%f, %f, %f)\n", self->_particleMovement[0].x, self->_particleMovement[0].y, self->_particleMovement[0].z);
    if (self->_particle[1]) {
        self->_particleMovement[1] = buVector3Scalar(movePerIMass, -INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], getInverseMass));
    }

    // Apply the penetration resolution
    buVector3 position = buVector3Add(INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], getPosition), self->_particleMovement[0]);
    //printf("ParticleContact::resolveInterpenetration:particle[0] position:(%f, %f, %f)\n", position.x, position.y, position.z);
    INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], setPosition, position);
    if (self->_particle[1]) {
        position = buVector3Add(INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], getPosition), self->_particleMovement[1]);
        INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], setPosition, position);
    }
    //printf("ParticleContact::resolveInterpenetration:leave\n");
}

static void pc_resolve(ParticleContact *self, buReal duration) {
    //printf("ParticleContact::resolve:enter: duration:%f\n", duration);
    pc_resolveVelocity(self, duration);
    pc_resolveInterpenetration(self, duration);
    //printf("ParticleContact::resolve:leave\n");
}

ParticleContactClass particleContactClass;
ParticleContactVTable pc_vtable;

// free object
void pc_free_instance(const Class *cls, Object *self) {
    printf("ParticleContact:free_instance:enter\n");
    free(self);
    printf("ParticleContact::free_instance:leave\n");
}

// new object
static Object *pc_new_instance(const Class *cls) {
    //printf("ParticleContact::new_instance:enter\n");
    ParticleContact *p = malloc(sizeof(ParticleContact));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    //printf("ParticleContact::new_instance:leave\n");
    return (Object *)p;
}

static const char *pc_get_name(ParticleContactClass *cls) {
    return cls->class_name;
}

static bool pc_initialized = false;
void ParticleContactCreateClass() {
    printf("ParticleContactCreateClass:enter\n");
    if (!pc_initialized) {
        printf("ParticleContactCreateClass:initializing\n");
        pc_vtable.base = vTable; // inherit from VTable

        // methods
        pc_vtable.resolve = pc_resolve;
        pc_vtable.calculateSeparatingVelocity = pc_calculateSeparatingVelocity;


        // init the particle class
        particleContactClass.base = class; // inherit from Class
        particleContactClass.base.vtable = (VTable *)&pc_vtable;
        particleContactClass.base.new_instance = pc_new_instance;
        particleContactClass.base.free = pc_free_instance;
        particleContactClass.class_name = strdup("ParticleContact");
        particleContactClass.get_name = pc_get_name;

        pc_initialized = true;
    }
    printf("ParticleContactCreateClass:leave\n");
}



/////////////////////////////////////////////////////////////
// ParticleContactResolver
/////////////////////////////////////////////////////////////


static void pcr_setIterations(ParticleContactResolver *self,  unsigned iterations) {
    self->_iterations = iterations;
}

static void pcr_resolveContacts(
        ParticleContactResolver *self,  
        ParticleContact **contactArray,
        unsigned numContacts,
        buReal duration) {
    //printf("ParticleContactResolver::resolveContacts:enter: numContacts:%u duration:%f\n", numContacts, duration);
    unsigned i;

    self->_iterationsUsed = 0;
    while(self->_iterationsUsed < self->_iterations) {
        // Find the contact with the largest closing velocity;
        buReal max = REAL_MAX;
        unsigned maxIndex = numContacts;
        for (i = 0; i < numContacts; i++) {
            ParticleContact *contact = contactArray[i];
            buReal sepVel = INSTANCE_METHOD_AS(ParticleContactVTable, contact, calculateSeparatingVelocity);
            if (sepVel < max &&
                (sepVel < 0 || contactArray[i]->_penetration > 0)) {
                max = sepVel;
                maxIndex = i;
            }
        }

        // Do we have anything worth resolving?
        if (maxIndex == numContacts) break;

        // Resolve this contact
        INSTANCE_METHOD_AS(ParticleContactVTable, contactArray[maxIndex], resolve, duration);

        // Update the interpenetrations for all particles
        buVector3 *move = contactArray[maxIndex]->_particleMovement;
        for (i = 0; i < numContacts; i++) {
            if (contactArray[i]->_particle[0] == contactArray[maxIndex]->_particle[0]) {
                contactArray[i]->_penetration -= buVector3Dot(move[0], contactArray[i]->_contactNormal);
            } else if (contactArray[i]->_particle[0] == contactArray[maxIndex]->_particle[1]) {
                contactArray[i]->_penetration -= buVector3Dot(move[1], contactArray[i]->_contactNormal);
            }
            if (contactArray[i]->_particle[1]) {
                if (contactArray[i]->_particle[1] == contactArray[maxIndex]->_particle[0]) {
                    contactArray[i]->_penetration += buVector3Dot(move[0], contactArray[i]->_contactNormal);
                } else if (contactArray[i]->_particle[1] == contactArray[maxIndex]->_particle[1]) {
                    contactArray[i]->_penetration += buVector3Dot(move[1], contactArray[i]->_contactNormal);
                }
            }
        }

        self->_iterationsUsed++;
    }
    //printf("ParticleContactResolver::resolveContacts:leave\n");
}

ParticleContactResolverClass particleContactResolverClass;
ParticleContactResolverVTable pcr_vtable;

// free object
void pcr_free_instance(const Class *cls, Object *self) {
    printf("ParticleContactResolver::free_instance:enter\n");
    free(self);
    printf("ParticleContactResolver::free_instance:leave\n");
}

// new object
static Object *pcr_new_instance(const Class *cls) {
    ParticleContactResolver *p = malloc(sizeof(ParticleContactResolver));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *pcr_get_name(ParticleContactResolverClass *cls) {
    return cls->class_name;
}

static bool pcr_initialized = false;
void ParticleContactResolverCreateClass() {
    printf("ParticleContactResolverCreateClass:enter\n");
    if (!pcr_initialized) {
        printf("ParticleContactResolverCreateClass:initializing\n");
        ParticleContactCreateClass();
        pcr_vtable.base = vTable; // inherit from VTable

        // methods
        pcr_vtable.setIterations = pcr_setIterations;
        pcr_vtable.resolveContacts = pcr_resolveContacts;


        // init the particle class
        particleContactResolverClass.base = class; // inherit from Class
        particleContactResolverClass.base.vtable = (VTable *)&pcr_vtable;
        particleContactResolverClass.base.new_instance = pcr_new_instance;
        particleContactResolverClass.base.free = pcr_free_instance;
        particleContactResolverClass.class_name = strdup("ParticleContactResolver");
        particleContactResolverClass.get_name = pcr_get_name;

        pcr_initialized = true;
    }
    printf("ParticleContactResolverCreateClass:leave\n");
}

/////////////////////////////////////////////////////////////
// ParticleContactGenerator
/////////////////////////////////////////////////////////////

unsigned addContact(ParticleContactGenerator *self, ParticleContact *contact, unsigned limit) {
    assert(false && "this should never be called directly, use the derived classes instead");
    return 0;
}

ParticleContactGeneratorClass particleContactGeneratorClass;
ParticleContactGeneratorVTable pcg_vtable;

// free object
void pcg_free_instance(const Class *cls, Object *self) {
    printf("ParticleContactGenerator::free_instance:enter\n");
    free(self);
    printf("ParticleContactGenerator::free_instance:leave\n");
}

// new object
static Object *pcg_new_instance(const Class *cls) {
    ParticleContactGenerator *p = malloc(sizeof(ParticleContactGenerator));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *pcg_get_name(ParticleContactGeneratorClass *cls) {
    return cls->class_name;
}

static bool pcg_initialized = false;
void ParticleContactGeneratorCreateClass() {
    printf("ParticleContactGeneratorCreateClass:enter\n");
    if (!pcg_initialized) {
        printf("ParticleContactGeneratorCreateClass:initializing\n");
        pcg_vtable.base = vTable; // inherit from VTable

        // methods
        pcg_vtable.addContact = addContact;

        // init the particle class
        particleContactGeneratorClass.base = class; // inherit from Class
        particleContactGeneratorClass.base.vtable = (VTable *)&pcg_vtable;
        particleContactGeneratorClass.base.new_instance = pcg_new_instance;
        particleContactGeneratorClass.base.free = pcg_free_instance;
        particleContactGeneratorClass.class_name = strdup("ParticleContactGenerator");
        particleContactGeneratorClass.get_name = pcg_get_name;

        pcg_initialized = true;
    }
    printf("ParticleContactGeneratorCreateClass:leave\n");
}
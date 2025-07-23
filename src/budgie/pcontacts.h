#ifndef PCONTACTS_H
#define PCONTACTS_H

#include "oop.h"
#include "core.h"
#include "cparticle.h"

/**
 * A Contact represents two objects in contact (in this case
 * ParticleContact representing two Particles). Resolving a
 * contact removes their interpenetration, and applies sufficient
 * impulse to keep them apart. Colliding bodies may also rebound.
 *
 * The contact has no callable functions, it just holds the
 * contact details. To resolve a set of contacts, use the particle
 * contact resolver class.
 */
typedef struct ParticleContact ParticleContact;
typedef struct ParticleContactClass ParticleContactClass;
typedef struct ParticleContactVTable ParticleContactVTable;

typedef struct ParticleContact {
    Object base;

    /**
     * Holds the particles that are involved in the contact. The
     * second of these can be NULL, for contacts with the scenery.
     */
    Particle* _particle[2];

    /**
     * Holds the normal restitution coefficient at the contact.
     */
    buReal _restitution;

    /**
     * Holds the direction of the contact in world coordinates.
     */
    buVector3 _contactNormal;

    /**
     * Holds the depth of penetration at the contact.
     */
    buReal _penetration;

    /**
     * Holds the amount each particle is moved by during interpenetration
     * resolution.
     */
    buVector3 _particleMovement[2];
} ParticleContact;

typedef struct ParticleContactVTable {
    VTable base;

    /**
     * Resolves this contact, for both velocity and interpenetration.
     */
    void (*resolve)(ParticleContact *self, buReal duration);

    /**
     * Calculates the separating velocity at this contact.
     */
    buReal (*calculateSeparatingVelocity)(ParticleContact *self);
} ParticleContactVTable;

typedef struct ParticleContactClass {
    Class base; // inherit from Class
    
    const char *class_name; // class name
    const char *(*get_name)(ParticleContactClass *cls);
} ParticleContactClass;

extern ParticleContactClass particleContactClass;
void ParticleContactCreateClass();

/**
 * The contact resolution routine for particle contacts. One
 * resolver instance can be shared for the whole simulation.
 */
typedef struct ParticleContactResolver ParticleContactResolver;
typedef struct ParticleContactResolverClass ParticleContactResolverClass;
typedef struct ParticleContactResolverVTable ParticleContactResolverVTable;

typedef struct ParticleContactResolver {
    Object base;

    /**
     * Holds the number of iterations allowed.
     */
    unsigned _iterations;

    /**
     * This is a performance tracking value - we keep a record
     * of the actual number of iterations used.
     */
    unsigned _iterationsUsed;
} ParticleContactResolver;

typedef struct ParticleContactResolverVTable {
    VTable base;

    /**
     * Sets the number of iterations that can be used.
     */
    void (*setIterations)(unsigned iterations);

    /**
     * Resolves a set of particle contacts for both penetration
     * and velocity.
     *
     * Contacts that cannot interact with each other should be
     * passed to separate calls to resolveContacts, as the
     * resolution algorithm takes much longer for lots of contacts
     * than it does for the same number of contacts in small sets.
     *
     * @param contactArray Pointer to an array of particle contact
     * objects.
     *
     * @param numContacts The number of contacts in the array to
     * resolve.
     *
     * @param numIterations The number of iterations through the
     * resolution algorithm. This should be at least the number of
     * contacts (otherwise some constraints will not be resolved -
     * although sometimes this is not noticable). If the
     * iterations are not needed they will not be used, so adding
     * more iterations may not make any difference. But in some
     * cases you would need millions of iterations. Think about
     * the number of iterations as a bound: if you specify a large
     * number, sometimes the algorithm WILL use it, and you may
     * drop frames.
     *
     * @param duration The duration of the previous integration step.
     * This is used to compensate for forces applied.
    */
    void (*resolveContacts)(ParticleContact *contactArray,
        unsigned numContacts,
        buReal duration);

} ParticleContactResolverVTable;

typedef struct ParticleContactResolverClass {
    Class base; // inherit from Class
    
    const char *class_name; // class name
    const char *(*get_name)(ParticleContactResolverClass *cls);
} ParticleContactResolverClass;


extern ParticleContactResolverClass particleContactResolverClass;
void ParticleContactResolverCreateClass();







typedef struct ParticleContactGenerator ParticleContactGenerator;
typedef struct ParticleContactGeneratorClass ParticleContactGeneratorClass;
typedef struct ParticleContactGeneratorVTable ParticleContactGeneratorVTable;


/**
 * This is the basic polymorphic interface for contact generators
 * applying to particles.
 */
typedef struct ParticleContactGenerator {
    Object base;
} ParticleContactGenerator;

typedef struct ParticleContactGeneratorVTable {
    /**
     * Fills the given contact structure with the generated
     * contact. The contact pointer should point to the first
     * available contact in a contact array, where limit is the
     * maximum number of contacts in the array that can be written
     * to. The method returns the number of contacts that have
     * been written.
     */
    unsigned (*addContact)(ParticleContact *contact,
                                unsigned limit);
} ParticleContactGeneratorVTable;



extern ParticleContactGeneratorClass particleContactGeneratorClass;
void ParticleContactGeneratorCreateClass();

#endif // CONTACTS_H

#ifndef PLINKS_H
#define PLINKS_H


#include "pcontacts.h"


/**
 * Links connect two particles together, generating a contact if
 * they violate the constraints of their link. It is used as a
 * base class for cables and rods, and could be used as a base
 * class for springs with a limit to their extension..
 */
typedef struct ParticleLink ParticleLink;
typedef struct ParticleLinkClass ParticleLinkClass;
typedef struct ParticleLinkVTable ParticleLinkVTable;

typedef struct ParticleLink {
    ParticleContactGenerator base;

    /**
     * Holds the pair of particles that are connected by this link.
     */
    Particle* _particle[2];

} ParticleLink;

typedef struct ParticleLinkVTable {
    ParticleContactGeneratorVTable base;

    /**
     * Returns the current length of the link.
     */
    buReal (*currentLength)(ParticleLink *self);
} ParticleLinkVTable;

typedef struct ParticleLinkClass {
    ParticleContactGeneratorClass base; // inherit from Class
    
    const char *class_name; // class name
    const char *(*get_name)(ParticleLinkClass *cls);
} ParticleLinkClass;

extern ParticleLinkClass particleLinkClass;
void ParticleLinkCreateClass();

/**
 * Cables link a pair of particles, generating a contact if they
 * stray too far apart.
 */
typedef struct ParticleCable ParticleCable; 
typedef struct ParticleCableClass ParticleCableClass;
typedef struct ParticleCableVTable ParticleCableVTable;

typedef struct ParticleCable {
    ParticleLink base;

    /**
     * Holds the maximum length of the cable.
     */
    buReal _maxLength;

    /**
     * Holds the restitution (bounciness) of the cable.
     */
    buReal _restitution;
} ParticleCable;


typedef struct ParticleCableVTable {
    ParticleLinkVTable base;
} ParticleCableVTable;

typedef struct ParticleCableClass {
    ParticleLinkClass base; // inherit from Class
    
    const char *class_name; // class name
    const char *(*get_name)(ParticleCableClass *cls);
} ParticleCableClass;

extern ParticleCableClass particleCableClass;
void ParticleCableCreateClass();

/**
 * Rods link a pair of particles, generating a contact if they
 * stray too far apart or too close.
 */
typedef struct ParticleRod ParticleRod;
typedef struct ParticleRodClass ParticleRodClass;
typedef struct ParticleRodVTable ParticleRodVTable;

typedef struct ParticleRod {
    ParticleLink base;

    /**
     * Holds the length of the rod.
     */
    buReal _length;
} ParticleRod;


typedef struct ParticleRodVTable {
    ParticleLinkVTable base;
} ParticleRodVTable;

typedef struct ParticleRodClass {
    ParticleLinkClass base; // inherit from Class
    
    const char *class_name; // class name
    const char *(*get_name)(ParticleRodClass *cls);
} ParticleRodClass;

extern ParticleRodClass particleRodClass;
void ParticleRodCreateClass();




/**
* Constraints are just like links, except they connect a particle to
* an immovable anchor point.
*/
typedef struct ParticleConstraint ParticleConstraint;
typedef struct ParticleConstraintClass ParticleConstraintClass;
typedef struct ParticleConstraintVTable ParticleConstraintVTable;

typedef struct ParticleConstraint {
    ParticleContactGenerator base;

    /**
    * Holds the particles connected by this constraint.
    */
    Particle* _particle;

    /**
     * The point to which the particle is anchored.
     */
    buVector3 _anchor;

} ParticleConstraint;

typedef struct ParticleConstraintVTable {
    ParticleContactGeneratorVTable base;

    /**
     * Returns the current length of the link.
     */
    buReal (*currentLength)(ParticleConstraint *self);
} ParticleConstraintVTable;

typedef struct ParticleConstraintClass {
    ParticleContactGeneratorClass base; // inherit from Class
    
    const char *class_name; // class name
    const char *(*get_name)(ParticleConstraintClass *cls);
} ParticleConstraintClass;

extern ParticleConstraintClass particleConstraintClass;
void ParticleConstraintCreateClass();



/**
* Cables link a particle to an anchor point, generating a contact if they
* stray too far apart.
*/
typedef struct ParticleCableConstraint ParticleCableConstraint; 
typedef struct ParticleCableConstraintClass ParticleCableConstraintClass;
typedef struct ParticleCableConstraintVTable ParticleCableConstraintVTable;

typedef struct ParticleCableConstraint {
    ParticleConstraint base;

    /**
     * Holds the maximum length of the cable.
     */
    buReal _maxLength;

    /**
     * Holds the restitution (bounciness) of the cable.
     */
    buReal _restitution;
} ParticleCableConstraint;


typedef struct ParticleCableConstraintVTable {
    ParticleConstraintVTable base;
} ParticleCableConstraintVTable;

typedef struct ParticleCableConstraintClass {
    ParticleConstraintClass base; // inherit from Class

    const char *class_name; // class name
    const char *(*get_name)(ParticleCableConstraintClass *cls);
} ParticleCableConstraintClass;

extern ParticleCableConstraintClass particleCableConstraintClass;
void ParticleCableConstraintCreateClass();


/**
* Rods link a particle to an anchor point, generating a contact if they
* stray too far apart or too close.
*/
typedef struct ParticleRodConstraint ParticleRodConstraint;
typedef struct ParticleRodConstraintClass ParticleRodConstraintClass;
typedef struct ParticleRodConstraintVTable ParticleRodConstraintVTable;

typedef struct ParticleRodConstraint {
    ParticleConstraint base;

    /**
     * Holds the length of the rod.
     */
    buReal _length;
} ParticleRodConstraint;


typedef struct ParticleRodConstraintVTable {
    ParticleConstraintVTable base;
} ParticleRodConstraintVTable;

typedef struct ParticleRodConstraintClass {
    ParticleConstraintClass base; // inherit from Class

    const char *class_name; // class name
    const char *(*get_name)(ParticleRodConstraintClass *cls);
} ParticleRodConstraintClass;

extern ParticleRodConstraintClass particleRodConstraintClass;
void ParticleRodConstraintCreateClass();
#endif // PLINKS_H
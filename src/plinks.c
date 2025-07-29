
#include "budgie/plinks.h"
#include <stddef.h>

//////////////////////////////////////////////////////////////////
// ParticleLink
//////////////////////////////////////////////////////////////////

buReal pl_currentLength(ParticleLink *self) {
    buVector3 relativePos = buVector3Difference(
        INSTANCE_METHOD_AS(ParticleVTable, self->_particle[0], getPosition),
        INSTANCE_METHOD_AS(ParticleVTable, self->_particle[1], getPosition)
    );

    return buVector3Norm(relativePos);
}

ParticleLinkClass particleLinkClass;
ParticleLinkVTable pl_vtable;

// free object
void pl_free_instance(const Class *cls, Object *self) {
    printf("ParticleLink::free_instance:enter\n");
    free(self);
    printf("ParticleLink::free_instance:leave\n");
}

// new object
static Object *pl_new_instance(const Class *cls) {
    ParticleLink *p = malloc(sizeof(ParticleLink));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *pl_get_name(ParticleLinkClass *cls) {
    return cls->class_name;
}

static bool particleLink_initialized = false;
void ParticleLinkCreateClass() {
    printf("ParticleLinkCreateClass:enter\n");
    if (!particleLink_initialized) {
        printf("ParticleLinkCreateClass:initializing\n");
        ParticleContactGeneratorCreateClass();
        pl_vtable.base = pcg_vtable; // inherit from VTable

        // methods
        pl_vtable.currentLength = pl_currentLength,

        // init the particle class
        particleLinkClass.base = particleContactGeneratorClass; // inherit from Class

        particleLinkClass.base.base.vtable = (VTable *)&pl_vtable;
        particleLinkClass.base.base.new_instance = pl_new_instance;
        particleLinkClass.base.base.free = pl_free_instance;
        particleLinkClass.class_name = strdup("ParticleLink");
        particleLinkClass.get_name = pl_get_name;

        particleLink_initialized = true;
    }
    printf("ParticleLinkCreateClass:leave\n");
}


//////////////////////////////////////////////////////////////////
// ParticleCable
//////////////////////////////////////////////////////////////////
unsigned pc_addContact(ParticleContactGenerator *self, ParticleContact *contact,
                                    unsigned limit){
    // Find the length of the cable
    buReal length = pl_currentLength((ParticleLink *)self);

    // Check if we're over-extended
    if (length < ((ParticleCable *)self)->_maxLength) {
        return 0;
    }

    // Otherwise return the contact
    contact->_particle[0] = ((ParticleLink *)self)->_particle[0];
    contact->_particle[1] = ((ParticleLink *)self)->_particle[1];

    // Calculate the normal
    buVector3 normal = buVector3Difference(
        INSTANCE_METHOD_AS(ParticleVTable, contact->_particle[1], getPosition),
        INSTANCE_METHOD_AS(ParticleVTable, contact->_particle[0], getPosition)
    );

    normal = buVector3Normalise(normal);
    contact->_contactNormal = normal;

    contact->_penetration = length-((ParticleCable *)self)->_maxLength;
    contact->_restitution = ((ParticleCable *)self)->_restitution;

    return 1;
}

ParticleCableClass particleCableClass;
ParticleCableVTable pc_vtable;

// free object
void pc_free_instance(const Class *cls, Object *self) {
    printf("ParticleCable::free_instance:enter\n");
    free(self);
    printf("ParticleCable::free_instance:leave\n");
}

// new object
static Object *pc_new_instance(const Class *cls) {
    ParticleCable *p = malloc(sizeof(ParticleCable));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *pc_get_name(ParticleCableClass *cls) {
    return cls->class_name;
}

static bool particleCable_initialized = false;
void ParticleCableCreateClass() {
    printf("ParticleCableCreateClass:enter\n");
    if (!particleCable_initialized) {
        printf("ParticleCableCreateClass:initializing\n");
        ParticleLinkCreateClass();
        pc_vtable.base = pl_vtable; // inherit from VTable

        // methods
        pc_vtable.base.base.addContact = pc_addContact;

        // init the particle class
        particleCableClass.base = particleLinkClass; // inherit from Class

        particleCableClass.base.base.base.vtable = (VTable *)&pc_vtable;
        particleCableClass.base.base.base.new_instance = pc_new_instance;
        particleCableClass.base.base.base.free = pc_free_instance;
        particleCableClass.class_name = strdup("ParticleCable");
        particleCableClass.get_name = pc_get_name;

        particleCable_initialized = true;
    }
    printf("ParticleCableCreateClass:leave\n");
}




//////////////////////////////////////////////////////////////////
// ParticleRod
//////////////////////////////////////////////////////////////////
unsigned pr_addContact(ParticleContactGenerator *self, ParticleContact *contact,
                                  unsigned limit) {
    // Find the length of the rod
    buReal currentLen = pl_currentLength((ParticleLink *)self);

    // Check if we're over-extended
    if (currentLen == ((ParticleRod *)self)->_length){
        return 0;
    }

    // Otherwise return the contact
    contact->_particle[0] = ((ParticleLink *)self)->_particle[0];
    contact->_particle[1] = ((ParticleLink *)self)->_particle[1];

    // Calculate the normal
    buVector3 normal = buVector3Difference(
        INSTANCE_METHOD_AS(ParticleVTable, contact->_particle[1], getPosition),
        INSTANCE_METHOD_AS(ParticleVTable, contact->_particle[0], getPosition)
    );
    normal = buVector3Normalise(normal);

    // The contact normal depends on whether we're extending or compressing
    if (currentLen > ((ParticleRod *)self)->_length) {
        contact->_contactNormal = normal;
        contact->_penetration = currentLen - ((ParticleRod *)self)->_length;
    } else {
        contact->_contactNormal = buVector3Scalar(normal, -1.0);
        contact->_penetration = ((ParticleRod *)self)->_length - currentLen;
    }

    // Always use zero restitution (no bounciness)
    contact->_restitution = 0;

    return 1;
}

ParticleRodClass particleRodClass;
ParticleRodVTable pr_vtable;

// free object
void pr_free_instance(const Class *cls, Object *self) {
    printf("ParticleRod::free_instance:enter\n");
    free(self);
    printf("ParticleRod::free_instance:leave\n");
}

// new object
static Object *pr_new_instance(const Class *cls) {
    ParticleRod *p = malloc(sizeof(ParticleRod));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *pr_get_name(ParticleRodClass *cls) {
    return cls->class_name;
}

static bool particleRod_initialized = false;
void ParticleRodCreateClass() {
    printf("ParticleRodCreateClass:enter\n");
    if (!particleRod_initialized) {
        printf("ParticleRodCreateClass:initializing\n");
        ParticleLinkCreateClass();
        pr_vtable.base = pl_vtable; // inherit from VTable

        // methods
        pr_vtable.base.base.addContact = pr_addContact;

        // init the particle class
        particleRodClass.base = particleLinkClass; // inherit from Class

        particleRodClass.base.base.base.vtable = (VTable *)&pr_vtable;
        particleRodClass.base.base.base.new_instance = pr_new_instance;
        particleRodClass.base.base.base.free = pr_free_instance;
        particleRodClass.class_name = strdup("ParticleRod");
        particleRodClass.get_name = pr_get_name;

        particleRod_initialized = true;
    }
    printf("ParticleRodCreateClass:leave\n");
}




//////////////////////////////////////////////////////////////////
// ParticleConstraint
//////////////////////////////////////////////////////////////////
buReal pcc_currentLength(ParticleConstraint *self) {
    buVector3 relativePos = buVector3Difference(
        INSTANCE_METHOD_AS(ParticleVTable, self->_particle, getPosition),
        self->_anchor
    );
    return buVector3Norm(relativePos);
}

ParticleConstraintClass particleConstraintClass;
ParticleConstraintVTable pcc_vtable;

// free object
void pcc_free_instance(const Class *cls, Object *self) {
    printf("ParticleConstraint::free_instance:enter\n");
    free(self);
    printf("ParticleConstraint::free_instance:leave\n");
}

// new object
static Object *pcc_new_instance(const Class *cls) {
    ParticleConstraint *p = malloc(sizeof(ParticleConstraint));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *pcc_get_name(ParticleConstraintClass *cls) {
    return cls->class_name;
}

static bool particleConstraint_initialized = false;
void ParticleConstraintCreateClass() {
    printf("ParticleConstraintCreateClass:enter\n");
    if (!particleConstraint_initialized) {
        printf("ParticleConstraintCreateClass:initializing\n");
        ParticleContactGeneratorCreateClass();
        pcc_vtable.base = pcg_vtable; // inherit from VTable

        // methods
        pcc_vtable.currentLength = pcc_currentLength,

        // init the particle class
        particleConstraintClass.base = particleContactGeneratorClass; // inherit from Class

        particleConstraintClass.base.base.vtable = (VTable *)&pcc_vtable;
        particleConstraintClass.base.base.new_instance = pcc_new_instance;
        particleConstraintClass.base.base.free = pcc_free_instance;
        particleConstraintClass.class_name = strdup("ParticleConstraint");
        particleConstraintClass.get_name = pcc_get_name;

        particleConstraint_initialized = true;
    }
    printf("ParticleConstraintCreateClass:leave\n");
}




//////////////////////////////////////////////////////////////////
// ParticleCableConstraint
//////////////////////////////////////////////////////////////////
unsigned pccc_addContact(ParticleContactGenerator *self, ParticleContact *contact,
                                   unsigned limit) {
    // Find the length of the cable
    buReal length = pcc_currentLength((ParticleConstraint *)self);

    // Check if we're over-extended
    if (length < ((ParticleCableConstraint *)self)->_maxLength) {
        return 0;
    }

    // Otherwise return the contact
    contact->_particle[0] = ((ParticleConstraint *)self)->_particle;
    contact->_particle[1] = NULL;

    // Calculate the normal
    buVector3 normal = buVector3Difference(
        ((ParticleConstraint *)self)->_anchor,
        INSTANCE_METHOD_AS(ParticleVTable, contact->_particle[0], getPosition)
    );
    normal = buVector3Normalise(normal);
    contact->_contactNormal = normal;

    contact->_penetration = length-((ParticleCableConstraint *)self)->_maxLength;
    contact->_restitution = ((ParticleCableConstraint *)self)->_restitution;

    return 1;
}


ParticleCableConstraintClass particleCableConstraintClass;
ParticleCableConstraintVTable pccc_vtable;

// free object
void pccc_free_instance(const Class *cls, Object *self) {
    printf("ParticleCableConstraint::free_instance:enter\n");
    free(self);
    printf("ParticleCableConstraint::free_instance:leave\n");
}

// new object
static Object *pccc_new_instance(const Class *cls) {
    ParticleCableConstraint *p = malloc(sizeof(ParticleCableConstraint));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *pccc_get_name(ParticleCableConstraintClass *cls) {
    return cls->class_name;
}

static bool particleCableConstraint_initialized = false;
void ParticleCableConstraintCreateClass() {
    printf("ParticleCableConstraintCreateClass:enter\n");
    if (!particleCableConstraint_initialized) {
        printf("ParticleCableConstraintCreateClass:initializing\n");
        ParticleConstraintCreateClass();
        pccc_vtable.base = pcc_vtable; // inherit from VTable

        // methods
        pccc_vtable.base.base.addContact = pccc_addContact;

        // init the particle class
        particleCableConstraintClass.base = particleConstraintClass; // inherit from Class

        particleCableConstraintClass.base.base.base.vtable = (VTable *)&pccc_vtable;
        particleCableConstraintClass.base.base.base.new_instance = pccc_new_instance;
        particleCableConstraintClass.base.base.base.free = pccc_free_instance;
        particleCableConstraintClass.class_name = strdup("ParticleCableConstraint");
        particleCableConstraintClass.get_name = pccc_get_name;

        particleCableConstraint_initialized = true;
    }
    printf("ParticleCableConstraintCreateClass:leave\n");
}


//////////////////////////////////////////////////////////////////
// ParticleRodConstraint
//////////////////////////////////////////////////////////////////
unsigned prc_addContact(ParticleContactGenerator *self, ParticleContact *contact,
                                 unsigned limit){
    // Find the length of the rod
    buReal currentLen = pcc_currentLength((ParticleConstraint *)self);

    // Check if we're over-extended
    if (currentLen == ((ParticleRodConstraint *)self)->_length){
        return 0;
    }

    // Otherwise return the contact
    contact->_particle[0] = ((ParticleConstraint *)self)->_particle;
    contact->_particle[1] = NULL;

    // Calculate the normal
    buVector3 normal = buVector3Difference(
        ((ParticleConstraint *)self)->_anchor,
        INSTANCE_METHOD_AS(ParticleVTable, contact->_particle[0], getPosition)
    );
    normal = buVector3Normalise(normal);

    // The contact normal depends on whether we're extending or compressing
    if (currentLen > ((ParticleRodConstraint *)self)->_length) {
        contact->_contactNormal = normal;
        contact->_penetration = currentLen - ((ParticleRodConstraint *)self)->_length;
    } else {
        contact->_contactNormal = buVector3Scalar(normal, -1);
        contact->_penetration = ((ParticleRodConstraint *)self)->_length - currentLen;
    }

    // Always use zero restitution (no bounciness)
    contact->_restitution = 0;

    return 1;
}

ParticleRodConstraintClass particleRodConstraintClass;
ParticleRodConstraintVTable pcr_vtable;

// free object
void pcr_free_instance(const Class *cls, Object *self) {
    printf("ParticleRodConstraint::free_instance:enter\n");
    free(self);
    printf("ParticleRodConstraint::free_instance:leave\n");
}

// new object
static Object *pcr_new_instance(const Class *cls) {
    ParticleRodConstraint *p = malloc(sizeof(ParticleRodConstraint));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *pcr_get_name(ParticleRodConstraintClass *cls) {
    return cls->class_name;
}

static bool particleRodConstraint_initialized = false;
void ParticleRodConstraintCreateClass() {
    printf("ParticleRodConstraintCreateClass:enter\n");
    if (!particleRodConstraint_initialized) {
        printf("ParticleRodConstraintCreateClass:initializing\n");
        ParticleConstraintCreateClass();
        pcr_vtable.base = pcc_vtable; // inherit from VTable

        // methods
        pcr_vtable.base.base.addContact = prc_addContact;

        // init the particle class
        particleRodConstraintClass.base = particleConstraintClass; // inherit from Class

        particleRodConstraintClass.base.base.base.vtable = (VTable *)&pcr_vtable;
        particleRodConstraintClass.base.base.base.new_instance = pcr_new_instance;
        particleRodConstraintClass.base.base.base.free = pcr_free_instance;
        particleRodConstraintClass.class_name = strdup("ParticleRodConstraint");
        particleRodConstraintClass.get_name = pcr_get_name;

        particleRodConstraint_initialized = true;
    }
    printf("ParticleRodConstraintCreateClass:leave\n");
}
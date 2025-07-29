
#include "../../budgie/oop.h"
#include "../../budgie/cparticle.h"
#include "../../budgie/pfgen.h"
#include "../camera.h"
#include "contact.h"
#include "linalg3x3.h"
#include "cube.h"
#include "../../budgie/random.h"
#include "../../budgie/pcontacts.h"
#include "rlgl.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>



#define CUBE_LENGTH 50.0
#define CUBE_MASS 1.0
#define INITIAL_HEIGHT 100.0
#define DAMPING 1.0

#define TARGET (Vector3){0.0, INITIAL_HEIGHT, 0.0}
#define CAMERA_DISTANCE 300.0
#define CAMERA_PITCH 0.5
#define CAMERA_YAW 0.5

#define MAX_CONTACTS 8

Cube *cube = NULL;
ParticleForceRegistry *forceRegistry = NULL;
Model model;
ParticleContact *contacts[MAX_CONTACTS];
Corner *corners[MAX_CONTACTS];
ParticleContactResolver *contactResolver = NULL;

void initCube() {
    printf("Contact::initCube:enter\n");
    buVector3 initialPosition = (buVector3){0.0, INITIAL_HEIGHT, 0.0};
    buVector3 initialVelocity = buRandomVectorByRange(&(buVector3){-10.0, 10.0, -10.0}, &(buVector3){10.0, 20.0, 10.0});
    buVector3 initialAcceleration = (buVector3){0.0, 0.0, 0.0};
    //Matrix3x3 Lambda = LAMBDA;
    Matrix3x3 initialRotation = buRandomRotationMatrix();
    buVector3 initialOmega_b = buRandomVectorByRange(&(buVector3){1.0, 1.0, 1.0}, &(buVector3){1.0, 1.0, 1.0});
    printf("Contact::initCube:initialOmega_b:(%f, %f, %f)\n",initialOmega_b.x,initialOmega_b.y,initialOmega_b.z);
    INSTANCE_METHOD_AS(CubeVTable, cube, setRigidBody, initialPosition, CUBE_LENGTH, initialVelocity, initialAcceleration, DAMPING, 1.0/CUBE_MASS, initialRotation, initialOmega_b);
    printf("Contact::initCube:leave\n");
}


// Method definitions
void init(Application *self) {
    printf("Contact::init:enter\n");
   

    cube = (Cube *)CLASS_METHOD(&cubeClass,new_instance);
    assert(cube);
    
    initCube();
    // Initialize the force registry
    forceRegistry = (ParticleForceRegistry *)CLASS_METHOD(&particleForceRegistryClass, new_instance);
    assert(forceRegistry); // Check for allocation failure
    // Add gravity force generator 
    ParticleForceGenerator *gravityForce = (ParticleForceGenerator *)CLASS_METHOD_AS(ParticleGravityClass, &particleGravityClass, new_instance, GRAVITY);
    assert(gravityForce); // Check for allocation failure
    INSTANCE_METHOD_AS(ParticleForceRegistryVTable, forceRegistry, add, (Particle *)cube, gravityForce);

    // set camera
    setTarget(TARGET);
    setCameraDistance(CAMERA_DISTANCE);
    setCameraPitch(CAMERA_PITCH);
    setCameraYaw(CAMERA_YAW);

    model = LoadModelFromMesh(GenMeshCube(CUBE_LENGTH, CUBE_LENGTH, CUBE_LENGTH));
    model.materials[0].shader = INSTANCE_METHOD_AS(ApplicationVTable, self, getShader);

    ParticleContactResolverCreateClass();
    ParticleContactCreateClass();
    ParticleContactGeneratorCreateClass();

    contactResolver = (ParticleContactResolver *)CLASS_METHOD(&particleContactResolverClass, new_instance);
    assert(contactResolver); // Check for allocation failure
    INSTANCE_METHOD_AS(ParticleContactResolverVTable, contactResolver, setIterations, 16);

    for (size_t i = 0; i < MAX_CONTACTS; i++) {
        contacts[i] = (ParticleContact *)CLASS_METHOD(&particleContactClass, new_instance);
        assert(contacts[i]); // Check for allocation failure
    }

    for (size_t i = 0; i < MAX_CONTACTS; i++) {
        corners[i] = (Corner *)malloc(sizeof(Corner));
        assert(corners[i]); // Check for allocation failure
    }
    printf("Contact::init:leave\n");
}


void deinitDemo() {

}

const char* getTitle(Application *self) {
    return ((Object *)self)->klass->class_name;
}



void update(Application *self, buReal duration) {
    //printf("Contact::update:enter: duration:%f\n", duration);
    // clear force accumulators
    INSTANCE_METHOD_AS(ParticleVTable, (Particle *)cube, clearAccumulator);
    INSTANCE_METHOD_AS(CubeVTable, cube, clearTorqueAccumulator);


    // update forces
    INSTANCE_METHOD_AS(ParticleForceRegistryVTable, forceRegistry, updateForces, duration);
    
    INSTANCE_METHOD_AS(CubeVTable, cube, integrateRigidBody, duration);

    buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, (Particle *)cube, getPosition);

    size_t numContacts = 0;
    for(size_t i = 0; i < cube->_corners->_length; i++) {
        buVector3 r_b = *(buVector3 *)INSTANCE_METHOD_AS(VectorVTable, cube->_corners, get, i);
        buVector3 r_w = Matrix3x3MultiplyVector(cube->_R, r_b);
        buVector3 rr_w = buVector3Add(r_w, ((Particle *)cube)->_position);



        // Check if corner is below ground
        if (rr_w.y < 0.0) {
            ParticleContact *contact = contacts[numContacts++];
            contact->_particle[0] = (Particle *)cube;
            contact->_particle[1] = NULL; // No second particle
            contact->_contactNormal = (buVector3){0.0, 1.0, 0.0}; // Normal pointing up
            contact->_penetration = -rr_w.y; // Depth of penetration
            contact->_restitution = 0.85; // Example restitution coefficient

            Corner *corner = corners[numContacts - 1];
            corner->r_b = r_b; // Relative position in body frame
            corner->r_w = r_w; // Relative position in world frame
            corner->normal = contact->_contactNormal; // Contact normal
            corner->restitution = 1.0; // Example restitution coefficient
        }

        
    }

    if(numContacts > 0) {
        // Horrid hack to apply impluse to corners below ground
        INSTANCE_METHOD_AS(CubeVTable, cube, applyCornerImpluse, corners, numContacts);

        // Resolve contacts
        INSTANCE_METHOD_AS(ParticleContactResolverVTable, contactResolver, resolveContacts, contacts, numContacts, duration);
    }
}


void display() {
    //printf("Contact::display:enter\n");
    buVector3 p = INSTANCE_METHOD_AS(ParticleVTable, (Particle *)cube, getPosition);
    buVector3 axis;
    buReal angle;
    Matrix3x3ToAxisAngle(cube->_R, &axis, &angle);
    DrawModelEx(model, (Vector3){p.x, p.y, p.z}, (Vector3){axis.x, axis.y, axis.z}, angle * RAD2DEG, (Vector3){1,1,1}, RED);
    //printf("Contact::display:leave\n");
}


void display_info(Application *self, size_t Y, size_t d) {    
}

void keyboard(Application *self, KeyboardKey key) {

}

void mouseButtonPressed(Application *self, MouseButton mouseButton) {

}

ContactVTable contact_vtable = {
    .base = {0}, // base VTable initialized to NULLs
};

static Object *contact_new_instance(const Class *cls) {
    Contact *p = malloc(sizeof(Contact));
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

void contact_free_instance(const Class *cls, Object *self) {
    free(self);
}

static const char *get_name(ContactClass *cls) {
    return cls->class_name;
}


ContactClass contactClass;
ContactVTable contact_vtable;

static bool contact_initialized = false;
void ContactCreateClass() {
    printf("ContactCreateClass:enter\n");
    if (!contact_initialized) {
        printf("ContactCreateClass:initializing\n");
        ApplicationCreateClass();
        CubeCreateClass();
        ParticleGravityCreateClass();
        ParticleForceRegistryCreateClass();
        contact_vtable.base = application_vtable;

        // override application methods
        contact_vtable.base.init = init;
        contact_vtable.base.deinit = deinitDemo;
        contact_vtable.base.getTitle = getTitle;
        contact_vtable.base.update = update;
        contact_vtable.base.display = display;
        contact_vtable.base.display_info = display_info;
        contact_vtable.base.keyboard = keyboard;
        contact_vtable.base.mouseButtonPressed = mouseButtonPressed;

        // init the ballistic class
        contactClass.base = applicationClass;
        contactClass.base.base.vtable = (VTable *)&contact_vtable;
        contactClass.class_name = strdup("Contact");
        contactClass.base.base.new_instance = contact_new_instance;
        contactClass.base.base.free = contact_free_instance;

        contact_initialized = true;
    }
    printf("ContactCreateClass:leave\n");
}

Object *getApplication() {
    ContactCreateClass();
    printf("getApplication: %s\n", contactClass.base.base.class_name);
    return CLASS_METHOD(&contactClass, new_instance);
}
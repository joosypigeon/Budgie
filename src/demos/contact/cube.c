// cpaticle.c
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "cube.h"
#include "linalg3x3.h"


// Method definitions

static void setRigidBody(
                Cube *self,
                buVector3 position,
                buReal length,
                buVector3 velocity,
                buVector3 acceleration, 
                buReal damping,
                buReal inverseMass,
                Matrix3x3 R, // Rotation matrix (body to world)
                buVector3 omega_b // Angular velocity (body frame)
            ) {
    //printf("Cube::setRigidBody:enter\n");
    INSTANCE_METHOD_AS(ParticleVTable, (Particle *)self, set, position, velocity, acceleration, damping, inverseMass);
    assert(inverseMass > 0.0);
    self->_length = length;
    self->_Lambda = Matrix3x3ScalarMultiply(I3, 1.0/6.0*(1/inverseMass)*length*length);
    self->_R = R;
    self->_omega_b = omega_b;
    Vector *corners = (Vector *)CLASS_METHOD(&vectorClass, new_instance);
    float s = length / 2.0f;

    for (int dx = -1; dx <= 1; dx += 2) {
        for (int dy = -1; dy <= 1; dy += 2) {
            for (int dz = -1; dz <= 1; dz += 2) {
                buVector3 *corner = malloc(sizeof(buVector3));
                assert(corner); // Check for allocation failure
                corner->x = dx * s;
                corner->y = dy * s;
                corner->z = dz * s;
                //printf("Cube::setRigidBody:corner:(%f, %f, %f)\n", corner->x, corner->y, corner->z);
                INSTANCE_METHOD_AS(VectorVTable, corners, push, corner);
            }
        }
    }
    self->_corners = corners;
    //printf("Cube::setRigidBody:leave\n");
}


static void addTorque(Cube *self, const buVector3 torque) {
    self->_torqueAccum = buVector3Add(self->_torqueAccum, torque);
}

static buVector3 getTorqueAccum(Cube *self) {
    return self->_torqueAccum;
} 

static void clearTorqueAccumulator(Cube *self) {
    self->_torqueAccum = (buVector3){0.0, 0.0, 0.0};
}

static void checkCorners(Cube *self) {
    //printf("Cube::checkCorners:enter\n");
    for (size_t i = 0; i < self->_corners->_length; i++) {
        buVector3 *corner = INSTANCE_METHOD_AS(VectorVTable, self->_corners, get, i);
        buVector3 worldCorner = Matrix3x3MultiplyVector(self->_R, *corner);
        worldCorner = buVector3Add(worldCorner, ((Particle *)self)->_position);
        // Check if corner is below ground
        if (worldCorner.y < 0.0) {
            // Reset position to initial position
            printf("Cube::checkCorners:corner %zu is below ground, (%f, %f, %f)\n", i, worldCorner.x, worldCorner.y, worldCorner.z);
        }
    }
    //printf("Cube::checkCorners:leave\n");
}


void integrateRigidBody(Cube *self, buReal duration) {
    //printf("cube::integrateRigidBody:enter: duration:%f\n", duration);
    // --- Linear motion ---
    buReal inverseMass = INSTANCE_METHOD_AS(ParticleVTable, (Particle *)self, getInverseMass);
    buVector3 force = INSTANCE_METHOD_AS(ParticleVTable, (Particle *)self, getForceAccum);
    INSTANCE_METHOD_AS(ParticleVTable, (Particle *)self, setAcceleration, buVector3Scalar(force, inverseMass));
    INSTANCE_METHOD_AS(ParticleVTable, (Particle *)self, integrate, duration);
    // --- Angular motion in body frame ---
    buVector3 domega_b;

    // Compute Lambda⁻¹ * (τ_b - ω_b × (Lambda * ω_b))
    buVector3 Iw = Matrix3x3MultiplyVector(self->_Lambda, self->_omega_b);
    buVector3 cross = buVector3Cross(self->_omega_b, Iw);
    buVector3 rhs = buVector3Difference(self->_torqueAccum, cross);
    domega_b = buDivideVectorComponents(rhs, Matrix3x3Diagonal(self->_Lambda));
    //printf("domega_b:(%f, %f, %f)\n",domega_b.x,domega_b.y,domega_b.z);
    // Integrate angular velocity
    self->_omega_b = buVector3Add(self->_omega_b, buVector3Scalar(domega_b, duration));

    //printf("self->_omega_b: (%f, %f, %f)\n",self->_omega_b.x,self->_omega_b.y,self->_omega_b.z);
    // --- Update orientation matrix ---
    // Convert ω_b to world frame: ω = R * ω_b
    buVector3 omega_world = Matrix3x3MultiplyVector(self->_R, self->_omega_b);

    // Skew-symmetric matrix [ω]^
    Matrix3x3 omega_skew = {
        0,              -omega_world.z, omega_world.y,
        omega_world.z,  0,              -omega_world.x,
        -omega_world.y, omega_world.x,  0 };
    // R(t+dt) ≈ R(t) + dt * [ω]^ * R(t)
    Matrix3x3 dR = Matrix3x3Multiply(omega_skew, self->_R);

    self->_R = Matrix3x3Add(self->_R, Matrix3x3ScalarMultiply(dR, duration));

    // Re-orthonormalize R here to prevent drift
    self->_R = Matrix3x3Reorthonormalize(self->_R);

    //checkCorners(self);

    //printf("cube::integrateRigidBody:leave\n");
}

// This is a horrid hack to apply impluse to corners below ground
static void applyCornerImpluse(
        Cube *self,
        Corner **corners,
        unsigned numContacts) {
    //printf("Cube::applyCornerImpluse:enter: numContacts:%u\n", numContacts);
    // In the world frame, the inertia tensor is I = R * Lambda * R^T
    // where R is the rotation matrix and Lambda is the inertia tensor in the body frame.
    // For a cube, the inertia tensor in the body frame is diagonal:
    // Lambda = diag(1/6 * m * l^2, 1/6 * m * l^2, 1/6 * m * l^2)
    // where m is the mass and l is the length of the cube.
    // Hence in the world frame, the inertia tensor is:
    // I = R * 1/6 * m * l^2 * I_3 * R^T = 1/6 * m * l^2 * (R * I_3 * R^T)
    // where I_3 is the 3x3 identity matrix.
    // So I = Lambda for a cube.
    // Hence I_inv = 6 / (m * l^2) * I_3
    buReal m = INSTANCE_METHOD_AS(ParticleVTable, (Particle *)self, getMass);
    buReal l = self->_length;
    buReal lamnda_inv =  6.0 / (m * l * l);
    buVector3 l_inv = (buVector3){lamnda_inv, lamnda_inv, lamnda_inv};


    buVector3 omega_w = Matrix3x3MultiplyVector(self->_R, self->_omega_b);
    buVector3 velocity = INSTANCE_METHOD_AS(ParticleVTable, (Particle *)self, getVelocity);
    buVector3 cubeCenter = INSTANCE_METHOD_AS(ParticleVTable, (Particle *)self, getPosition);

    for(size_t i = 0; i < numContacts; i++) {
        Corner *corner = corners[i];

        buVector3 n = corner->normal; // Contact normal
        buVector3 r_b = corner->r_b; // Relative position from cube center to corner in body frame
        buVector3 r_w = corner->r_w; // Relative position from cube center to corner in world frame
        buVector3 cornerVelocity = buVector3Add(velocity, buVector3Cross(omega_w, r_w));
        buReal J_n =-(1.0 + corner->restitution) * buVector3Dot(cornerVelocity, n)
                / (1/m + buVector3Dot(
                            n,
                            buVector3Cross(
                                buVector3ComponentProduct(
                                    l_inv,
                                    buVector3Cross(r_w, n)),
                                r_w)));
        buVector3 J_w = buVector3Scalar(n, J_n);
        buVector3 J_b = Matrix3x3MultiplyVector(Matrix3x3Transpose(self->_R), J_w);
        buVector3 delta_omega_b = buVector3ComponentProduct(l_inv, buVector3Cross(r_b, J_b));

        printf("Cube::applyCornerImpluse:corner r_w:(%f, %f, %f) r_b:(%f, %f, %f) J_n:%f J_w:(%f, %f, %f) J_b:(%f, %f, %f) delta_omega_b:(%f, %f, %f)\n",
            r_w.x, r_w.y, r_w.z,
            r_b.x, r_b.y, r_b.z,
            J_n,
            J_w.x, J_w.y, J_w.z,
            J_b.x, J_b.y, J_b.z,
            delta_omega_b.x, delta_omega_b.y, delta_omega_b.z);

        self->_omega_b = buVector3Add(self->_omega_b, delta_omega_b);
    }  
    //printf("Cube::applyCornerImpluse:leave\n"); 
}

CubeClass cubeClass;
CubeVTable cube_vtable;

// free object
void cube_free_instance(const Class *cls, Object *self) {
    printf("Particle::free_instance:enter\n");
    Cube *cube = (Cube *)self;
    CLASS_METHOD(&vectorClass, free, (Object *)cube->_corners);
    free(self);
    printf("Particle::free_instance:leave\n");
}

// new object
static Object *cube_new_instance(const Class *cls) {
    Cube *p = malloc(sizeof(Cube));
    assert(p);  // Check for allocation failure
    ((Object *)p)->klass = cls;
    return (Object *)p;
}

static const char *get_name(CubeClass *cls) {
    return cls->class_name;
}

static bool cube_initialized = false;
void CubeCreateClass() {
    printf("CubeCreateClass:enter\n");
    if (!cube_initialized) {
        printf("CubeCreateClass:initializing\n");
        ParticleCreateClass();
        cube_vtable.base = particle_vtable; // inherit from VTable

        // methods
        cube_vtable.integrateRigidBody = integrateRigidBody;
        cube_vtable.setRigidBody = setRigidBody;
        cube_vtable.addTorque = addTorque;
        cube_vtable.getTorqueAccum = getTorqueAccum;
        cube_vtable.clearTorqueAccumulator = clearTorqueAccumulator;
        cube_vtable.applyCornerImpluse = applyCornerImpluse;


        // init the particle class
        cubeClass.base = particleClass; // inherit from Class
        cubeClass.base.base.vtable = (VTable *)&cube_vtable;
        cubeClass.base.base.new_instance = cube_new_instance;
        cubeClass.base.base.free = cube_free_instance;
        cubeClass.class_name = strdup("Cube");
        cubeClass.get_name = get_name;

        cube_initialized = true;
    }
    printf("CubeCreateClass:leave\n");
}


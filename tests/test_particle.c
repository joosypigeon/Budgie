#include "unity/src/unity.h"
#include "../src/budgie/core.h"
#include "../src/budgie/particle.h"
#include <math.h>

#define EPSILON 1e-6

void setUp(void) {}
void tearDown(void) {}

void test_buVector3ParticleIntegrate_basic_motion(void) {
    buVector3Particle p = {
        .position = {0.0, 0.0, 0.0},
        .velocity = {1.0, 0.0, 0.0},
        .acceleration = {0.0, 0.0, 0.0},
        .damping = 1.0,
        .inverseMass = 1.0
    };

    buVector3ParticleIntegrate(&p, 1.0);

    // Position should advance by velocity * duration
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, p.position.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, p.position.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, p.position.z);

    // Velocity should remain unchanged
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, p.velocity.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, p.velocity.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, p.velocity.z);
}

void test_buVector3ParticleIntegrate_with_acceleration(void) {
    buVector3Particle p = {
        .position = {0.0, 0.0, 0.0},
        .velocity = {0.0, 0.0, 0.0},
        .acceleration = {1.0, 0.0, 0.0},
        .damping = 1.0,
        .inverseMass = 1.0
    };

    buVector3ParticleIntegrate(&p, 2.0);

    // Velocity = a * t = 2.0
    // Position = 0 + velocity * t = 0 + 0.0 * 2.0 = 0.0
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, p.position.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, p.position.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, p.position.z);

    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 2.0, p.velocity.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, p.velocity.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, p.velocity.z);
}

void test_buVector3ParticleIntegrate_with_damping(void) {
    buVector3Particle p = {
        .position = {0.0, 0.0, 0.0},
        .velocity = {1.0, 0.0, 0.0},
        .acceleration = {0.0, 0.0, 0.0},
        .damping = 0.5,
        .inverseMass = 1.0
    };

    buVector3ParticleIntegrate(&p, 1.0);

    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, p.position.x);     // p = v * dt = 1 * 1
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.5, p.velocity.x);     // v *= damping^dt = 1 * 0.5^1
}

void test_buVector3KineticEnergy_basic(void) {
    buVector3Particle p = {
        .velocity = {2.0, 0.0, 0.0},
        .inverseMass = 0.5  // mass = 2.0
    };

    buReal ke = buVector3KineticEnergy(&p);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.5 * 2.0 * 2.0 * 2.0, ke);  // 0.5 * m * v^2
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_buVector3ParticleIntegrate_basic_motion);
    RUN_TEST(test_buVector3ParticleIntegrate_with_acceleration);
    RUN_TEST(test_buVector3ParticleIntegrate_with_damping);
    RUN_TEST(test_buVector3KineticEnergy_basic);
    return UNITY_END();
}

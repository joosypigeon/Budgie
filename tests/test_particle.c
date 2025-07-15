#include "../tests/unity/src/unity.h"
#include "../src/budgie/core.h"
#include "../src/budgie/oop.h"
#include "../src/budgie/cparticle.h"
#include <math.h>

#define EPSILON 1e-6

void setUp(void) {}
void tearDown(void) {}

void test_integrate_basic_motion(void) {
    Particle *p = (Particle *)CLASS_METHOD(&particleClass,new_instance);
    INSTANCE_METHOD_AS(ParticleVTable, p, set, (buVector3){0.0, 0.0, 0.0}, (buVector3){1.0, 0.0, 0.0}, (buVector3){0.0, 0.0, 0.0}, 1.0, 1.0);
    INSTANCE_METHOD_AS(ParticleVTable, p, integrate, 1.0);
    buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, p, getPosition);
    buVector3 velocity = INSTANCE_METHOD_AS(ParticleVTable, p, getVelocity);

    // Position should advance by velocity * duration
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, position.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, position.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, position.z);

    // Velocity should remain unchanged
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, velocity.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, velocity.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, velocity.z);

    CLASS_METHOD(&particleClass, free, (Object *)p);
}

void test_integrate_with_acceleration(void) {
    Particle *p = (Particle *)CLASS_METHOD(&particleClass,new_instance);
    INSTANCE_METHOD_AS(ParticleVTable, p, set, (buVector3){0.0, 0.0, 0.0}, (buVector3){0.0, 0.0, 0.0}, (buVector3){2.0, 0.0, 0.0}, 1.0, 1.0);
    INSTANCE_METHOD_AS(ParticleVTable, p, integrate, 1.0);
    buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, p, getPosition);
    buVector3 velocity = INSTANCE_METHOD_AS(ParticleVTable, p, getVelocity);

    // Velocity = a * t = 2.0
    // Position = 0 + velocity * t = 0 + 0.0 * 2.0 = 0.0
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, position.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, position.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, position.z);

    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 2.0, velocity.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, velocity.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, velocity.z);

    CLASS_METHOD((Object *)&particleClass, free, (Object *)p);
}

void test_integrate_with_damping(void) {
    Particle *p = (Particle *)CLASS_METHOD(&particleClass,new_instance);
    INSTANCE_METHOD_AS(ParticleVTable, p, set, (buVector3){0.0, 0.0, 0.0}, (buVector3){1.0, 0.0, 0.0}, (buVector3){1.0, 0.0, 0.0}, 0.5, 1.0);
    INSTANCE_METHOD_AS(ParticleVTable, p, integrate, 1.0);
    buVector3 position = INSTANCE_METHOD_AS(ParticleVTable, p, getPosition);
    buVector3 velocity = INSTANCE_METHOD_AS(ParticleVTable, p, getVelocity);

    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, position.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, velocity.x);

    CLASS_METHOD(&particleClass, free, (Object *)p);
}



int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_integrate_basic_motion);
    RUN_TEST(test_integrate_with_acceleration);
    RUN_TEST(test_integrate_with_damping);
    return UNITY_END();
}

#include "unity/src/unity.h"
#include "../src/budgie/core.h"
#include <math.h>

#define EPSILON 1e-6

void setUp(void) {}
void tearDown(void) {}

void test_buVector3Norm(void) {
    buVector3 v = {3.0, 4.0, 0.0};
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 5.0, buVector3Norm(v));
}

void test_buVector3SquareNorm(void) {
    buVector3 v = {1.0, 2.0, 2.0};
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 9.0, buVector3SquareNorm(v));
}

void test_buVector3Normalise(void) {
    buVector3 v = {0.0, 3.0, 4.0};
    buVector3 u = buVector3Normalise(v);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, u.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.6, u.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.8, u.z);
}

void test_buVector3Scalar(void) {
    buVector3 v = {1.0, -2.0, 0.5};
    buVector3 s = buVector3Scalar(v, 2.0);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 2.0, s.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, -4.0, s.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, s.z);
}

void test_buVector3Add(void) {
    buVector3 a = {1.0, 2.0, 3.0};
    buVector3 b = {4.0, 5.0, 6.0};
    buVector3 r = buVector3Add(a, b);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 5.0, r.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 7.0, r.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 9.0, r.z);
}

void test_buVector3Difference(void) {
    buVector3 a = {5.0, 4.0, 3.0};
    buVector3 b = {1.0, 1.0, 1.0};
    buVector3 r = buVector3Difference(a, b);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 4.0, r.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 3.0, r.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 2.0, r.z);
}

void test_buVector3ComponentProduct(void) {
    buVector3 a = {1.0, 2.0, 3.0};
    buVector3 b = {4.0, 0.5, -1.0};
    buVector3 r = buVector3ComponentProduct(a, b);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 4.0, r.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, r.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, -3.0, r.z);
}

void test_buVector3Dot(void) {
    buVector3 a = {1.0, 2.0, 3.0};
    buVector3 b = {4.0, -5.0, 6.0};
    buReal d = buVector3Dot(a, b);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 12.0, d);  // 1*4 + 2*(-5) + 3*6 = 12
}

void test_buVector3Cross(void) {
    buVector3 a = {1.0, 0.0, 0.0};
    buVector3 b = {0.0, 1.0, 0.0};
    buVector3 c = buVector3Cross(a, b);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, c.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, c.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, c.z);
}

void test_buMakeVector3OrthonormalBasis_XYZ_orthonormal(void) {
    buVector3 u = {1.0, 2.0, 3.0};
    buVector3 v = {4.0, 5.0, 6.0};
    buVector3 w = {0.0, 0.0, -1.0};

    buCoordinateFrame f = buMakeVector3OrthonormalBasis(u, v, w);

    // Lengths should be 1
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, buVector3Norm(f.X));
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, buVector3Norm(f.Y));
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, buVector3Norm(f.Z));

    // Vectors should be orthogonal (dot product 0)
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, buVector3Dot(f.X, f.Y));
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, buVector3Dot(f.X, f.Z));
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, buVector3Dot(f.Y, f.Z));

    // Z should point in same general direction as w
    TEST_ASSERT(buVector3Dot(f.Z, w) > 0.0);
}

void test_buMakeVector3OrthonormalBasis_known_axes(void) {
    buVector3 u = {1.0, 0.0, 0.0};
    buVector3 v = {0.0, 1.0, 0.0};
    buVector3 w = {0.0, 0.0, 1.0};

    buCoordinateFrame f = buMakeVector3OrthonormalBasis(u, v, w);

    // Expect standard basis
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, f.X.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, f.X.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, f.X.z);

    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, f.Y.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, f.Y.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, f.Y.z);

    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, f.Z.x);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 0.0, f.Z.y);
    TEST_ASSERT_FLOAT_WITHIN(EPSILON, 1.0, f.Z.z);
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_buVector3Norm);
    RUN_TEST(test_buVector3SquareNorm);
    RUN_TEST(test_buVector3Normalise);
    RUN_TEST(test_buVector3Scalar);
    RUN_TEST(test_buVector3Add);
    RUN_TEST(test_buVector3Difference);
    RUN_TEST(test_buVector3ComponentProduct);
    RUN_TEST(test_buVector3Dot);
    RUN_TEST(test_buVector3Cross);
    RUN_TEST(test_buMakeVector3OrthonormalBasis_XYZ_orthonormal);
    RUN_TEST(test_buMakeVector3OrthonormalBasis_known_axes);
    return UNITY_END();
}

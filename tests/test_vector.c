#include "unity/src/unity.h"
#include "../src/budgie/vector.h"
#include "../src/budgie/oop.h"
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void test_push_and_getLength(void) {
    Vector *v = (Vector *)CLASS_METHOD(&vectorClass, new_instance);
    TEST_ASSERT_EQUAL_UINT32(0, INSTANCE_METHOD_AS(VectorVTable, v, getLength));

    int a = 42;
    INSTANCE_METHOD_AS(VectorVTable, v, push, &a);
    TEST_ASSERT_EQUAL_UINT32(1, INSTANCE_METHOD_AS(VectorVTable, v, getLength));

    int b = 77;
    INSTANCE_METHOD_AS(VectorVTable, v, push, &b);
    TEST_ASSERT_EQUAL_UINT32(2, INSTANCE_METHOD_AS(VectorVTable, v, getLength));

    CLASS_METHOD(&vectorClass, free, (Object *)v);
}

void test_get_and_set(void) {
    Vector *v = (Vector *)CLASS_METHOD(&vectorClass, new_instance);

    int a = 123, b = 456;
    INSTANCE_METHOD_AS(VectorVTable, v, push, &a);
    INSTANCE_METHOD_AS(VectorVTable, v, push, &b);

    int *a_ptr = INSTANCE_METHOD_AS(VectorVTable, v, get, 0);
    int *b_ptr = INSTANCE_METHOD_AS(VectorVTable, v, get, 1);
    TEST_ASSERT_EQUAL_PTR(&a, a_ptr);
    TEST_ASSERT_EQUAL_PTR(&b, b_ptr);

    int c = 789;
    INSTANCE_METHOD_AS(VectorVTable, v, set, 0, &c);
    int *c_ptr = INSTANCE_METHOD_AS(VectorVTable, v, get, 0);
    TEST_ASSERT_EQUAL_PTR(&c, c_ptr);

    CLASS_METHOD(&vectorClass, free, (Object *)v);
}

void test_pop(void) {
    Vector *v = (Vector *)CLASS_METHOD(&vectorClass, new_instance);

    int x = 10, y = 20;
    INSTANCE_METHOD_AS(VectorVTable, v, push, &x);
    INSTANCE_METHOD_AS(VectorVTable, v, push, &y);

    void *y_out = INSTANCE_METHOD_AS(VectorVTable, v, pop);
    TEST_ASSERT_EQUAL_PTR(&y, y_out);
    TEST_ASSERT_EQUAL_UINT32(1, INSTANCE_METHOD_AS(VectorVTable, v, getLength));

    void *x_out = INSTANCE_METHOD_AS(VectorVTable, v, pop);
    TEST_ASSERT_EQUAL_PTR(&x, x_out);
    TEST_ASSERT_EQUAL_UINT32(0, INSTANCE_METHOD_AS(VectorVTable, v, getLength));

    void *null_out = INSTANCE_METHOD_AS(VectorVTable, v, pop);
    TEST_ASSERT_NULL(null_out);

    CLASS_METHOD(&vectorClass, free, (Object *)v);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_push_and_getLength);
    RUN_TEST(test_get_and_set);
    RUN_TEST(test_pop);
    return UNITY_END();
}

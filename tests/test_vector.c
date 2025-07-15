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

void test_general_operations(void) {
    Vector *v = (Vector *)CLASS_METHOD(&vectorClass, new_instance);

    // Push some string literals
    INSTANCE_METHOD_AS(VectorVTable, v, push, "Apple");
    INSTANCE_METHOD_AS(VectorVTable, v, push, "Banna");
    INSTANCE_METHOD_AS(VectorVTable, v, push, "Cherry");

    TEST_ASSERT_EQUAL_STRING("Apple", (char *)INSTANCE_METHOD_AS(VectorVTable, v, get, 0));
    TEST_ASSERT_EQUAL_STRING("Banna", (char *)INSTANCE_METHOD_AS(VectorVTable, v, get, 1));
    TEST_ASSERT_EQUAL_STRING("Cherry", (char *)INSTANCE_METHOD_AS(VectorVTable, v, get, 2));

    TEST_ASSERT_EQUAL_UINT32(3, INSTANCE_METHOD_AS(VectorVTable, v, getLength));

    INSTANCE_METHOD_AS(VectorVTable, v, set, 1, "Blueberry");

    TEST_ASSERT_EQUAL_STRING("Blueberry", (char *)INSTANCE_METHOD_AS(VectorVTable, v, get, 1));

    // Clean up
    CLASS_METHOD(&vectorClass, free, (Object *)v);
}

void test_large_vector(void) {
    const size_t N = 10000;
    Vector *v = (Vector *)CLASS_METHOD(&vectorClass, new_instance);

    // Allocate and store N integers
    int *data = malloc(N * sizeof(int));
    for (size_t i = 0; i < N; ++i) {
        data[i] = (int)i;
        INSTANCE_METHOD_AS(VectorVTable, v, push, &data[i]);
    }

    // Check the length is as expected
    TEST_ASSERT_EQUAL_UINT32(N, INSTANCE_METHOD_AS(VectorVTable, v, getLength));

    // Spot-check some values
    TEST_ASSERT_EQUAL_INT(0, *(int *)INSTANCE_METHOD_AS(VectorVTable, v, get, 0));
    TEST_ASSERT_EQUAL_INT(1234, *(int *)INSTANCE_METHOD_AS(VectorVTable, v, get, 1234));
    TEST_ASSERT_EQUAL_INT(9999, *(int *)INSTANCE_METHOD_AS(VectorVTable, v, get, 9999));

    // Set a new value in the middle
    int sentinel = -12345;
    INSTANCE_METHOD_AS(VectorVTable, v, set, 5000, &sentinel);
    TEST_ASSERT_EQUAL_PTR(&sentinel, INSTANCE_METHOD_AS(VectorVTable, v, get, 5000));

    // Pop all items (ensuring nothing crashes)
    for (size_t i = 0; i < N; ++i) {
        INSTANCE_METHOD_AS(VectorVTable, v, pop);
    }
    TEST_ASSERT_EQUAL_UINT32(0, INSTANCE_METHOD_AS(VectorVTable, v, getLength));

    // Clean up
    CLASS_METHOD(&vectorClass, free, (Object *)v);
    free(data);
}
    

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_push_and_getLength);
    RUN_TEST(test_get_and_set);
    RUN_TEST(test_pop);
    RUN_TEST(test_general_operations);
    RUN_TEST(test_large_vector);
    return UNITY_END();
}

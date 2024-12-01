#include "CVector.h"
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

extern void compact(Vector* v, bool force);
extern bool isSlotFree(Vector* v, int index);

void test_vector_init() {
    Vector* vector = createVector(10);
    assert(vector->size == 0);
    assert(vector->capacity == 10);
    for (int i = 0; i < vector->capacity; i++) {
        assert(isSlotFree(vector, i));
    }
    freeVector(vector);
    printf("test_vector_init passed\n");
}
void test_vector_add() {
    Vector* vector = createVector(10);
    vector_add(vector, 10);
    vector_add(vector, 20);
    vector_add(vector, 30);
    assert(vector->size == 3);
    assert(vector_get(vector, 0) == 10);
    assert(vector_get(vector, 1) == 20);
    assert(vector_get(vector, 2) == 30);
    freeVector(vector);
    printf("test_vector_add passed\n");
}

void test_vector_remove() {
    Vector* vector = createVector(10);
    vector_add(vector, 10);
    vector_add(vector, 20);
    vector_add(vector, 30);
    vector_remove(vector, 1);
    assert(vector->size == 2);
    assert(vector_get(vector, 0) == 10);
    assert(vector_get(vector, 1) == 30);
    // Try removing last remaining element
    vector_remove(vector, 0);
    assert(vector->size == 1);
    assert(vector_get(vector, 0) == 30);
    freeVector(vector);
    printf("test_vector_remove passed\n");
}

void test_vector_resize() {
    Vector* vector = createVector(10);
    for (int i = 0; i < 20; i++) {
        vector_add(vector, i);
    }
    assert(vector->size == 20);
    for (int i = 0; i < 20; i++) {
        assert(vector_get(vector, i) == i);
    }
    freeVector(vector);
    printf("test_vector_resize passed\n");
}

void test_vector_compact() {
    Vector* vector = createVector(10);
    vector_add(vector, 10);
    vector_add(vector, 20);
    vector_add(vector, 30);
    vector_add(vector, 40);
    vector_add(vector, 50);
    assert(vector->maxUsedIndex == 4);
    int list[2] = {1, 3};
    deleteElementsByIndex(vector, list, 2);
    assert(vector->size == 3);
    assert(vector->maxUsedIndex == 4);
    compact(vector, true);
    assert(vector->maxUsedIndex == 2);
    assert(vector->data[0] == 10);
    assert(vector->data[1] == 30);
    assert(vector->data[2] == 50);
    // Try removing last remaining element
    vector_remove(vector, 2);
    assert(vector->maxUsedIndex == 1);

    freeVector(vector);
    printf("test_vector_compact passed\n");
}

int main() {
    test_vector_init();
    test_vector_add();
    test_vector_remove();
    test_vector_resize();
    test_vector_compact();
    printf("All tests passed!\n");
    return 0;
}